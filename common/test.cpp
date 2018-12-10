#include <gtest/gtest.h>
#include <list>
#include <filesystem>
#include <memory>
#include <fstream>
#include <iterator>
#include <algorithm>

namespace std { namespace filesystem {} }
namespace std { namespace experimental { namespace filesystem {} } }

namespace fs
{
    using namespace std::experimental::filesystem;
}

struct TestData
{
    std::string name;
    fs::path input_path;
    fs::path output_path;

    friend std::ostream& operator<< (std::ostream &out, const TestData &data)
    {
        return out << data.input_path.string();
    }

    static std::unique_ptr<TestData> create(const std::string fullname, const fs::path path)
    {
        if (fullname.size() < 3) return {};
        
        const std::string ext(fullname.end()-3, fullname.end());

        if (ext != ".in") return {};

        const std::string name(fullname.begin(), fullname.end()-3);

        const std::string path_string = path.string();
        const std::string path_prefix(path_string.begin(), path_string.end()-3);
        const fs::path output_path = path_prefix + ".out";

        return std::make_unique<TestData>(TestData{name, path, output_path});
    }
};

std::list<TestData> listTests(fs::path root = TASK_PATH, std::string prefix = "")
{
    std::list<TestData> ret;

    for(const auto &entry : fs::directory_iterator(root))
    {
        const std::string filename = entry.path().filename().string();

        const fs::file_status status = entry.status();

        if (fs::is_directory(status))
        {
            auto subtests = listTests(entry.path(), prefix + filename + "__");
            ret.splice(ret.end(), subtests);
        }

        if (fs::is_regular_file(status))
        {
            auto data = TestData::create(prefix + filename, entry.path());

            if (data)
                ret.push_back(*data);
        }
    }

    return ret;
}

class Test : public testing::TestWithParam<TestData> 
{
};

std::string getTestParamName(const testing::TestParamInfo<TestData>& info)
{
    return info.param.name;
}

void test();

TEST_P(Test, Run) 
{
    std::ifstream in(GetParam().input_path);
    std::stringstream out;

    auto cin_rd = std::cin.rdbuf(in.rdbuf());
    auto cout_rd = std::cout.rdbuf(out.rdbuf());

    EXPECT_NO_THROW(test());

    std::cin.rdbuf(cin_rd);
    std::cout.rdbuf(cout_rd);

    ASSERT_FALSE(HasFailure()) << "Execution failed.";

    std::ifstream expected_output(GetParam().output_path);

    {
        auto outputed = std::istream_iterator<std::string>(out);
        auto expected = std::istream_iterator<std::string>(expected_output);

        const auto end = std::istream_iterator<std::string>();

        while(outputed != end && expected != end)
        {
            EXPECT_EQ(*expected, *outputed);
            ++outputed;
            ++expected;
        }

        EXPECT_TRUE(end == outputed) << "Read more data than expected.";
        EXPECT_TRUE(end == expected) << "Read less data than expected.";
    }
}

INSTANTIATE_TEST_CASE_P(Test, Test, ::testing::ValuesIn(listTests()), getTestParamName);