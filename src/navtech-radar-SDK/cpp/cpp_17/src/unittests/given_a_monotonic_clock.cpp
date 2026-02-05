#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <sstream>

#include "Time_utils.h"

using namespace Navtech;
using namespace Navtech::Time;
using namespace Navtech::Time::Monotonic;
using namespace std;


class GivenAMonotonicClock : public ::testing::Test {
protected:
    GivenAMonotonicClock() = default;

    Duration d_pos { 200'000'000 };
    Duration d_neg { -200'000'000 };
    Duration d_max { numeric_limits<int64_t>::max() };
    Duration d_min { numeric_limits<int64_t>::min() };

    Observation obs_pos { d_pos };
    Observation obs_max { d_max };

    double dbl_max { numeric_limits<double>::max() };
    double dbl_min { numeric_limits<double>::min() };
    double dbl_pos { 2.0f };
    double dbl_neg { -2.0f };

    Thread_safe::Duration ts_d_pos { 200'000'000 };
    Thread_safe::Duration ts_d_neg { -200'000'000 };
    Thread_safe::Duration ts_d_max { numeric_limits<int64_t>::max() };
    Thread_safe::Duration ts_d_min { numeric_limits<int64_t>::min() };

    Thread_safe::Monotonic::Observation ts_obs_pos { d_pos };
    Thread_safe::Monotonic::Observation ts_obs_max { d_max };

};


// -----------------------------------------------------------------------
// Environment support tests
//
//TEST_F(GivenAMonotonicClock, TheMonotonicClockSupportsNanosecondTiming)
//{
//    std::chrono::nanoseconds t0 { std::chrono::high_resolution_clock::now().time_since_epoch() };
//    std::chrono::nanoseconds t1 { std::chrono::high_resolution_clock::now().time_since_epoch() };
//
//    ASSERT_GT(t1.count(), t0.count());
//    ASSERT_LE(t1.count() - t0.count(), 1000);
//}


//TEST_F(GivenAMonotonicClock, TheSteadyClockSupportsNanosecondTiming)
//{
//    std::chrono::nanoseconds t0 { std::chrono::steady_clock::now().time_since_epoch() };
//    std::chrono::nanoseconds t1 { std::chrono::steady_clock::now().time_since_epoch() };

//    ASSERT_GT(t1.count(), t0.count());
//    ASSERT_LE(t1.count() - t0.count(), 1000);
//}


// -----------------------------------------------------------------------
// Duration Construction and conversion tests
//
TEST_F(GivenAMonotonicClock, ADefaultDurationCanBeConstructed)
{
    Duration d { };

    ASSERT_EQ(d.ticks(), 0);
}


TEST_F(GivenAMonotonicClock, ADurationCanBeInitializedWithMicroSecondTicks)
{
    Duration d { 1'000'000 };

    ASSERT_EQ(d.ticks(), 1'000'000);

    ASSERT_FLOAT_EQ(d.in_sec(), 1.0f);
}



TEST_F(GivenAMonotonicClock, ADurationCanBeConstructedFromATimespec)
{
    std::timespec t { };
    t.tv_sec  = 7;
    t.tv_nsec = 500'000'000;

    Duration d { t };
 
    ASSERT_EQ(d.ticks(), 7'500'000);
}


TEST_F(GivenAMonotonicClock, DurationsCanBeRepresentedByPeriod)
{
    Duration d { 5'000'000 };

    ASSERT_EQ(d.ticks(),   5'000'000);
    // ASSERT_EQ(d.to_nsec(), 5'000'000);
    ASSERT_EQ(d.in_usec(), 5'000'000u);
    ASSERT_FLOAT_EQ(d.in_msec(), 5000.0f);
    ASSERT_FLOAT_EQ(d.in_sec(),  5.0f);
}


