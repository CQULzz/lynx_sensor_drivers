#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "string_helpers.h"

using namespace Navtech::Utility;

class GivenAStringHelper : public ::testing::Test 
{
public:
    GivenAStringHelper() = default;
};


TEST_F(GivenAStringHelper, NumberOfBytesShouldConvertToCorrectString)
{
    constexpr std::uint64_t onepointfive_terabytes = 1'500'000'000'000;
    constexpr std::uint64_t onepointfive_gigabytes = 1'500'000'000;
    constexpr std::uint64_t onepointfive_megabytes = 1'500'000;
    constexpr std::uint64_t onepointfive_kilobytes = 1'500;
    constexpr std::uint64_t fivehundred_bytes = 500;

    ASSERT_EQ("1.50 TB", to_memory_string(onepointfive_terabytes));
    ASSERT_EQ("1.50 GB", to_memory_string(onepointfive_gigabytes));
    ASSERT_EQ("1.50 MB", to_memory_string(onepointfive_megabytes));
    ASSERT_EQ("1.50 kB", to_memory_string(onepointfive_kilobytes));
    ASSERT_EQ("500 bytes", to_memory_string(fivehundred_bytes));
}


TEST_F(GivenAStringHelper, CollectionsAndValuesShouldConvertToHexString)
{
    std::vector<std::uint8_t> dead_beef { 0xDE, 0xAD, 0xBE, 0xEF };
    std::uint64_t beef_dead { 0xBEEFDEAD };

    ASSERT_EQ(to_hex_string(dead_beef), "0xDEADBEEF");
    ASSERT_EQ(to_hex_string(beef_dead), "0xBEEFDEAD");
}


TEST_F(GivenAStringHelper, LTrimShouldTrimLeftSide)
{
    std::string padded { "   Test String is a Test String   " };

    ASSERT_EQ(ltrim(padded), "Test String is a Test String   ");
    ASSERT_EQ(ltrim( "   Test String is a Test String   "), "Test String is a Test String   ");
    ASSERT_EQ(rtrim("   "), "");
}


TEST_F(GivenAStringHelper, RTrimShouldTrimRightSide)
{
    std::string padded { "   Test String is a Test String   " };

    ASSERT_EQ(rtrim(padded), "   Test String is a Test String");
    ASSERT_EQ(rtrim("   Test String is a Test String   "), "   Test String is a Test String");
    ASSERT_EQ(rtrim("   "), "");
}


TEST_F(GivenAStringHelper, TrimShouldTrimBothSides)
{
    std::string padded { "   Test String is a Test String   " };

    ASSERT_EQ(trim(padded), "Test String is a Test String");
    ASSERT_EQ(trim("   Test String is a Test String   "), "Test String is a Test String");
    ASSERT_EQ(trim("   "), "");
}


TEST_F(GivenAStringHelper, SplitShouldSplitStringOnDelimiter)
{
    std::string comma_sentence { "Test,String,Is,A,Test,String" };
    std::string slash_sentence { "Test/String/Is/A/Test/String"};

    auto comma_separated = split(comma_sentence, ',');
    auto slash_separated = split(slash_sentence, '/');
    auto nothing = split("", '/');

    std::vector<std::string> expected  { "Test", "String", "Is", "A", "Test", "String" };

    ASSERT_EQ(comma_separated.size(), 6);
    ASSERT_EQ(comma_separated, expected);

    ASSERT_EQ(slash_separated.size(), 6);
    ASSERT_EQ(slash_separated, expected);

    ASSERT_EQ(nothing.size(), 0);
}


TEST_F(GivenAStringHelper, ReplaceShouldCorrectlyReplaceElements)
{
    std::string original { "Test String is a Test String" };

    replace(original, "String", "Replacement");
    ASSERT_EQ(original, "Test Replacement is a Test Replacement");

    replace(original, "GNU", "Not Unix");
    ASSERT_EQ(original, "Test Replacement is a Test Replacement");
}