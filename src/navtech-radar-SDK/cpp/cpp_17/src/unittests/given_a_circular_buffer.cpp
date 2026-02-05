
#include <vector>
#include <string>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Circular_buffer.h"


using Navtech::Utility::Circular_buffer;
using namespace std;


class GivenACircularBuffer : public ::testing::Test {
protected:
    GivenACircularBuffer() = default;

    static constexpr size_t test_buffer_capacity { 8 };
};



TEST_F(GivenACircularBuffer, DefaultConstructedBufferIsEmpty)
{
    Circular_buffer<int, test_buffer_capacity> buffer { };

    ASSERT_TRUE(buffer.empty());
}


TEST_F(GivenACircularBuffer, AnEmptyBufferHasSizeZero)
{
    Circular_buffer<int, 8> buffer { };

    ASSERT_EQ(buffer.size(), 0);
    ASSERT_EQ(buffer.capacity(), 8);
}


TEST_F(GivenACircularBuffer, CapacityIsNumElementsBeforeOverwrite)
{
    Circular_buffer<int, 8> buffer { };

    buffer.push(100);
    ASSERT_EQ(buffer.size(), 1);
    ASSERT_EQ(buffer.capacity(), 7);
}


TEST_F(GivenACircularBuffer, AFullBufferHasZeroCapacity)
{
    Circular_buffer<int, 8> buffer { };
    
    for (int i  { 0 }; i < 8; ++i) {
        buffer.push(i);
    }

    ASSERT_EQ(buffer.size(), 8);
    ASSERT_EQ(buffer.capacity(), 0);
}


TEST_F(GivenACircularBuffer, PushingAnLvalueCopies)
{
    Circular_buffer<string, 8> buffer { };
    string str { "Hello world" };
    
    buffer.push(str);
    
    ASSERT_FALSE(str.empty());
    ASSERT_EQ(buffer.size(), 1);
}


TEST_F(GivenACircularBuffer, PushingAnRvalueMoves)
{
    Circular_buffer<string, 8> buffer { };
    string str { "Hello world" };
    
    buffer.push(move(str));
    
    ASSERT_TRUE(str.empty());
    ASSERT_EQ(buffer.size(), 1);
}


TEST_F(GivenACircularBuffer, PushingAnLvalueContainerCopiesAllElements)
{
    Circular_buffer<int, 8> buffer { };
    vector<int> v { 1, 2, 3, 4, 5 };
    buffer.push(v);

    ASSERT_EQ(buffer.size(), 5);
    ASSERT_FALSE(v.empty());
    
    for (int i : v) {
        ASSERT_NE(i, 0);
    }
}


TEST_F(GivenACircularBuffer, PushingAnRvalueContainerMovesAllElements)
{
    Circular_buffer<int, 8> buffer { };
    std::vector<int> v { 1, 2, 3, 4, 5 };
    buffer.push(move(v));

    ASSERT_TRUE(v.empty());
    ASSERT_EQ(buffer.size(), 5);
}


TEST_F(GivenACircularBuffer, PushingMoreElementsThanCapacityOverwritesOld)
{
    Circular_buffer<int, 8> buffer { };

    ASSERT_EQ(buffer.size(), 0);

    std::vector<int> v { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    buffer.push(v);

    ASSERT_EQ(buffer.size(), 8);
    ASSERT_EQ(buffer.capacity(), 0);

    auto result = buffer.pop();
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value(), 2);
}


TEST_F(GivenACircularBuffer, PushingTwoIteratorsCopiesElementsInRange)
{
    Circular_buffer<int, 8> buffer { };
    std::vector<int> v { 1, 2, 3, 4, 5 };
    buffer.push(begin(v), end(v));

    ASSERT_FALSE(buffer.empty());
    ASSERT_EQ(buffer.size(), 5);
    ASSERT_EQ(v.size(), 5);
}


TEST_F(GivenACircularBuffer, PushingSameIteratorTwiceCopiesNothing)
{
    Circular_buffer<int, 8> buffer { };
    std::vector<int> v { 1, 2, 3, 4, 5 };
    buffer.push(begin(v), begin(v));

    ASSERT_TRUE(buffer.empty());
    ASSERT_EQ(buffer.size(), 0);
    ASSERT_EQ(v.size(), 5);
}