TEST_F(GivenAMonotonicClock, ADurationCanBeConstructedFromUserDefinedLiterals)
{
    // auto d1 { 1'000'000'000_nsec };
    auto d2 { 1'000'000_usec };
    auto d3 { 1'000_msec };
    auto d4 { 1_sec };

    // ASSERT_FLOAT_EQ(d1.in_sec(), 1);
    ASSERT_FLOAT_EQ(d2.in_sec(), 1.0f);
    ASSERT_FLOAT_EQ(d3.in_sec(), 1.0f);
    ASSERT_FLOAT_EQ(d4.in_sec(), 1.0f);
}



TEST_F(GivenAMonotonicClock, ADurationCanBeRounded)
{
    auto d1 { 4600_usec };
    auto d2 { 4'496'500_usec };

    ASSERT_EQ(d1.ticks(), 4'600);

    auto d3 = to_nearest_millisecond(d1);
    ASSERT_FLOAT_EQ(d3.in_msec(), 5.0f);
    ASSERT_FLOAT_EQ(d3.in_sec(), 0.005f);

    auto d4 = to_nearest_second(d1);
    ASSERT_EQ(d4.in_msec(), 0);
    ASSERT_EQ(d4.in_sec(), 0);

    auto d5 = to_nearest_millisecond(d2);
    auto d6 = to_nearest_second(d2);

    ASSERT_FLOAT_EQ(d5.in_msec(), 4497.0f);
    ASSERT_FLOAT_EQ(d5.in_sec(), 4.497f);

    ASSERT_FLOAT_EQ(d6.in_msec(), 4000.0f);
    ASSERT_FLOAT_EQ(d6.in_sec(), 4.0f);
    
}


TEST_F(GivenAMonotonicClock, SubMillisecondDurationsAreStreamedAsWholeMicroseconds)
{
    ostringstream os { };

    auto d1 { 0.375_msec };

    os << d1;

    ASSERT_EQ(os.str(), "375μs");
}


TEST_F(GivenAMonotonicClock, MillisecondDurationsAreStreamedAsFractionalMSec)
{
    ostringstream os { };

    auto d1 { 750.375_msec };

    os << d1;

    ASSERT_EQ(os.str(), "750.375ms");
}


TEST_F(GivenAMonotonicClock, DurationsGreaterThanOneSecondAreStreamedAsFractionalSeconds)
{
    ostringstream os { };

    auto d1 { 1750.375_msec };

    os << d1;

    ASSERT_EQ(os.str(), "1.750375s");
}


TEST_F(GivenAMonotonicClock, DurationsAreStreamedAsSeconds)
{
    ostringstream os { };

    auto d1 { 750.375_msec };

    os << d1;

    ASSERT_EQ(os.str(), "750.375ms");
}


TEST_F(GivenAMonotonicClock, DurationsCanBeConvertedFromScalarTypes)
{
    auto d1 = to_usec_duration(3500);
    auto d2 = to_msec_duration(3.5);
    auto d3 = to_sec_duration(0.0035);

    ASSERT_EQ(d1.ticks(), 3'500);
    ASSERT_EQ(d2.ticks(), 3'500);
    ASSERT_EQ(d3.ticks(), 3'500);

    auto d4 = to_msec_duration(3000);
    
    ASSERT_FLOAT_EQ(d4.in_sec(), 3.0);
}


TEST_F(GivenAMonotonicClock, DurationsCanBeConvertedToStdChronoTypes)
{
    auto d1 = 4500_msec;
    chrono::seconds      chrono_sec  = d1.to_chrono<chrono::seconds>();
    chrono::milliseconds chrono_msec = d1.to_chrono<chrono::milliseconds>();
    chrono::microseconds chrono_usec = d1.to_chrono<chrono::microseconds>();

    ASSERT_EQ(chrono_sec.count(), 4);
    ASSERT_EQ(chrono_msec.count(), 4500);
    ASSERT_EQ(chrono_usec.count(), 4'500'000);
}


TEST_F(GivenAMonotonicClock, DurationsCanBeAssigned)
{
    Duration d1 { 450_msec };
    Duration d2 { 50_msec };

    d1 = d2;

    ASSERT_EQ(d1, 50_msec);
    ASSERT_EQ(d2, 50_msec);
}


TEST_F(GivenAMonotonicClock, ForeverIsMax)
{
    ASSERT_EQ(Duration::forever().ticks(), numeric_limits<Tick_type>::max());
}


// -----------------------------------------------------------------------
// Threadsafe Duration Construction and conversion tests
//

TEST_F(GivenAMonotonicClock, ADefaultThreadsafeDurationCanBeConstructed)
{
    Thread_safe::Duration d { };

    ASSERT_EQ(d.ticks(), 0);
}


TEST_F(GivenAMonotonicClock, AThreadsafeDurationCanBeInitializedWithNanosecondTicks)
{
    Thread_safe::Duration d { 1'000'000 };

    ASSERT_FLOAT_EQ(d.in_sec(), 1);
}


TEST_F(GivenAMonotonicClock, ThreadsafeDurationsCanBeRepresentedByPeriod)
{
    Thread_safe::Duration d { 5'000 };

    ASSERT_EQ(d.ticks(), 5'000);
    ASSERT_EQ(d.in_usec(), 5'000);
    ASSERT_FLOAT_EQ(d.in_msec(), 5.0f);
    ASSERT_FLOAT_EQ(d.in_sec(), 0.005f);
}


TEST_F(GivenAMonotonicClock, AThreadsafeDurationCanBeConstructedFromUserDefinedLiterals)
{
    Thread_safe::Duration d1 { 1'000'000_usec };
    Thread_safe::Duration d2 { 1'000_msec };
    Thread_safe::Duration d3 { 1_sec };

    ASSERT_FLOAT_EQ(d1.in_sec(), 1.0f);
    ASSERT_FLOAT_EQ(d2.in_sec(), 1.0f);
    ASSERT_FLOAT_EQ(d3.in_sec(), 1.0f);
}


TEST_F(GivenAMonotonicClock, AThreadsafeDurationCanBeRounded)
{
    Thread_safe::Duration d1 { 4600_usec };
    Thread_safe::Duration d2 { 4'496'500_usec };

    Thread_safe::Duration d3 = to_nearest_millisecond(d1);
    Thread_safe::Duration d4 = to_nearest_second(d1);

    Thread_safe::Duration d5 = to_nearest_millisecond(d2);
    Thread_safe::Duration d6 = to_nearest_second(d2);

    ASSERT_FLOAT_EQ(d3.in_msec(), 5.0f);
    ASSERT_FLOAT_EQ(d3.in_sec(), 0.005f);

    ASSERT_FLOAT_EQ(d4.in_msec(), 0.0f);
    ASSERT_FLOAT_EQ(d4.in_sec(), 0.0f);

    ASSERT_FLOAT_EQ(d5.in_msec(), 4497.0f);
    ASSERT_FLOAT_EQ(d5.in_sec(), 4.497f);

    ASSERT_FLOAT_EQ(d6.in_msec(), 4000.0f);
    ASSERT_FLOAT_EQ(d6.in_sec(), 4.0f);
    
}


TEST_F(GivenAMonotonicClock, ThreadsafeDurationsCanBeAssigned)
{
    Thread_safe::Duration d1 { 450_msec };
    Thread_safe::Duration d2 { 50_msec };

    d1 = d2;

    ASSERT_EQ(d1, 50_msec);
    ASSERT_EQ(d2, 50_msec);
}


TEST_F(GivenAMonotonicClock, ThreadsafeDurationsCanBeMoved)
{
    Thread_safe::Duration d1 { 450_msec };
    Thread_safe::Duration d2 { 50_msec };

    d1 = move(d2);

    ASSERT_EQ(d1, 50_msec);
    ASSERT_EQ(d2, 50_msec);
}


TEST_F(GivenAMonotonicClock, ThreadsafeDurationsAreStreamedAsSeconds)
{
    ostringstream os { };

    Thread_safe::Duration d1 { 750.375_msec };

    os << d1;

    ASSERT_EQ(os.str(), "750.375ms");
}


TEST_F(GivenAMonotonicClock, ThreadsafeDurationsCanBeConvertedFromScalarTypes)
{
    Thread_safe::Duration d1 = to_usec_duration(3500);
    Thread_safe::Duration d2 = to_msec_duration(3.5);
    Thread_safe::Duration d3 = to_sec_duration(0.0035);

    ASSERT_EQ(d1.ticks(), 3'500);
    ASSERT_EQ(d2.ticks(), 3'500);
    ASSERT_EQ(d3.ticks(), 3'500);

    Thread_safe::Duration d4 = to_msec_duration(3000);
    
    ASSERT_FLOAT_EQ(d4.in_sec(), 3.0f);
}


TEST_F(GivenAMonotonicClock, ThreadsafeDurationsCanBeConvertedToStdChronoTypes)
{
    Thread_safe::Duration d1 = 4500_msec;
    chrono::seconds      chrono_sec  = d1.to_chrono<chrono::seconds>();
    chrono::milliseconds chrono_msec = d1.to_chrono<chrono::milliseconds>();
    chrono::microseconds chrono_usec = d1.to_chrono<chrono::microseconds>();

    ASSERT_EQ(chrono_sec.count(), 4);
    ASSERT_EQ(chrono_msec.count(), 4500);
    ASSERT_EQ(chrono_usec.count(), 4'500'000);
}


TEST_F(GivenAMonotonicClock, ThreadsafeDurationsCanBeConvertedToNonThreadSafeDurations)
{
    Duration d1 { };
    Thread_safe::Duration d2 = 4500_msec;

    auto fn = [&d1](const Duration& tsd) { d1 = tsd; };
    
    fn(d2.unlocked());
    
    ASSERT_EQ(d1, d2);
}


// -----------------------------------------------------------------------
// Monotonic observation construction and conversion tests
//
TEST_F(GivenAMonotonicClock, AnObservationCanBeDefaultConstructed)
{
    Observation t0 { };

    ASSERT_EQ(t0.since_epoch().ticks(), 0);
}


TEST_F(GivenAMonotonicClock, SubsequentObservationsAlwaysIncrease)
{
    auto t0 = now();
    auto t1 = now();
    auto t2 = now();
    auto t3 = now();

    ASSERT_TRUE(t1 >= t0);
    ASSERT_TRUE(t2 >= t1);
    ASSERT_TRUE(t3 >= t2);
}


TEST_F(GivenAMonotonicClock, SleepAbsolute)
{
    auto t0 = now();
    
    sleep_until(now() + 50_msec);
    
    auto t1 = now();

    auto delta = t1 - t0;

    ASSERT_TRUE(delta >= 50_msec);
}


TEST_F(GivenAMonotonicClock, ObservationsCanBeCopied)
{
    auto t0 = Clock::now();
    Observation t1 { t0 };

    ASSERT_EQ(t0, t1);
}


TEST_F(GivenAMonotonicClock, ObservationsCanBeAssigned)
{
    auto t0 = now();
    auto t1 = now() + 5_sec;

    t1 = t0;

    ASSERT_EQ(t0, t1);
}


TEST_F(GivenAMonotonicClock, ANullTimeObservationHasNoValue)
{
    auto t = null_time;

    ASSERT_EQ(t.since_epoch().ticks(), 0);
}


TEST_F(GivenAMonotonicClock, AnObservationCanConvertToNTP)
{
    Duration duration_since_epoch { 1629812650837542 };  // 2021-08-24 13:44:10.837542

    Observation t0 { duration_since_epoch };

    auto result = t0.to_timespec();

    ASSERT_EQ(result.tv_sec, 1629812650);
    ASSERT_EQ(result.tv_nsec, 837542000);
}


TEST_F(GivenAMonotonicClock, AMonotonicObservationCanBeConvertedToRealTime)
{
    Observation t_mono { now() };
    auto t_real = t_mono.to_real_time();
}


// -----------------------------------------------------------------------
// thread_safe::Monotonic Observation Construction and conversion tests
//
TEST_F(GivenAMonotonicClock, AThreadsafeObservationCanBeDefaultConstructed)
{
    Thread_safe::Monotonic::Observation t0 { };

    ASSERT_EQ(t0.since_epoch().ticks(), 0);
}


TEST_F(GivenAMonotonicClock, AThreadsafeObservationCanConvertToNTP)
{
    Duration duration_since_epoch { 1629812650837542 };  // 2021-08-24 13:44:10.837542112

    Thread_safe::Monotonic::Observation t0 { duration_since_epoch };

    auto result = t0.to_timespec();

    ASSERT_EQ(result.tv_sec, 1629812650);
    ASSERT_EQ(result.tv_nsec, 837542000);
}


TEST_F(GivenAMonotonicClock, SubsequentThreadsafeObservationsAlwaysIncrease)
{
    Thread_safe::Monotonic::Observation t0 = now();
    Thread_safe::Monotonic::Observation t1 = now();
    Thread_safe::Monotonic::Observation t2 = now();
    Thread_safe::Monotonic::Observation t3 = now();

    ASSERT_TRUE(t1 >= t0);
    ASSERT_TRUE(t2 >= t1);
    ASSERT_TRUE(t3 >= t2);
}


TEST_F(GivenAMonotonicClock, SleepAbsoluteWithThreadsafeObservation)
{
    Thread_safe::Monotonic::Observation t0 = now() + 50_msec;
    
    sleep_until(t0);
    
    Thread_safe::Monotonic::Observation t1 = now();

    auto delta = t1 - t0;

    cout << delta << endl;

    ASSERT_GE(delta.to_nearest_millisecond(), 0_msec);
}


TEST_F(GivenAMonotonicClock, ThreadsafeObservationsCanBeCopied)
{
    Thread_safe::Monotonic::Observation t0 = Clock::now();
    Thread_safe::Monotonic::Observation t1 { t0 };

    ASSERT_EQ(t0, t1);
}


TEST_F(GivenAMonotonicClock, ThreadsafeObservationsCanBeAssigned)
{
    Thread_safe::Monotonic::Observation t0 = now();
    Thread_safe::Monotonic::Observation t1 = now() + 5_sec;

    t1 = t0;

    ASSERT_EQ(t0, t1);
}


TEST_F(GivenAMonotonicClock, AThreadsafeObservationCanBeInitializedWithNulltime)
{
    Thread_safe::Monotonic::Observation t = null_time;

    ASSERT_EQ(t.since_epoch().ticks(), 0);
}


TEST_F(GivenAMonotonicClock, ThreadsafeAndNonThreadsafeObservationCanBeCombined)
{
    Thread_safe::Monotonic::Observation t0 = now();

    auto t1 = t0 + 50_msec;

    auto delta = t1 - t0;

    ASSERT_TRUE(t1 > t0);
    ASSERT_TRUE(delta >= 0_sec);

    t0 = now();
    t1 = now();
    t0 += 5_sec;

    ASSERT_TRUE(t0 > t1);
}


// -----------------------------------------------------------------------
// Basic operator overload tests
//
TEST_F(GivenAMonotonicClock, DurationsCanBeAdded)
{
    ASSERT_EQ(d_min + d_min, 0_msec);
    ASSERT_EQ(d_neg + d_pos, 0_msec);
    ASSERT_EQ(0_msec + 0_msec, 0_msec);

    ASSERT_EQ(d_pos + d_pos, 400_sec);
    
    ASSERT_EQ((d_max + d_min).ticks(), -1);

    ASSERT_EQ(d_min + 0_msec, d_min);
    ASSERT_EQ(d_max + 0_msec, d_max);
    ASSERT_TRUE((d_max + d_neg) < d_max);
    ASSERT_TRUE(d_max + d_pos != d_max);
    ASSERT_TRUE(d_pos + d_max != d_max);
}


TEST_F(GivenAMonotonicClock, DurationsSupportAssignmentOperatorOverloads)
{
    auto d1 { 450_msec };
    auto d2 { 100_msec };

    d1 += 50_msec;
    d2 -= 50_msec;

    ASSERT_EQ(d1, 500_msec);
    ASSERT_EQ(d2, 50_msec);

    d1 *= 2.0;
    d2 /= 10.0;

    ASSERT_EQ(d1, 1000_msec);
    ASSERT_EQ(d2, 5_msec);
}


TEST_F(GivenAMonotonicClock, DurationsCanBeSubtracted)
{
    ASSERT_EQ(d_min - d_min, 0_msec);
    ASSERT_EQ(d_neg - d_pos, Duration { -400'000'000 });
    ASSERT_EQ(0_msec - 0_msec, 0_msec);
    ASSERT_EQ(d_pos - d_pos, 0_msec);
    ASSERT_EQ(d_max - d_min, Duration { -1 });
    ASSERT_EQ(d_min - 0_msec, d_min);
    ASSERT_EQ(d_max - 0_msec, d_max);
    
    // Overflow beahviour is undefined!  This is not a 
    // sensible test!
    //
    // ASSERT_TRUE((d_max - d_neg) < d_max);
    ASSERT_TRUE((d_max - d_pos) != d_max);
    ASSERT_TRUE((d_pos - d_max) > d_min );
}


TEST_F(GivenAMonotonicClock, DurationsCanBeaAddedToObservations)
{
    ASSERT_EQ(0_msec + null_time, null_time);
    ASSERT_EQ(d_pos + obs_pos, Observation { Duration { 400'000'000 } });
    ASSERT_EQ(0_msec + obs_max, obs_max);
    ASSERT_NE(d_pos + obs_max, obs_max);
    ASSERT_TRUE((d_neg + obs_max) < obs_max);
    ASSERT_EQ(d_neg + null_time, null_time);
    ASSERT_EQ(d_neg + obs_pos, null_time);
    ASSERT_EQ(d_max + obs_max, null_time);  // Overflow will lead to negative result
}


TEST_F(GivenAMonotonicClock, ObservationsCanBeaAddedToDurations)
{
    ASSERT_EQ(null_time + 0_msec, null_time);
    ASSERT_EQ(obs_pos + d_pos, Observation { Duration { 400'000'000 } });
    ASSERT_EQ(obs_max + 0_msec, obs_max);
    ASSERT_NE(obs_max + d_pos, obs_max);
    ASSERT_TRUE((obs_max + d_neg) < obs_max);
    ASSERT_EQ(null_time + d_neg, null_time);
    ASSERT_EQ(obs_pos + d_neg, null_time);
    ASSERT_EQ(obs_max + d_max, null_time);  // Overflow will lead to negative result
}


TEST_F(GivenAMonotonicClock, DurationsCanBeSubtractedFromObservations)
{
    ASSERT_EQ(obs_pos - 0_msec, obs_pos);
    ASSERT_EQ(obs_pos - d_pos, null_time);
    ASSERT_EQ(obs_max - 0_msec, obs_max);
    ASSERT_NE(obs_max - d_pos, obs_max);
    ASSERT_EQ(obs_max - d_neg, null_time);  // Overflow to nagative value
    ASSERT_EQ(null_time - d_max, null_time);
    ASSERT_EQ(obs_pos - d_neg, Observation { Duration { 400'000'000 } });
    ASSERT_EQ(obs_max - d_max, null_time); 
}


TEST_F(GivenAMonotonicClock, ObservationsCanBeSubtracted)
{
    ASSERT_EQ(null_time - null_time, 0_msec);
    ASSERT_EQ(obs_pos - obs_pos, 0_msec);
    ASSERT_EQ(null_time - obs_pos, Duration { -200'000'000 });
    ASSERT_NE(obs_pos - obs_max, d_min);
    ASSERT_EQ(obs_max - null_time, d_max);
}


TEST_F(GivenAMonotonicClock, DurationsCanBeScaled)
{
    ASSERT_EQ(d_pos * dbl_min, 0_usec);
    ASSERT_EQ(d_pos * dbl_neg, Duration { -400'000'000 });
    ASSERT_EQ(d_pos * 0.0f, Duration { });
    ASSERT_EQ(d_pos * dbl_pos, Duration { 400'000'000 });
    ASSERT_TRUE(d_pos * dbl_max != d_max);
    ASSERT_EQ(d_max * dbl_min, 0_usec);
    ASSERT_TRUE(d_max * dbl_neg != d_max);
    ASSERT_EQ(d_max * 0.0f, Duration { });
    ASSERT_TRUE(d_max * dbl_pos != d_max); 
    ASSERT_EQ(d_neg * dbl_min, 0_usec);
    ASSERT_EQ(d_neg * dbl_pos, Duration { -400'000'000 });
    ASSERT_EQ(d_neg * dbl_neg, 400_sec);
    ASSERT_EQ(d_min * dbl_min, 0_usec);
    ASSERT_EQ(d_min * dbl_max, d_min);

    // -Ofast or -ffast-math optimisation disables NaN
    //
    // ASSERT_THROW((d_pos * NAN), std::invalid_argument);

    ASSERT_EQ(dbl_min * d_pos, 0_usec);
    ASSERT_EQ(dbl_neg * d_pos , Duration { -400'000'000 });
    ASSERT_EQ(0.0f * d_pos, Duration { });
    ASSERT_EQ(dbl_pos * d_pos, Duration { 400'000'000 });
    ASSERT_TRUE(dbl_max * d_pos != d_max);
    ASSERT_EQ(dbl_min * d_max, 0_usec);
    ASSERT_TRUE(dbl_neg * d_max != d_max);
    ASSERT_EQ(0.0f * d_max, Duration { });
    ASSERT_TRUE(dbl_pos * d_max != d_max); 
    ASSERT_EQ(dbl_min * d_neg, 0_usec);
    ASSERT_EQ(dbl_pos * d_neg, Duration { -400'000'000 });
    ASSERT_EQ(dbl_neg * d_neg, 400_sec);
    ASSERT_EQ(dbl_min * d_min, 0_usec);
    ASSERT_EQ(dbl_max * d_min, d_min);
}


TEST_F(GivenAMonotonicClock, DurationsCanBeDividedByDurations)
{
    ASSERT_THROW(0_msec / 0_msec, std::overflow_error);

    ASSERT_EQ(d_pos / d_min, 0);
    ASSERT_EQ(d_pos / d_neg, -1);
    ASSERT_EQ(d_pos / d_pos, 1);
    ASSERT_EQ(d_pos / 5_sec, 40);
    ASSERT_EQ(d_pos / d_max, 0);
    ASSERT_THROW(d_pos / 0_sec, std::overflow_error);

    ASSERT_EQ(d_neg / d_min, 0);
    ASSERT_EQ(d_neg / d_neg, 1);
    ASSERT_EQ(d_neg / d_pos, -1);
    ASSERT_EQ(d_neg / 5_sec, -40);
    ASSERT_EQ(d_neg / d_max, 0);
    ASSERT_THROW(d_neg / 0_sec, std::overflow_error);

    ASSERT_EQ(d_min / d_min, 1);
    ASSERT_EQ(d_max / d_max, 1);
    ASSERT_EQ(d_max / d_min, 0);
    ASSERT_EQ(d_min / d_max, -1);
}


TEST_F(GivenAMonotonicClock, DurationsCanBeDividedByDoubles)
{
    ASSERT_THROW(0_msec / 0.0f, std::overflow_error);
    ASSERT_THROW(d_pos / 0.0f, std::overflow_error);
    ASSERT_THROW(d_neg / 0.0f, std::overflow_error);

    ASSERT_EQ(d_pos / dbl_min, d_min);
    ASSERT_EQ(d_pos / dbl_neg, Duration { -100'000'000 });
    ASSERT_EQ(d_pos / dbl_pos, 100_sec);
    ASSERT_EQ(d_pos / dbl_max, 0_msec);

    ASSERT_EQ(0_sec / dbl_pos, 0_sec);
    ASSERT_EQ(0_sec / dbl_neg, 0_sec);
    ASSERT_EQ(0_sec / dbl_max, 0_sec);
    ASSERT_EQ(0_sec / dbl_min, 0_sec);

    ASSERT_EQ(d_min / dbl_min, d_min);
    ASSERT_NE(d_min / dbl_neg, d_min);
    ASSERT_NE(d_min / dbl_pos, d_min);
    ASSERT_EQ(d_min / dbl_max, 0_msec);

    ASSERT_EQ(d_max / dbl_min, d_min);
    ASSERT_NE(d_max / dbl_neg, d_max);
    ASSERT_NE(d_max / dbl_pos, d_max);
    ASSERT_EQ(d_max / dbl_max, 0_usec);
}


TEST_F(GivenAMonotonicClock, DurationsCanBeCompared)
{
    auto d1 = 350_msec;
    auto d2 = 250'000_usec;
    auto d3 = 0.35_sec;

    ASSERT_TRUE(d1 == d3);
    ASSERT_TRUE(d2 != d3);
    ASSERT_TRUE(d1 > d2);
    ASSERT_TRUE(d2 < d1);
    
    ASSERT_FALSE(d1 > d3);
    ASSERT_FALSE(d3 < d1);

    ASSERT_TRUE(d1 >= d3);
    ASSERT_TRUE(d3 <= d1);
}


TEST_F(GivenAMonotonicClock, ObservationsCanBeCompared)
{
    auto obs1 = now();
    auto obs2 = obs1 - 500_msec;
    auto obs3 = obs2 + 500_msec;

    ASSERT_TRUE(obs1 == obs3);
    ASSERT_TRUE(obs2 != obs3);
    ASSERT_TRUE(obs1 > obs2);
    ASSERT_TRUE(obs2 < obs1);
    
    ASSERT_FALSE(obs1 > obs3);
    ASSERT_FALSE(obs3 < obs1);

    ASSERT_TRUE(obs1 >= obs3);
    ASSERT_TRUE(obs3 <= obs1);
}


TEST_F(GivenAMonotonicClock, AbsoluteDifferenceBetweenObservationAndDuration)
{
    ASSERT_EQ(abs_diff(null_time, 0_msec), 0_msec);
    ASSERT_EQ(abs_diff(null_time, d_neg), d_pos);
    ASSERT_EQ(abs_diff(null_time, d_pos), d_pos);

    ASSERT_EQ(abs_diff(obs_pos, d_pos), 0_msec);
    ASSERT_EQ(abs_diff(obs_pos, d_neg), 400_sec);

    ASSERT_EQ(abs_diff(obs_max, d_max).ticks(), 0);
    ASSERT_EQ(abs_diff(obs_max, d_min).ticks(), 1);
}


TEST_F(GivenAMonotonicClock, AbsoluteDifferenceBetweenDurations)
{
    auto d1 = 250_msec;
    auto d2 = 100_msec;

    ASSERT_EQ(abs_diff(0_sec, 0_msec), 0_usec);

    ASSERT_EQ(abs_diff(d1, d2), 150_msec);
    ASSERT_EQ(abs_diff(d2, d1), 150_msec);
    ASSERT_EQ(abs_diff(0_msec, d1), 250_msec);
    ASSERT_EQ(abs_diff(d1, 0_msec), 250_msec);

    ASSERT_EQ(abs_diff(d_pos, d_pos), 0_msec);
    ASSERT_EQ(abs_diff(d_neg, d_neg), 0_msec);

    ASSERT_EQ(abs_diff(d_pos, d_neg), 400_sec);
    ASSERT_EQ(abs_diff(d_neg, d_pos), 400_sec);

    ASSERT_EQ(abs_diff(0_msec, d_max), d_max);
    ASSERT_EQ(abs_diff(0_msec, d_min), d_min);
}


TEST_F(GivenAMonotonicClock, AbsoluteDifferenceBetweenObservations)
{
    ASSERT_EQ(abs_diff(null_time, null_time), 0_msec);

    ASSERT_EQ(abs_diff(obs_pos, null_time), 200_sec);
    ASSERT_EQ(abs_diff(null_time, obs_pos), 200_sec);
}


// -----------------------------------------------------------------------
// Constexpr operator overload tests
//
TEST_F(GivenAMonotonicClock, ConstexprDurationsCanBeAdded)
{
    constexpr auto d1 { 450_msec };
    constexpr auto d2 { 50_msec };
    constexpr auto d3 = d1 + d2;

    ASSERT_EQ(d3, 500_msec);
}


// -----------------------------------------------------------------------
// Threadsafe operator overload tests
//
TEST_F(GivenAMonotonicClock, ThreadsafeDurationsCanBeAdded)
{
    ASSERT_EQ(ts_d_min + ts_d_min, 0_msec);
    ASSERT_EQ(ts_d_neg + ts_d_pos, 0_msec);
    ASSERT_EQ(0_msec + 0_msec, 0_msec);
    ASSERT_EQ(ts_d_pos + ts_d_pos, 400_sec);
    ASSERT_EQ(ts_d_max + ts_d_min, Duration { -1 });
    ASSERT_EQ(ts_d_min + 0_msec, ts_d_min);
    ASSERT_EQ(ts_d_max + 0_msec, ts_d_max);
    ASSERT_TRUE((ts_d_max + ts_d_neg) < ts_d_max);
    ASSERT_TRUE(ts_d_max + ts_d_pos != ts_d_max);
    ASSERT_TRUE(ts_d_pos + ts_d_max != ts_d_max);
}


TEST_F(GivenAMonotonicClock, ThreadsafeDurationsSupportAssignmentOperatorOverloads)
{
    Thread_safe::Duration d1 { 450_msec };
    Thread_safe::Duration d2 { 100_msec };

    d1 += 50_msec;
    d2 -= 50_msec;

    ASSERT_EQ(d1, 500_msec);
    ASSERT_EQ(d2, 50_msec);

    d1 *= 2.0;
    d2 /= 10.0;

    ASSERT_EQ(d1, 1000_msec);
    ASSERT_EQ(d2, 5_msec);
}


TEST_F(GivenAMonotonicClock, ThreadsafeDurationsCanBeSubtracted)
{
    ASSERT_EQ(ts_d_min - ts_d_min, 0_msec);
    ASSERT_EQ(ts_d_neg - ts_d_pos, Duration { -400'000'000 });
    ASSERT_EQ(0_msec - 0_msec, 0_msec);
    ASSERT_EQ(ts_d_pos - ts_d_pos, 0_msec);
    ASSERT_EQ(ts_d_max - ts_d_min, Duration { -1 });
    ASSERT_EQ(ts_d_min - 0_msec, ts_d_min);
    ASSERT_EQ(ts_d_max - 0_msec, ts_d_max);
    ASSERT_TRUE((ts_d_max - ts_d_neg) < ts_d_max);
    ASSERT_TRUE((ts_d_max - ts_d_pos) != ts_d_max);
    ASSERT_TRUE((ts_d_pos - ts_d_max) > ts_d_min );
}


TEST_F(GivenAMonotonicClock, ThreadsafeDurationsCanBeaAddedToObservations)
{
    ASSERT_EQ(0_msec + null_time, null_time);
    ASSERT_EQ(ts_d_pos + ts_obs_pos, Observation { Duration { 400'000'000 } });
    ASSERT_EQ(0_msec + ts_obs_max, ts_obs_max);
    ASSERT_NE(ts_d_pos + ts_obs_max, ts_obs_max);
    ASSERT_TRUE((ts_d_neg + ts_obs_max) < ts_obs_max);
    ASSERT_EQ(ts_d_neg + null_time, null_time);
    ASSERT_EQ(ts_d_neg + ts_obs_pos, null_time);
    ASSERT_EQ(ts_d_max + ts_obs_max, null_time);  // Overflow will lead to negative result
}


TEST_F(GivenAMonotonicClock, ThreadsafeObservationsCanBeaAddedToDurations)
{
    ASSERT_EQ(null_time + 0_msec, null_time);
    ASSERT_EQ(ts_obs_pos + ts_d_pos, Observation { Duration { 400'000'000 } });
    ASSERT_EQ(ts_obs_max + 0_msec, ts_obs_max);
    ASSERT_NE(ts_obs_max + ts_d_pos, ts_obs_max);
    ASSERT_TRUE((ts_obs_max + ts_d_neg) < ts_obs_max);
    ASSERT_EQ(null_time + ts_d_neg, null_time);
    ASSERT_EQ(ts_obs_pos + ts_d_neg, null_time);
    ASSERT_EQ(ts_obs_max + ts_d_max, null_time);  // Overflow will lead to negative result
}


TEST_F(GivenAMonotonicClock, ThreadsafeDurationsCanBeSubtractedFromObservations)
{
    ASSERT_EQ(ts_obs_pos - 0_msec, ts_obs_pos);
    ASSERT_EQ(ts_obs_pos - ts_d_pos, null_time);
    ASSERT_EQ(ts_obs_max - 0_msec, ts_obs_max);
    ASSERT_NE(ts_obs_max - ts_d_pos, ts_obs_max);
    ASSERT_EQ(ts_obs_max - ts_d_neg, null_time);  // Overflow to nagative value
    ASSERT_EQ(null_time - ts_d_max, null_time);
    ASSERT_EQ(ts_obs_pos - ts_d_neg, Observation { Duration { 400'000'000 } });
    ASSERT_EQ(ts_obs_max - ts_d_max, null_time); 
}


TEST_F(GivenAMonotonicClock, ThreadsafeObservationsCanBeSubtracted)
{
    ASSERT_EQ(null_time - null_time, 0_msec);
    ASSERT_EQ(ts_obs_pos - ts_obs_pos, 0_msec);
    ASSERT_EQ(null_time - ts_obs_pos, Duration { -200'000'000 });
    ASSERT_NE(ts_obs_pos - ts_obs_max, ts_d_min);
    ASSERT_EQ(ts_obs_max - null_time, ts_d_max);
}


TEST_F(GivenAMonotonicClock, ThreadsafeDurationsCanBeScaled)
{
    ASSERT_EQ(ts_d_pos * dbl_min, 0_usec);
    ASSERT_EQ(ts_d_pos * dbl_neg, Duration { -400'000'000 });
    ASSERT_EQ(ts_d_pos * 0.0f, Duration { });
    ASSERT_EQ(ts_d_pos * dbl_pos, Duration { 400'000'000 });
    ASSERT_TRUE(ts_d_pos * dbl_max != ts_d_max);
    ASSERT_EQ(ts_d_max * dbl_min, 0_usec);
    ASSERT_TRUE(ts_d_max * dbl_neg != ts_d_max);
    ASSERT_EQ(ts_d_max * 0.0f, Duration { });
    ASSERT_TRUE(ts_d_max * dbl_pos != ts_d_max); 
    ASSERT_EQ(ts_d_neg * dbl_min, 0_usec);
    ASSERT_EQ(ts_d_neg * dbl_pos, Duration { -400'000'000 });
    ASSERT_EQ(ts_d_neg * dbl_neg, 400_sec);
    ASSERT_EQ(ts_d_min * dbl_min, 0_usec);
    ASSERT_EQ(ts_d_min * dbl_max, ts_d_min);

    // -Ofast or -ffast-math optimisation disables NaN
    //
    // ASSERT_THROW((d_pos * NAN), std::invalid_argument);

    ASSERT_EQ(dbl_min * ts_d_pos, 0_usec);
    ASSERT_EQ(dbl_neg * ts_d_pos , Duration { -400'000'000 });
    ASSERT_EQ(0.0f * ts_d_pos, Duration { });
    ASSERT_EQ(dbl_pos * ts_d_pos, Duration { 400'000'000 });
    ASSERT_TRUE(dbl_max * ts_d_pos != d_max);
    ASSERT_EQ(dbl_min * ts_d_max, 0_usec);
    ASSERT_TRUE(dbl_neg * ts_d_max != d_max);
    ASSERT_EQ(0.0f * ts_d_max, Duration { });
    ASSERT_TRUE(dbl_pos * ts_d_max != d_max); 
    ASSERT_EQ(dbl_min * ts_d_neg, 0_usec);
    ASSERT_EQ(dbl_pos * ts_d_neg, Duration { -400'000'000 });
    ASSERT_EQ(dbl_neg * ts_d_neg, 400_sec);
    ASSERT_EQ(dbl_min * ts_d_min, 0_usec);
    ASSERT_EQ(dbl_max * ts_d_min, d_min);
}


TEST_F(GivenAMonotonicClock, ThreadsafeDurationsCanBeDividedByDurations)
{
    ASSERT_THROW(0_msec / 0_msec, std::overflow_error);

    ASSERT_EQ(ts_d_pos / ts_d_min, 0);
    ASSERT_EQ(ts_d_pos / ts_d_neg, -1);
    ASSERT_EQ(ts_d_pos / ts_d_pos, 1);
    ASSERT_EQ(ts_d_pos / 5_sec, 40);
    ASSERT_EQ(ts_d_pos / ts_d_max, 0);
    ASSERT_THROW(ts_d_pos / 0_sec, std::overflow_error);

    ASSERT_EQ(ts_d_neg / ts_d_min, 0);
    ASSERT_EQ(ts_d_neg / ts_d_neg, 1);
    ASSERT_EQ(ts_d_neg / ts_d_pos, -1);
    ASSERT_EQ(ts_d_neg / 5_sec, -40);
    ASSERT_EQ(ts_d_neg / ts_d_max, 0);
    ASSERT_THROW(ts_d_neg / 0_sec, std::overflow_error);

    ASSERT_EQ(ts_d_min / ts_d_min, 1);
    ASSERT_EQ(ts_d_max / ts_d_max, 1);
    ASSERT_EQ(ts_d_max / ts_d_min, 0);
    ASSERT_EQ(ts_d_min / ts_d_max, -1);
}


TEST_F(GivenAMonotonicClock, ThreadsafeDurationsCanBeDividedByDoubles)
{
    ASSERT_THROW(0_msec / 0.0f, std::overflow_error);
    ASSERT_THROW(ts_d_pos / 0.0f, std::overflow_error);
    ASSERT_THROW(ts_d_neg / 0.0f, std::overflow_error);

    ASSERT_EQ(ts_d_pos / dbl_min, d_min);
    ASSERT_EQ(ts_d_pos / dbl_neg, Duration { -100'000'000 });
    ASSERT_EQ(ts_d_pos / dbl_pos, 100_sec);
    ASSERT_EQ(ts_d_pos / dbl_max, 0_msec);

    ASSERT_EQ(0_sec / dbl_pos, 0_sec);
    ASSERT_EQ(0_sec / dbl_neg, 0_sec);
    ASSERT_EQ(0_sec / dbl_max, 0_sec);
    ASSERT_EQ(0_sec / dbl_min, 0_sec);

    ASSERT_EQ(ts_d_min / dbl_min, d_min);
    ASSERT_NE(ts_d_min / dbl_neg, d_min);
    ASSERT_NE(ts_d_min / dbl_pos, d_min);
    ASSERT_EQ(ts_d_min / dbl_max, 0_msec);

    ASSERT_EQ(ts_d_max / dbl_min, d_min);
    ASSERT_NE(ts_d_max / dbl_neg, d_max);
    ASSERT_NE(ts_d_max / dbl_pos, d_max);
    ASSERT_EQ(ts_d_max / dbl_max, 0_usec);
}


TEST_F(GivenAMonotonicClock, ThreadsafeDurationsCanBeCompared)
{
    Thread_safe::Duration d1 { 350_msec };
    Thread_safe::Duration d2 { 250'000_usec };
    Thread_safe::Duration d3 { 0.35_sec };

    ASSERT_TRUE(d1 == d3);
    ASSERT_TRUE(d2 != d3);
    ASSERT_TRUE(d1 > d2);
    ASSERT_TRUE(d2 < d1);
    
    ASSERT_FALSE(d1 > d3);
    ASSERT_FALSE(d3 < d1);

    ASSERT_TRUE(d1 >= d3);
    ASSERT_TRUE(d3 <= d1);
}


TEST_F(GivenAMonotonicClock, ThreadsafeObservationsCanBeCompared)
{
    Thread_safe::Monotonic::Observation obs1 { now() };
    Thread_safe::Monotonic::Observation obs2 { obs1 - 500_msec };
    Thread_safe::Monotonic::Observation obs3 { obs2 + 500_msec };

    ASSERT_TRUE(obs1 == obs3);
    ASSERT_TRUE(obs2 != obs3);
    ASSERT_TRUE(obs1 > obs2);
    ASSERT_TRUE(obs2 < obs1);
    
    ASSERT_FALSE(obs1 > obs3);
    ASSERT_FALSE(obs3 < obs1);

    ASSERT_TRUE(obs1 >= obs3);
    ASSERT_TRUE(obs3 <= obs1);
}


TEST_F(GivenAMonotonicClock, AbsoluteDifferenceBetweenThreadsafeObservationAndDuration)
{
    ASSERT_EQ(abs_diff(null_time, 0_msec), 0_msec);
    ASSERT_EQ(abs_diff(null_time, ts_d_neg), ts_d_pos);
    ASSERT_EQ(abs_diff(null_time, ts_d_pos), ts_d_pos);

    ASSERT_EQ(abs_diff(ts_obs_pos, ts_d_pos), 0_msec);
    ASSERT_EQ(abs_diff(ts_obs_pos, ts_d_neg), 400_sec);

    ASSERT_EQ(abs_diff(ts_obs_max, ts_d_max).ticks(), 0);
    ASSERT_EQ(abs_diff(ts_obs_max, ts_d_min).ticks(), 1);
}


TEST_F(GivenAMonotonicClock, AbsoluteDifferenceBetweenThreadsafeDurations)
{
    Thread_safe::Duration d1 { 250_msec };
    Thread_safe::Duration d2 { 100_msec };

    ASSERT_EQ(abs_diff(0_sec, 0_msec), 0_usec);

    ASSERT_EQ(abs_diff(d1, d2), 150_msec);
    ASSERT_EQ(abs_diff(d2, d1), 150_msec);
    ASSERT_EQ(abs_diff(0_msec, d1), 250_msec);
    ASSERT_EQ(abs_diff(d1, 0_msec), 250_msec);

    ASSERT_EQ(abs_diff(ts_d_pos, ts_d_pos), 0_msec);
    ASSERT_EQ(abs_diff(ts_d_neg, ts_d_neg), 0_msec);

    ASSERT_EQ(abs_diff(ts_d_pos, ts_d_neg), 400_sec);
    ASSERT_EQ(abs_diff(ts_d_neg, ts_d_pos), 400_sec);

    ASSERT_EQ(abs_diff(0_msec, ts_d_max), ts_d_max);
    ASSERT_EQ(abs_diff(0_msec, ts_d_min), ts_d_min);
}


TEST_F(GivenAMonotonicClock, AbsoluteDifferenceBetweenThreadsafeObservations)
{
    ASSERT_EQ(abs_diff(null_time, null_time), 0_msec);

    ASSERT_EQ(abs_diff(ts_obs_pos, null_time), 200_sec);
    ASSERT_EQ(abs_diff(null_time, ts_obs_pos), 200_sec);
}
#if 0
#endif // if 0