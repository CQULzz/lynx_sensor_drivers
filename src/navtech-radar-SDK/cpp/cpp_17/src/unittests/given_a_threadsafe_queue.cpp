#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>

#include "Threadsafe_queue.h"

using namespace Navtech;
using namespace Navtech::Utility;
using namespace std;


class GivenAThreadSafeQueue : public ::testing::Test {
protected:
    GivenAThreadSafeQueue() = default;
};


TEST_F(GivenAThreadSafeQueue, DefaultConstructedQueue)
{
    Threadsafe_queue<int> q { };

    ASSERT_EQ(q.size(), 0);
    ASSERT_TRUE(q.empty());
}


TEST_F(GivenAThreadSafeQueue, ElementsAreFIFOed)
{
    Threadsafe_queue<int> q { };

    int first_in  { 101 };
    int second_in { 102 };
    int third_in  { 103 };

    q.push(first_in);
    q.push(second_in);
    q.push(third_in);

    auto first_out  = q.pop().value();
    auto second_out = q.pop().value();
    auto third_out  = q.pop().value();

    ASSERT_EQ(first_in, first_out);
    ASSERT_EQ(second_in, second_out);
    ASSERT_EQ(third_in, third_out);
}


TEST_F(GivenAThreadSafeQueue, ItemsCanBeInsertedByCopy)
{
    Threadsafe_queue<string> q { };

    string test_str{ "TEST" };
    q.push(test_str);
    ASSERT_EQ(test_str, "TEST");

    auto result = q.pop().value();
    ASSERT_EQ(result, "TEST");
}


TEST_F(GivenAThreadSafeQueue, ItemsCanBeInsertedByMove)
{
    Threadsafe_queue<string> q { };

    string test_str{ "TEST" };
    q.push(move(test_str));
    ASSERT_TRUE(test_str.empty());

    auto result = q.pop().value();
    ASSERT_EQ(result, "TEST");
}


TEST_F(GivenAThreadSafeQueue, AddingAnItemIncreasesQueueSize)
{
    Threadsafe_queue<int> q { };

    q.push(100);

    ASSERT_FALSE(q.empty());
    ASSERT_EQ(q.size(), 1);
}


TEST_F(GivenAThreadSafeQueue, RemovingAQueueWithASingleItemEmptiesQueue)
{
    Threadsafe_queue<int> q { };

    q.push(100);

    ASSERT_FALSE(q.empty());
    ASSERT_EQ(q.size(), 1);

    auto result = q.pop().value();

    ASSERT_EQ(result, 100);
    ASSERT_TRUE(q.empty());
    ASSERT_EQ(q.size(), 0);
}


TEST_F(GivenAThreadSafeQueue, PoppingFromAnEmptyQueueReturnsEmptyOptional)
{
    Threadsafe_queue<int> q { };

    auto result = q.try_pop();
    ASSERT_FALSE(result.has_value());
}


TEST_F(GivenAThreadSafeQueue, AddingToAMaxedOutQueueDoesNothing)
{
    Threadsafe_queue<int> q { 2 };

    q.push(100);
    q.push(101);

    ASSERT_FALSE(q.empty());
    ASSERT_EQ(q.size(), 2);

    auto result = q.push(102);

    ASSERT_FALSE(result);
    ASSERT_EQ(q.size(), 2);
}


TEST_F(GivenAThreadSafeQueue, PoppingFromAMaxedOutQueueAllowsAPush)
{
    Threadsafe_queue<int> q { 2 };

    q.push(100);
    q.push(101);

    ASSERT_FALSE(q.empty());
    ASSERT_EQ(q.size(), 2);

    q.push(102);

    ASSERT_EQ(q.size(), 2);

    auto out = q.pop().value();
    ASSERT_EQ(q.size(), 1);
    ASSERT_EQ(out, 100);

    q.push(102);
    auto first_out  = q.pop().value();
    auto second_out = q.pop().value();

    ASSERT_EQ(first_out, 101);
    ASSERT_EQ(second_out, 102);
}