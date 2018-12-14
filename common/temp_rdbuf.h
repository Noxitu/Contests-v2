#include <ios>

class temporary_rdbuf
{
private:
    std::ios &m_ios;
    std::streambuf *m_old_rdbuf;

public:
    temporary_rdbuf(std::ios &ios, std::streambuf *rdbuf) : m_ios(ios)
    {
        m_old_rdbuf = m_ios.rdbuf(rdbuf);
    }

    ~temporary_rdbuf()
    {
        m_ios.rdbuf(m_old_rdbuf);
    }
};