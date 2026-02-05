#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>
#include <cstdint>

#include "csv_parser.h"

using namespace Navtech::Utility;
using namespace std;

class GivenACSVParser : public ::testing::Test 
{
public:
    GivenACSVParser() = default;
};


TEST_F(GivenACSVParser, ParseStrings)
{
    string input { "hello, world, this, is, some, text" };

    CSV_parser<string> parser { };

    auto results = parser.parse(input);

    ASSERT_EQ(results.size(), 6);
    ASSERT_EQ(results[0], "hello");
    ASSERT_EQ(results[5], "text");
}


TEST_F(GivenACSVParser, ParseIntegers)
{
    string input { "0, 1, 2, 3, 4, 5" };

    CSV_parser<int> parser { };

    auto results = parser.parse(input);

    ASSERT_EQ(results.size(), 6);
    ASSERT_EQ(results[0], 0);
    ASSERT_EQ(results[5], 5);
}


TEST_F(GivenACSVParser, ParseUint8)
{
    string input { "0, 1, 2, 3, 4, 5" };

    CSV_parser<uint8_t> parser { };

    auto results = parser.parse(input);

    ASSERT_EQ(results.size(), 6);
    ASSERT_EQ(results[0], 0);
    ASSERT_EQ(results[5], 5);
}


TEST_F(GivenACSVParser, ParseUint16)
{
    string input { "0, 100, 200, 300, 400, 500" };

    CSV_parser<uint16_t> parser { };

    auto results = parser.parse(input);

    ASSERT_EQ(results.size(), 6);
    ASSERT_EQ(results[0], 0);
    ASSERT_EQ(results[5], 500);
}


TEST_F(GivenACSVParser, ParseFloats)
{
    string input { "0.0, 1.1, 2.2, 3.3, 4.4, 5.5" };

    CSV_parser<float> parser { };

    auto results = parser.parse(input);

    ASSERT_EQ(results.size(), 6);
    ASSERT_FLOAT_EQ(results[0], 0.0f);
    ASSERT_FLOAT_EQ(results[5], 5.5f);
}



TEST_F(GivenACSVParser, ParseDoubles)
{
    string input { "0.0, 1.1, 2.2, 3.3, 4.4, 5.5" };

    CSV_parser<double> parser { };

    auto results = parser.parse(input);

    ASSERT_EQ(results.size(), 6);
    ASSERT_DOUBLE_EQ(results[0], 0.0);
    ASSERT_DOUBLE_EQ(results[5], 5.5);
}


TEST_F(GivenACSVParser, ParseEmpty)
{
    string input { };

    CSV_parser<int> parser { };

    auto results = parser.parse(input);

    ASSERT_EQ(results.size(), 0);
}


TEST_F(GivenACSVParser, ParseOne)
{
    string input { "77" };

    CSV_parser<int> parser { };

    auto results = parser.parse(input);

    ASSERT_EQ(results.size(), 1);
    ASSERT_EQ(results[0], 77);
}