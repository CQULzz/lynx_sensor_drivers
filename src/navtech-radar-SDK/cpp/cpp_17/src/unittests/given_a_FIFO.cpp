#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "FIFO.h"

using namespace std;
using namespace Navtech::Utility;


class GivenAFIFO : public ::testing::Test {
protected:
    GivenAFIFO() = default;
};


TEST_F(GivenAFIFO, DefaultConstructionIsEmpty)
{
    FIFO<int> buffer { };
    
    ASSERT_TRUE(buffer.empty());
}


TEST_F(GivenAFIFO, TryPopFromEmptyReturnsNullopt)
{
    FIFO<int> buffer { };

    auto result = buffer.try_pop();

    ASSERT_FALSE(result.has_value());
}


TEST_F(GivenAFIFO, ClearEmptiesFIFO)
{
    FIFO<int> buffer { };

    for (int i { 0 }; i < 10; ++i) buffer.push(i);

    ASSERT_EQ(buffer.size(), 10);
    ASSERT_FALSE(buffer.empty());

    buffer.clear();

    ASSERT_EQ(buffer.size(), 0);
    ASSERT_TRUE(buffer.empty());
}



TEST_F(GivenAFIFO, PushingAnLvalueCopies)
{
    FIFO<string> buffer { };
    string str { "Hello world" };

    buffer.push(str);

    ASSERT_EQ(buffer.size(), 1);
    ASSERT_FALSE(str.empty());
}


TEST_F(GivenAFIFO, PushingAnRvalueMoves)
{
    FIFO<string> buffer { };
    string str { "Hello world" };

    buffer.push(move(str));

    ASSERT_EQ(buffer.size(), 1);
    ASSERT_TRUE(str.empty());
}


TEST_F(GivenAFIFO, PoppingFromEmptyFIFOThrows)
{
    FIFO<int> buffer { };

    ASSERT_THROW(buffer.pop(), std::runtime_error);
}


TEST_F(GivenAFIFO, PoppingElementReducesFIFOSize)
{
    FIFO<int> buffer { };

    for (int i { 0 }; i < 10; ++i) buffer.push(i);

    ASSERT_EQ(buffer.size(), 10);

    buffer.pop();

    ASSERT_EQ(buffer.size(), 9);
    
}


TEST_F(GivenAFIFO, FirstInIsFirstOut)
{
    FIFO<int> buffer { };

    int first_in  { 1 };
    int second_in { 2 };
    
    buffer.push(first_in);
    buffer.push(second_in);
    
    auto first_out = buffer.pop();
    auto second_out = buffer.pop();

    ASSERT_EQ(first_out, first_in);
    ASSERT_EQ(second_out, second_in);
}
