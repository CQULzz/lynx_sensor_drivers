#include <thread>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Timer.h"

using namespace std;
using namespace Navtech;
using namespace Navtech::Utility;
using namespace std::chrono_literals;


// Class to expose internals of Timer
//
class Test_timer : public Timer {
    using Timer::Timer;
};



class GivenATimer : public ::testing::Test {
protected:
    GivenATimer() = default;
};


TEST_F(GivenATimer, StartThenStop)
{
    Test_timer timer { 10_msec, Timer::Mode::repeating };

    timer.start();
    timer.stop();
}


TEST_F(GivenATimer, RunAPeriodicTimeout)
{
    int i { };
    Test_timer timer { 100_msec, [&i]() { ++i; } };

    timer.start();

    this_thread::sleep_for(1s);

    timer.stop();

    ASSERT_GE(i, 9);
}


TEST_F(GivenATimer, RunAOneShotTimeout)
{
    int i { };
    Test_timer timer  { 100_msec, [&i]() { ++i; }, Timer::Mode::one_shot };

    timer.start();

    this_thread::sleep_for(1s);

    timer.stop();

    ASSERT_EQ(i, 1);
}