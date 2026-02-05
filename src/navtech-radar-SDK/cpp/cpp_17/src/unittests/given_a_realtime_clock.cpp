#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <sstream>
#include <iostream>
#include <filesystem>

#include "Time_utils.h"

using namespace Navtech;
using namespace Navtech::Time;
using namespace Navtech::Time::Real_time;
using namespace std;


class GivenARealtimeClock : public ::testing::Test {
protected:
    GivenARealtimeClock() = default;

    Duration d_pos { 200'000'000 };
    Duration d_neg { -200'000'000 };
    Duration d_max { std::numeric_limits<int64_t>::max() };
    Duration d_min { std::numeric_limits<int64_t>::min() };

    Observation obs_pos { d_pos };
    Observation obs_max { d_max };

    double dbl_max { std::numeric_limits<double>::max() };
    double dbl_min { std::numeric_limits<double>::min() };
    double dbl_pos { 2.0f };
    double dbl_neg { -2.0f };

    Thread_safe::Duration ts_d_pos { 200'000'000 };
    Thread_safe::Duration ts_d_neg { -200'000'000 };
    Thread_safe::Duration ts_d_max { std::numeric_limits<int64_t>::max() };
    Thread_safe::Duration ts_d_min { std::numeric_limits<int64_t>::min() };

    Thread_safe::Real_time::Observation ts_obs_pos { d_pos };
    Thread_safe::Real_time::Observation ts_obs_max { d_max };
};

// -----------------------------------------------------------------------
// NB:
// Duration unit tests are performed in 'given_a_monotonic_clock.cpp'
//
// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
// Real-time observation construction and conversion tests
//
TEST_F(GivenARealtimeClock, AnObservationCanBeDefaultConstructed)
{
    Observation t0 { };

    ASSERT_EQ(t0.since_epoch().ticks(), 0);
}


TEST_F(GivenARealtimeClock, ObservationsCanBeSubtracted)
{
    auto t0 = now();

    Monotonic::sleep_for(500_msec);

    auto t1        = now();
    auto delta     = t1 - t0;
    auto abs_delta = abs_diff(t0, t1);

    ASSERT_GE(delta.to_nearest_millisecond(), 500_msec);
    ASSERT_GE(abs_delta.to_nearest_millisecond(), 500_msec);
}


TEST_F(GivenARealtimeClock, ObservationsCanBeInitializedFromDurations)
{
    Duration duration_since_epoch { 1629812650837542 };  // 2021-08-24 13:44:10.837542
                                    
    Observation t0 { duration_since_epoch };

    ASSERT_EQ(t0.since_epoch().ticks(), duration_since_epoch.ticks());
    ASSERT_EQ(t0.to_string(), "2021-08-24T13:44:10.838Z");
}