TEST_F(GivenACircularBuffer, PushingASpanCopiesNElements)
{
    Circular_buffer<int, 8> buffer { };
    std::vector<int> v { 1, 2, 3, 4, 5 };
    buffer.push(begin(v), v.size());

    ASSERT_FALSE(buffer.empty());
    ASSERT_EQ(buffer.size(), 5);
    ASSERT_EQ(v.size(), 5);
}


TEST_F(GivenACircularBuffer, PushingASpanOfZeroCopiesNothing)
{
    Circular_buffer<int, 8> buffer { };
    std::vector<int> v { 1, 2, 3, 4, 5 };
    buffer.push(begin(v), 0);

    ASSERT_TRUE(buffer.empty());
    ASSERT_EQ(buffer.size(), 0);
    ASSERT_EQ(v.size(), 5);
}


TEST_F(GivenACircularBuffer, PoppingFromEmptyBufferReturnsNullopt)
{
    Circular_buffer<int, 8> buffer { };
    auto result = buffer.pop();

    ASSERT_FALSE(result.has_value());
}


TEST_F(GivenACircularBuffer, PoppingNReturnsNElements)
{
    Circular_buffer<int, 8> buffer { };

    for (int i { 0 }; i < 8; ++i) buffer.push(i);

    auto result = buffer.pop_n(8);

    ASSERT_TRUE(buffer.empty());
    ASSERT_FALSE(result.empty());
    ASSERT_EQ(buffer.size(), 0);
    ASSERT_EQ(result.size(), 8);
}


TEST_F(GivenACircularBuffer, PoppingGreaterThanBufferSizeReturnsSizeElements)
{
    Circular_buffer<int, 8> buffer { };

    for (int i { 0 }; i < 8; ++i) buffer.push(i);

    auto result = buffer.pop_n(16); // <= More than buffer size!

    ASSERT_TRUE(buffer.empty());
    ASSERT_FALSE(result.empty());
    ASSERT_EQ(buffer.size(), 0);
    ASSERT_EQ(result.size(), 8);
}


TEST_F(GivenACircularBuffer, PoppingZeroElementsReturnsEmptyVector)
{
    Circular_buffer<int, 8> buffer { };

    for (int i { 0 }; i < 8; ++i) buffer.push(i);

    auto result = buffer.pop_n(0);  // <= Return none

    ASSERT_FALSE(buffer.empty());
    ASSERT_TRUE(result.empty());
    ASSERT_EQ(buffer.size(), 8);
    ASSERT_EQ(result.size(), 0);
}


TEST_F(GivenACircularBuffer, PoppingNFromEmptyBufferReturnsEmptyVector)
{
    Circular_buffer<int, 8> buffer { };

    auto result = buffer.pop_n(8);

    ASSERT_TRUE(buffer.empty());
    ASSERT_TRUE(result.empty());
}


TEST_F(GivenACircularBuffer, PoppingSpanReturnsNElements)
{
    Circular_buffer<int, 8> buffer { };
    vector<int> result { };

    for (int i { 0 }; i < 8; ++i) buffer.push(i);

    buffer.pop_n_into(back_inserter(result), 5);

    ASSERT_EQ(result.size(), 5);
    ASSERT_EQ(buffer.size(), 3);
}


TEST_F(GivenACircularBuffer, PoppingSpanGreaterThanSizeReturnsOnlySizeElements)
{
    Circular_buffer<int, 8> buffer { };
    vector<int> result { };

    for (int i { 0 }; i < 8; ++i) buffer.push(i);

    buffer.pop_n_into(back_inserter(result), 100);

    ASSERT_EQ(result.size(), 8);
    ASSERT_EQ(buffer.size(), 0);
}


TEST_F(GivenACircularBuffer, FirstInIsFirstOut)
{
    Circular_buffer<int, 8> buffer { };

    int first_in  { 1 };
    int second_in { 2 };
    
    buffer.push(first_in);
    buffer.push(second_in);
    
    auto first_out = buffer.pop();

    ASSERT_TRUE(first_out.has_value());
    ASSERT_EQ(first_out.value(), first_in);
}