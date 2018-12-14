#include <map>
#include <iostream>
#include <string>
#include <optional>
#include <sstream>
#include <vector>
#include "temp_rdbuf.h"
#include <chrono>

int node_id;
int node_count;

int NumberOfNodes() { return node_count; }

int MyNodeId() { return node_id; }

struct Message 
{
    int id;
    std::chrono::high_resolution_clock::duration timestamp;
    std::vector<unsigned char> data;
};

namespace duration
{
    std::chrono::high_resolution_clock::time_point measure_start;
    std::chrono::high_resolution_clock::duration duration;

    void start()
    {
        measure_start = std::chrono::high_resolution_clock::now();
        duration = {};
    }

    void update()
    {
        auto measure_stop = std::chrono::high_resolution_clock::now();
        duration += (measure_stop - measure_start);
        measure_start = measure_stop;
    }

    void update(std::chrono::high_resolution_clock::duration d)
    {
        if (d > duration)
            duration = d;

        measure_start = std::chrono::high_resolution_clock::now();
    }

    std::chrono::milliseconds stop()
    {
        update();
        return std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    }
}

using Messages = std::vector<Message>;

int next_message_id = 1;
std::vector<Messages*> node_write_buffers;
std::vector<int> node_write_indices;
std::vector<int> node_write_offsets;

std::vector<const Messages*> node_read_buffers;
std::vector<int> node_read_indices;
std::vector<int> node_read_offsets;

struct message_not_recieved {};

template<typename T>
void PutImpl(int target, T value)
{
    Messages *messages = node_write_buffers.at(target);
    const int index = node_write_indices.at(target);
    int &offset = node_write_offsets.at(target);

    if (index > messages->size())
    {
        std::cerr << "    Internal error: index > messages->size()" << std::endl;
        throw 42;
    }

    if (index == messages->size())
    {
        std::cerr << "    Creating new message to #" << target << std::endl;
        messages->push_back(Message{next_message_id++, {}});
    }

    auto &message = messages->at(index).data;

    unsigned char *data = reinterpret_cast<unsigned char*>(&value);

    if (offset+sizeof(T) <= message.size())
    {
        if (memcmp(message.data()+offset, data, sizeof(T)) != 0)
        {
            std::cerr << "    Wrote different message than before!" << std::endl;
            throw 42;
        }
    }
    else if (offset == message.size())
    {
        for (int i = 0; i < sizeof(T); ++i)
        {
            message.push_back(data[i]);
        }
    }
    else
    {
        std::cerr << "    Internal error: wrong offset." << std::endl;
        throw 42;
    }

    offset += sizeof(T);
}

void PutInt(int target, int value) { PutImpl<int>(target, value); }
void PutLL(int target, long long value) { PutImpl<long long>(target, value); }

void Send(int target)
{
    duration::update();

    Messages *messages = node_write_buffers.at(target);
    int &index = node_write_indices.at(target);

    Message &message = messages->at(index);
    message.timestamp = duration::duration;

    index += 1;
    node_write_offsets.at(target) = 0;

    std::cerr << "    @ Sent at " << std::chrono::duration_cast<std::chrono::milliseconds>(duration::duration).count() << std::endl;
}

template<typename T>
T GetImpl(int source)
{
    const Messages *messages = node_read_buffers.at(source);
    const int index = node_read_indices.at(source);
    int &offset = node_read_offsets.at(source);

    const auto &message = messages->at(index).data;
    if (offset+sizeof(T) > message.size())
    {
        std::cerr << "    Reading message after it ended." << std::endl;
        std::cerr << "      reading at " << offset << std::endl;
        std::cerr << "      reading length " << sizeof(T) << std::endl;
        std::cerr << "      message length " << message.size() << std::endl;
        throw 42;
    }

    T ret;
    memcpy(reinterpret_cast<unsigned char*>(&ret), message.data()+offset, sizeof(T));
    offset += sizeof(T);
    return ret;
}

int GetInt(int source) { return GetImpl<int>(source); }
long long GetLL(int source) { return GetImpl<long long>(source); }