TEST_F(GivenARealtimeClock, ObservationsAlwaysDisplayMillisecondsToThreeFigures)
{
    Duration duration_since_epoch { 1'629'812'650'830'000 };  // 2021-08-24 13:44:10.83
                                    
    Observation t0 { duration_since_epoch };

    ASSERT_EQ(t0.since_epoch().ticks(), duration_since_epoch.ticks());
    ASSERT_EQ(t0.to_string(), "2021-08-24T13:44:10.830Z");
}


TEST_F(GivenARealtimeClock, ObservationsCanBeSeparated)
{
    Observation t0 { 1629812650837542_usec }; // 2021-08-24 13:44:10.837542

    ASSERT_EQ(t0.year(), 2021);
    ASSERT_EQ(t0.month(), 8);
    ASSERT_EQ(t0.day(), 24);
    ASSERT_EQ(t0.hour(), 13);
    ASSERT_EQ(t0.minute(), 44);
    ASSERT_EQ(t0.second(), 10);
    ASSERT_EQ(t0.milliseconds(), 837);
    ASSERT_EQ(t0.microseconds(), 542);
}


TEST_F(GivenARealtimeClock, ObservationsCanHaveNoSubSecondPart)
{
    Observation t0 { 1629812650000000_usec }; // 2021-08-24 13:44:10

    ASSERT_EQ(t0.year(), 2021);
    ASSERT_EQ(t0.month(), 8);
    ASSERT_EQ(t0.day(), 24);
    ASSERT_EQ(t0.hour(), 13);
    ASSERT_EQ(t0.minute(), 44);
    ASSERT_EQ(t0.second(), 10);
    ASSERT_EQ(t0.milliseconds(), 0);
    ASSERT_EQ(t0.microseconds(), 0);
}


TEST_F(GivenARealtimeClock, OutputFormatsCanBeSpecified)
{
    Observation t0 { 1629812650837542_usec }; // 2021-08-24 13:44:10.837542

    ASSERT_EQ(t0.to_string(), "2021-08-24T13:44:10.838Z");      // Log format (default);
    ASSERT_EQ(t0.format_as("%F").to_string(), "2021-08-24");
    ASSERT_EQ(t0.format_as("%T").to_string(), "13:44:10");
    ASSERT_EQ(t0.format_as("%T.%ms").to_string(), "13:44:10.838");
    ASSERT_EQ(t0.format_as("%T.%us").to_string(), "13:44:10.837542");
    ASSERT_EQ(t0.format_as("%T.%ms GMT").to_string(), "13:44:10.838 GMT");
    ASSERT_EQ(t0.format_as("%Y%m%d_%H:%M:%S.%ms").to_string(), "20210824_13:44:10.838");
}


TEST_F(GivenARealtimeClock, ObservationsCanBeStreamed)
{
    Observation t0 { 1629812650837542_usec }; // 2021-08-24 13:44:10.837542

    ostringstream os { };

    os << t0;

    ASSERT_EQ(os.str(), "2021-08-24T13:44:10.838Z");

    ostringstream { }.swap(os); // Empty the stringstream

    os << t0.format_as("%F");

    ASSERT_EQ(os.str(), "2021-08-24");
}


TEST_F(GivenARealtimeClock, ObservationsCanBeCopied)
{
    auto t0 = Clock::now();
    Observation t1 { t0 };

    ASSERT_EQ(t0, t1);
}


TEST_F(GivenARealtimeClock, ObservationsCanBeAssigned)
{
    auto t0 = now();
    auto t1 = now() + 5_sec;

    t1 = t0;

    ASSERT_EQ(t0, t1);
}


TEST_F(GivenARealtimeClock, ANullTimeObservationHasNoValue)
{
    auto t = null_time;

    ASSERT_EQ(t.since_epoch().ticks(), 0);
}


TEST_F(GivenARealtimeClock, AnObservationCanBeConstructedFromNTPTime)
{
    timespec t { };
    t.tv_sec  = 1'629'812'650; // 2021-08-24 13:44:10.837542
    t.tv_nsec = 837'542'000;

    Observation t0 { t };

    ASSERT_EQ(t0.year(), 2021);
    ASSERT_EQ(t0.month(), 8);
    ASSERT_EQ(t0.day(), 24);
    ASSERT_EQ(t0.hour(), 13);
    ASSERT_EQ(t0.minute(), 44);
    ASSERT_EQ(t0.second(), 10);
    ASSERT_EQ(t0.milliseconds(), 837);
    ASSERT_EQ(t0.microseconds(), 542);
}


TEST_F(GivenARealtimeClock, AnObservationCanBeConvertedToAnNTPTime)
{
    Observation t0 { 1629812650837542_usec }; // 2021-08-24 13:44:10.837542

    auto result = t0.to_timespec();
    ASSERT_EQ(result.tv_sec, 1'629'812'650);
    ASSERT_EQ(result.tv_nsec, 837'542'000);
}


TEST_F(GivenARealtimeClock, ARealTimeObservationCanBeConvertedToAMonotonicObservation)
{
    Observation t0 { 1629812650837542_usec }; // 2021-08-24 13:44:10.837542

    Monotonic::Observation result = t0.to_monotonic();
    ASSERT_EQ(result.since_epoch().ticks(), 1629812650837542);
}


TEST_F(GivenARealtimeClock, AFileWithATimeBasedNameCanBeCreated)
{
    Observation t0 { 1629812650837542_usec }; // 2021-08-24 13:44:10.837542

    // File paths take different structures on Windows compared to Linux
    //
    #ifdef __linux__
        filesystem::path test_path { "/systemconfig/firmware/data"};
        
        auto file_name = test_path / t0.format_as("%Y%m%d%H%M%S").to_string();
        file_name += ".txt";

        ASSERT_EQ(file_name.string(), "/systemconfig/firmware/data/20210824134410.txt");
    #else
        filesystem::path test_path { "\\systemconfig\\firmware\\data"};
        
        auto file_name = test_path / t0.format_as("%Y%m%d%H%M%S").to_string();
        file_name += ".txt";

        ASSERT_EQ(file_name.string(), "\\systemconfig\\firmware\\data\\20210824134410.txt");
    #endif
}


// -----------------------------------------------------------------------
// thread_safe::Real_time observation construction tests
//
TEST_F(GivenARealtimeClock, AThreadsafeObservationCanBeDefaultConstructed)
{
    Thread_safe::Real_time::Observation t0 { };

    ASSERT_EQ(t0.since_epoch().ticks(), 0);
}


TEST_F(GivenARealtimeClock, ThreadsafeObservationsCanBeSubtracted)
{
    Thread_safe::Real_time::Observation t0 = now();

    Monotonic::sleep_for(500_msec);

    Thread_safe::Real_time::Observation t1 = now();

    auto delta     = t1 - t0;
    auto abs_delta = abs_diff(t0, t1);

    ASSERT_GE(delta.to_nearest_millisecond(), 500_msec);
    ASSERT_GE(abs_delta.to_nearest_millisecond(), 500_msec);
}


TEST_F(GivenARealtimeClock, ThreadsafeObservationsCanBeInitializedFromDurations)
{
    Duration duration_since_epoch { 1629812650837542 };  // 2021-08-24 13:44:10.837542
    Thread_safe::Real_time::Observation t0 { duration_since_epoch };

    ASSERT_EQ(t0.since_epoch().ticks(), duration_since_epoch.ticks());
    ASSERT_EQ(t0.to_string(), "2021-08-24T13:44:10.838Z");
}


TEST_F(GivenARealtimeClock, ThreadsafeObservationsCanBeSeparated)
{
    Thread_safe::Real_time::Observation t0 { 1629812650837542_usec }; // 2021-08-24 13:44:10.837542

    ASSERT_EQ(t0.year(), 2021);
    ASSERT_EQ(t0.month(), 8);
    ASSERT_EQ(t0.day(), 24);
    ASSERT_EQ(t0.hour(), 13);
    ASSERT_EQ(t0.minute(), 44);
    ASSERT_EQ(t0.second(), 10);
    ASSERT_EQ(t0.milliseconds(), 837);
    ASSERT_EQ(t0.microseconds(), 542);
}


TEST_F(GivenARealtimeClock, ThreadsafeObservationsCanHaveNoSubSecondPart)
{
    Thread_safe::Real_time::Observation t0 { 1629812650000000_usec }; // 2021-08-24 13:44:10

    ASSERT_EQ(t0.year(), 2021);
    ASSERT_EQ(t0.month(), 8);
    ASSERT_EQ(t0.day(), 24);
    ASSERT_EQ(t0.hour(), 13);
    ASSERT_EQ(t0.minute(), 44);
    ASSERT_EQ(t0.second(), 10);
    ASSERT_EQ(t0.milliseconds(), 0);
    ASSERT_EQ(t0.microseconds(), 0);
}


TEST_F(GivenARealtimeClock, ThreadsafeOutputFormatsCanBeSpecified)
{
    Thread_safe::Real_time::Observation t0 { 1629812650837542_usec }; // 2021-08-24 13:44:10.837542

    ASSERT_EQ(t0.to_string(), "2021-08-24T13:44:10.838Z");      // Log format (default);
    ASSERT_EQ(t0.format_as("%F").to_string(), "2021-08-24");
    ASSERT_EQ(t0.format_as("%T").to_string(), "13:44:10");
    ASSERT_EQ(t0.format_as("%T.%ms").to_string(), "13:44:10.838");
    ASSERT_EQ(t0.format_as("%T.%us").to_string(), "13:44:10.837542");
    ASSERT_EQ(t0.format_as("%T.%ms GMT").to_string(), "13:44:10.838 GMT");
    ASSERT_EQ(t0.format_as("%Y%m%d_%H:%M:%S.%ms").to_string(), "20210824_13:44:10.838");
}



TEST_F(GivenARealtimeClock, ThreadsafeObservationsCanBeStreamed)
{
    Thread_safe::Real_time::Observation t0 { 1629812650837542_usec }; // 2021-08-24 13:44:10.837542

    ostringstream os { };

    os << t0;

    ASSERT_EQ(os.str(), "2021-08-24T13:44:10.838Z");

    ostringstream { }.swap(os); // Empty the stringstream

    os << t0.format_as("%F");

    ASSERT_EQ(os.str(), "2021-08-24");
}


TEST_F(GivenARealtimeClock, ThreadsafeObservationsCanBeCopied)
{
    Thread_safe::Real_time::Observation t0 = Clock::now();
    Thread_safe::Real_time::Observation t1 { t0 };

    ASSERT_EQ(t0, t1);
}



TEST_F(GivenARealtimeClock, ThreadsafeObservationsCanBeAssigned)
{
    Thread_safe::Real_time::Observation t0 = now();
    Thread_safe::Real_time::Observation t1 = now() + 5_sec;

    t1 = t0;

    ASSERT_EQ(t0, t1);
}


TEST_F(GivenARealtimeClock, ANullTimeThreadsafeObservationHasNoValue)
{
    Thread_safe::Real_time::Observation t = null_time;

    ASSERT_EQ(t.since_epoch().ticks(), 0);
}


TEST_F(GivenARealtimeClock, AThreadsafeObservationCanBeConstructedFromATimespec)
{
    timespec t { };
    t.tv_sec  = 1'629'812'650; // 2021-08-24 13:44:10.837542
    t.tv_nsec = 837'542'000;

    Thread_safe::Real_time::Observation t0 { t };

    ASSERT_EQ(t0.year(), 2021);
    ASSERT_EQ(t0.month(), 8);
    ASSERT_EQ(t0.day(), 24);
    ASSERT_EQ(t0.hour(), 13);
    ASSERT_EQ(t0.minute(), 44);
    ASSERT_EQ(t0.second(), 10);
    ASSERT_EQ(t0.milliseconds(), 837);
    ASSERT_EQ(t0.microseconds(), 542);
}


TEST_F(GivenARealtimeClock, AThreadsafeObservationCanBeConvertedToAnNTPTime)
{
    Thread_safe::Real_time::Observation t0 { 1629812650837542_usec }; // 2021-08-24 13:44:10.837542

    auto result = t0.to_timespec();
    ASSERT_EQ(result.tv_sec, 1'629'812'650);
    ASSERT_EQ(result.tv_nsec, 837'542'000);
}


TEST_F(GivenARealtimeClock, ThreadsafeAndNonThreadsafeObservationCanBeCombined)
{
    Thread_safe::Real_time::Observation t0 = now();

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
TEST_F(GivenARealtimeClock, DurationsCanBeaAddedToObservations)
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


TEST_F(GivenARealtimeClock, ObservationsCanBeaAddedToDurations)
{
    ASSERT_EQ(null_time + 0_msec, null_time);
    ASSERT_EQ((obs_pos + d_pos).since_epoch().ticks(), Observation { Duration { 400'000'000 } }.since_epoch().ticks());
    ASSERT_EQ(obs_max + 0_msec, obs_max);
    ASSERT_NE(obs_max + d_pos, obs_max);
    ASSERT_TRUE((obs_max + d_neg) < obs_max);
    ASSERT_EQ(null_time + d_neg, null_time);
    ASSERT_EQ(obs_pos + d_neg, null_time);
    ASSERT_EQ(obs_max + d_max, null_time);  // Overflow will lead to negative result
}


TEST_F(GivenARealtimeClock, DurationsCanBeSubtractedFromObservations)
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


TEST_F(GivenARealtimeClock, ObservationsCanBeCompared)
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


TEST_F(GivenARealtimeClock, AbsoluteDifferenceBetweenObservationAndDuration)
{
    ASSERT_EQ(abs_diff(null_time, 0_msec), 0_msec);
    ASSERT_EQ(abs_diff(null_time, d_neg), d_pos);
    ASSERT_EQ(abs_diff(null_time, d_pos), d_pos);

    ASSERT_EQ(abs_diff(obs_pos, d_pos), 0_msec);
    ASSERT_EQ(abs_diff(obs_pos, d_neg), 400_sec);

    ASSERT_EQ(abs_diff(obs_max, d_max).ticks(), 0);
    ASSERT_EQ(abs_diff(obs_max, d_min).ticks(), 1);
}


TEST_F(GivenARealtimeClock, AbsoluteDifferenceBetweenObservations)
{
    ASSERT_EQ(abs_diff(null_time, null_time), 0_msec);

    ASSERT_EQ(abs_diff(obs_pos, null_time), 200_sec);
    ASSERT_EQ(abs_diff(null_time, obs_pos), 200_sec);
}


// -----------------------------------------------------------------------
// Threadsafe operator overload tests
//
TEST_F(GivenARealtimeClock, ThreadsafeDurationsCanBeaAddedToObservations)
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


TEST_F(GivenARealtimeClock, ThreadsafeObservationsCanBeaAddedToDurations)
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


TEST_F(GivenARealtimeClock, ThreadsafeDurationsCanBeSubtractedFromObservations)
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


TEST_F(GivenARealtimeClock, ThreadsafeObservationsCanBeCompared)
{
    Thread_safe::Real_time::Observation obs1 { now() };
    Thread_safe::Real_time::Observation obs2 { obs1 - 500_msec };
    Thread_safe::Real_time::Observation obs3 { obs2 + 500_msec };

    ASSERT_TRUE(obs1 == obs3);
    ASSERT_TRUE(obs2 != obs3);
    ASSERT_TRUE(obs1 > obs2);
    ASSERT_TRUE(obs2 < obs1);
    
    ASSERT_FALSE(obs1 > obs3);
    ASSERT_FALSE(obs3 < obs1);

    ASSERT_TRUE(obs1 >= obs3);
    ASSERT_TRUE(obs3 <= obs1);
}


TEST_F(GivenARealtimeClock, AbsoluteDifferenceBetweenThreadsafeObservationAndDuration)
{
    ASSERT_EQ(abs_diff(null_time, 0_msec), 0_msec);
    ASSERT_EQ(abs_diff(null_time, ts_d_neg), ts_d_pos);
    ASSERT_EQ(abs_diff(null_time, ts_d_pos), ts_d_pos);

    ASSERT_EQ(abs_diff(ts_obs_pos, ts_d_pos), 0_msec);
    ASSERT_EQ(abs_diff(ts_obs_pos, ts_d_neg), 400_sec);

    ASSERT_EQ(abs_diff(ts_obs_max, ts_d_max).ticks(), 0);
    ASSERT_EQ(abs_diff(ts_obs_max, ts_d_min).ticks(), 1);
}


TEST_F(GivenARealtimeClock, AbsoluteDifferenceBetweenThreadsafeDurations)
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


TEST_F(GivenARealtimeClock, AbsoluteDifferenceBetweenThreadsafeObservations)
{
    ASSERT_EQ(abs_diff(null_time, null_time), 0_msec);

    ASSERT_EQ(abs_diff(ts_obs_pos, null_time), 200_sec);
    ASSERT_EQ(abs_diff(null_time, ts_obs_pos), 200_sec);
}

#if 0
#endif // if 0
