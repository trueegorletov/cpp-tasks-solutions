#include <gtest/gtest.h>

#include <numeric>

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

#include "base85ed.h"

const std::vector<std::pair<const char *, const char * >> short_cases =
{
    { "",     ""     },
    { "F#",   "1"    },
    { "F){",  "12"   },
    { "F)}j", "123"  },
    { "F)}kW","1234" }
};

static std::vector<uint8_t> cstr2v(const char *s)
{
    return std::vector<uint8_t>(
               s,
               s + std::string(s).size()
           );
}

// Тесты encode
TEST(Base85ShortsEncode, TrivialShortEncodes)
{
    for (const auto &p : short_cases)
    {
        EXPECT_EQ(base85::encode(cstr2v(p.second)), cstr2v(p.first));
    }
}

// Тесты decode
TEST(Base85ShortsDecode, TrivialShortDecodes)
{
    for (const auto &p : short_cases)
    {
        EXPECT_EQ(base85::decode(cstr2v(p.first)), cstr2v(p.second));
    }
}

TEST(Base85KnownCases, EncodesPythonCompatibleVectors)
{
    EXPECT_EQ(base85::encode({0, 0, 0, 0}), cstr2v("00000"));
    EXPECT_EQ(base85::encode({255, 255, 255, 255}), cstr2v("|NsC0"));
    EXPECT_EQ(base85::encode(cstr2v("hello world")), cstr2v("Xk~0{Zy<MXa%^M"));

    const std::vector<uint8_t> bytes =
    {
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
        11, 12, 13, 14, 15, 16, 17, 18, 19
    };

    EXPECT_EQ(base85::encode(bytes), cstr2v("0RjUA1qKHQ2?`4g4Gs?w5fT#"));
}

TEST(Base85KnownCases, DecodesPythonCompatibleVectors)
{
    EXPECT_EQ(base85::decode(cstr2v("00000")), (std::vector<uint8_t> {0, 0, 0, 0}));
    EXPECT_EQ(base85::decode(cstr2v("|NsC0")),
              (std::vector<uint8_t> {255, 255, 255, 255}));
    EXPECT_EQ(base85::decode(cstr2v("Xk~0{Zy<MXa%^M")), cstr2v("hello world"));

    const std::vector<uint8_t> bytes =
    {
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
        11, 12, 13, 14, 15, 16, 17, 18, 19
    };

    EXPECT_EQ(base85::decode(cstr2v("0RjUA1qKHQ2?`4g4Gs?w5fT#")), bytes);
}

TEST(Base85RoundTrip, WorksForManyBinaryLengths)
{
    std::vector<uint8_t> bytes(256);
    std::iota(bytes.begin(), bytes.end(), 0);

    for (size_t size = 0; size <= bytes.size(); ++size)
    {
        const auto prefix_end = bytes.begin() +
                                static_cast<std::vector<uint8_t>::difference_type>(size);
        std::vector<uint8_t> prefix(bytes.begin(), prefix_end);
        EXPECT_EQ(base85::decode(base85::encode(prefix)), prefix);
    }
}

TEST(Base85DecodeErrors, RejectsBadCharactersAndOverflow)
{
    EXPECT_THROW(base85::decode(cstr2v("abc ")), std::invalid_argument);
    EXPECT_THROW(base85::decode(std::vector<uint8_t> {'\n'}), std::invalid_argument);

    EXPECT_THROW(base85::decode(cstr2v("~~~~~")), std::overflow_error);
    EXPECT_THROW(base85::decode(cstr2v("|NsC1")), std::overflow_error);
}