int Receive(int source)
{
    duration::update();

    std::cerr << "    @ Receive started at " << std::chrono::duration_cast<std::chrono::milliseconds>(duration::duration).count() << std::endl;

    if (source == -1)
    {
        int best_source = -1;
        int best_id = next_message_id;

        for (int i = 0; i < node_count; ++i)
        {
            const Messages *messages = node_read_buffers.at(i);
            const int index = node_read_indices.at(i);
            const int offset = node_read_offsets.at(i);

            if (index >= 0) 
            {
                const auto &prev_message = messages->at(index).data;

                if (offset != prev_message.size())
                {
                    std::cerr << "    Receive(-1) called before reading all recieved messages" << std::endl;
                    throw 42;
                }
            }

            if (index+1 >= messages->size())
                continue;

            const int next_id = messages->at(index+1).id;

            if (next_id < best_id)
            {
                best_id = next_id;
                best_source = i;
            }
        }

        if (best_source == -1)
        {
            std::cerr << "    Receive(-1) did not find source" << std::endl;
            throw message_not_recieved{};
        }
        
        source = best_source;
    }

    const Messages *messages = node_read_buffers.at(source);
    int &index = node_read_indices.at(source);
    index += 1;

    node_read_offsets.at(source) = 0;

    if (index >= messages->size())
    {
        std::cerr << "    Receive(), but message not yet recieved" << std::endl;
        throw message_not_recieved{};
    }

    const Message &message = messages->at(index);

    duration::update(message.timestamp);

    std::cerr << "    @ Receive completed at " << std::chrono::duration_cast<std::chrono::milliseconds>(duration::duration).count() << std::endl;

    return source;
}

void run();

int main(const int argc, const char * const argv[]) try
{
    using namespace std;

    {
        std::stringstream node_count_ss;

        for (int i = 1; i < argc; ++i)
            node_count_ss << argv[i] << ' ';

        node_count_ss << 100;

        node_count_ss >> node_count;
    }

    vector<Messages> buffers(node_count*node_count);

    node_write_buffers.resize(node_count);
    node_write_indices.resize(node_count);
    node_write_offsets.resize(node_count);

    node_read_buffers.resize(node_count);
    node_read_indices.resize(node_count);
    node_read_offsets.resize(node_count);

    optional<string> answer;

    bool last_run = false;
    int previous_next_id = -1;

    vector<bool> is_done(node_count, false);

    while (true)
    {
        cerr << "Starting new sequence..." << endl;
        answer = {};

        if (!last_run && previous_next_id == next_message_id)
            throw std::logic_error("No messages sent, dead-lock.");

        previous_next_id = next_message_id;

        std::chrono::milliseconds max_duration = {};

        bool success = true;

        for(node_id = 0; node_id < node_count; ++node_id)
        {
            if (!last_run && is_done[node_id] == true) continue;

            for (int i = 0; i < node_count; ++i)
            {
                node_write_buffers.at(i) = &buffers[node_id * node_count + i];
                node_write_indices.at(i) = 0;
                node_write_offsets.at(i) = 0;

                node_read_buffers.at(i) = &buffers[i * node_count + node_id];
                node_read_indices.at(i) = -1;
                node_read_offsets.at(i) = 0;
            }

            stringstream ss;

            cerr << "  Running instance #" << node_id << "..." << endl;
            std::chrono::milliseconds dur;

            {
                temporary_rdbuf _(cout, ss.rdbuf());
                try
                {
                    duration::start();
                    run();
                    dur = duration::stop();
                    is_done[node_id] = true;
                }
                catch(message_not_recieved)
                {
                    success = false;
                    continue;
                }

                if (last_run)
                {
                    cerr << "    Took " << dur.count() << " milliseconds." << endl;

                    if (dur > max_duration)
                        max_duration = dur;
                }
            }

            string output = ss.str();
            if (!output.empty())
            {
                if (answer.has_value())
                {
                    cerr << "    Got second output. Failure!" << endl;
                    return EXIT_FAILURE;
                }

                cerr << "    Got output." << endl;
                answer = output;
            }

        }

        if (last_run)
        {
            if (!success)
            {
                cerr << "  Failure during last run!" << endl;
            }

            cerr << "  Took " << max_duration.count() << " milliseconds." << endl;

            break;
        }

        if (success)
        {
            cerr << "  Sequence success." << endl;
            last_run = true;
        }
    }

    if (!answer.has_value())
    {
        cerr << "No answer. Failure!" << endl;
        return EXIT_FAILURE;
    }

    cerr << "Got answer:" << endl;
    cout << *answer << flush;
}
catch(std::exception &ex)
{
    std::cerr << "Run failed with exception: " << ex.what() << std::endl;
}
catch(...)
{
    std::cerr << "Run failed with non-exception throwable." << std::endl;
}