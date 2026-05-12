#include <iostream>
#include <vector>
#include <cstdint>
#include <functional>
#include <string>

#include "base85ed.h"

std::vector<uint8_t> read_stdin_to_vector_iostream()
{
    constexpr std::streamsize BUF_SIZE = static_cast<std::streamsize>(64) * 1024;
    std::vector<uint8_t> out;
    out.reserve(1024);
    std::vector<char> buf(BUF_SIZE);

    // Ensure std::cin is in binary mode where applicable (no-op on POSIX).
    std::ios::sync_with_stdio(false);

    while (std::cin)
    {
        std::cin.read(buf.data(), BUF_SIZE);
        std::streamsize n = std::cin.gcount();
        if (n > 0)
        {
            out.insert(out.end(), reinterpret_cast<uint8_t*>(buf.data()),
                       reinterpret_cast<uint8_t*>(buf.data() + n));
        }
        if (n < BUF_SIZE)
        {
            // either EOF or error
            break;
        }
    }
    return out;
}

void write_vector_to_stdout(const std::vector<uint8_t>& data)
{
    // Ensure no tied flushing and faster IO (optional)
    std::ios::sync_with_stdio(false);

    if (!data.empty())
    {
        std::cout.write(reinterpret_cast<const char*>(data.data()),
                        static_cast<std::streamsize>(data.size()));
    }
    // flush to ensure data is written out
    std::cout.flush();
}


int main(int argc, const char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Use -e or -d argument\n";
        return 1;
    }
    else
    {
        std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> func = nullptr;
        std::string a = argv[1];
        if (a == "--encode" || a == "-e")
        {
            func = base85::encode;
        }
        else if (a == "--decode" || a == "-d")
        {
            func = base85::decode;
        }
        else
        {
            std::cerr << "Don't know how to deal with <" << a << ">, use -e or -d\n";
            return 1;
        }

        auto data = read_stdin_to_vector_iostream();
        auto result = func(data);
        write_vector_to_stdout(result);
    }

    return 0;
}
