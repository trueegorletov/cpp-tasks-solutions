#include <algorithm>
#include <array>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

#include "base85ed.h"

namespace
{

constexpr std::array<char, 85> ALPHABET =
{
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
    'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
    'y', 'z', '!', '#', '$', '%', '&', '(', ')', '*',
    '+', '-', ';', '<', '=', '>', '?', '@', '^', '_',
    '`', '{', '|', '}', '~'
};

std::array<int8_t, 256> make_decode_table()
{
    std::array<int8_t, 256> table {};
    table.fill(-1);

    for (size_t i = 0; i < ALPHABET.size(); ++i)
    {
        table[static_cast<uint8_t>(ALPHABET[i])] = static_cast<int8_t>(i);
    }

    return table;
}

uint32_t read_hunk(uint8_t const *bytes, size_t length)
{
    uint32_t value = 0;

    for (size_t i = 0; i < 4; ++i)
    {
        value <<= 8U;
        if (i < length)
        {
            value |= bytes[i];
        }
    }

    return value;
}

void append_uint32_bytes(std::vector<uint8_t> &out, uint32_t value)
{
    out.push_back(static_cast<uint8_t>((value >> 24U) & 0xffU));
    out.push_back(static_cast<uint8_t>((value >> 16U) & 0xffU));
    out.push_back(static_cast<uint8_t>((value >> 8U) & 0xffU));
    out.push_back(static_cast<uint8_t>(value & 0xffU));
}

} // namespace

std::vector<uint8_t> base85::encode(std::vector<uint8_t> const &bytes)
{
    if (bytes.empty())
    {
        return {};
    }

    std::vector<uint8_t> encoded;
    encoded.reserve(((bytes.size() + 3U) / 4U) * 5U);

    for (size_t offset = 0; offset < bytes.size(); offset += 4U)
    {
        const size_t hunk_size = std::min<size_t>(4U, bytes.size() - offset);
        uint32_t value = read_hunk(bytes.data() + offset, hunk_size);

        std::array<uint8_t, 5> hunk {};
        for (size_t i = hunk.size(); i > 0U; --i)
        {
            hunk[i - 1U] = static_cast<uint8_t>(ALPHABET[value % 85U]);
            value /= 85U;
        }

        const size_t encoded_size = hunk_size == 4U ? 5U : hunk_size + 1U;
        encoded.insert(encoded.end(), hunk.begin(), hunk.begin() + encoded_size);
    }

    return encoded;
}

std::vector<uint8_t> base85::decode(std::vector<uint8_t> const &b85str)
{
    if (b85str.empty())
    {
        return {};
    }

    static const std::array<int8_t, 256> decode_table = make_decode_table();

    std::vector<uint8_t> decoded;
    decoded.reserve((b85str.size() / 5U) * 4U + 4U);

    for (size_t offset = 0; offset < b85str.size(); offset += 5U)
    {
        const size_t hunk_size = std::min<size_t>(5U, b85str.size() - offset);
        uint64_t value = 0;

        for (size_t i = 0; i < 5U; ++i)
        {
            int8_t digit = 84;
            if (i < hunk_size)
            {
                digit = decode_table[b85str[offset + i]];
                if (digit < 0)
                {
                    throw std::invalid_argument("bad base85 character");
                }
            }

            value = value * 85U + static_cast<uint8_t>(digit);
        }

        if (value > std::numeric_limits<uint32_t>::max())
        {
            throw std::overflow_error("base85 overflow");
        }

        append_uint32_bytes(decoded, static_cast<uint32_t>(value));

        if (hunk_size < 5U)
        {
            decoded.resize(decoded.size() - (5U - hunk_size));
        }
    }

    return decoded;
}
