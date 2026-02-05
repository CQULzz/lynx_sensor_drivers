// ---------------------------------------------------------------------------------------------------------------------
// Copyright 2025 Navtech Radar Limited
// This file is part of IASDK which is released under The MIT License (MIT).
// See file LICENSE.txt in project root or go to https://opensource.org/licenses/MIT
// for full license details.
//
// Disclaimer:
// Navtech Radar is furnishing this item "as is". Navtech Radar does not provide 
// any warranty of the item whatsoever, whether express, implied, or statutory,
// including, but not limited to, any warranty of merchantability or fitness
// for a particular purpose or any warranty that the contents of the item will
// be error-free.
// In no respect shall Navtech Radar incur any liability for any damages, including,
// but limited to, direct, indirect, special, or consequential damages arising
// out of, resulting from, or any way connected to the use of the item, whether
// or not based upon warranty, contract, tort, or otherwise; whether or not
// injury was sustained by persons or property or otherwise; and whether or not
// loss was sustained from, or arose out of, the results of, the item, or any
// services that may be provided by Navtech Radar.
// ---------------------------------------------------------------------------------------------------------------------
#include <thread>

#include "Monotonic_observation.h"
#include "Duration.h"
#include "Realtime_observation.h"
#include "Threadsafe_duration.h"
#include "Threadsafe_monotonic.h"
#include "Time_ops.h"

namespace Navtech::Time::Monotonic {

    Observation null_time { };

    // ------------------------------------------------------------------------------------------------------------
    //
    Observation::Observation(const Duration& init) : 
        time { std::chrono::microseconds { init.ticks() } }
    {
    }


    Duration Observation::since_epoch() const
    {
        return Duration {
            static_cast<Tick_type>(
                time
                .time_since_epoch()
                .count()
            )
        };
    }


    Real_time::Observation Observation::to_real_time() const
    {
        return Real_time::Observation { Monotonic::Clock::started_at() + since_epoch() };
    }


    timespec Observation::to_timespec() const
    {
        auto ticks = time.time_since_epoch().count();
        auto sec   = static_cast<time_t>(ticks / 1'000'000);
        auto usec  = static_cast<long>(ticks - (sec * 1'000'000));

        // timespec defines sub-seconds part in nsec
        //
        return timespec { sec, usec * 1000 };
    }


    Observation& Observation::operator+=(const Duration& d)
    {
        std::chrono::microseconds offset { d.ticks() };
        time += offset;
        return *this;
    }


    Observation& Observation::operator-=(const Duration& d)
    {
        std::chrono::microseconds offset { d.ticks() };
        time -= offset;
        return *this;
    }


    Duration abs_diff(const Observation& lhs, const Observation& rhs)
    {
        if (lhs >= rhs) return Duration { lhs.since_epoch() - rhs.since_epoch() };
        else            return Duration { rhs.since_epoch() - lhs.since_epoch() };
    }


    // ------------------------------------------------------------------------------------------------------------
    //
    Observation Clock::now()
    {
        return Observation { 
            Duration {
                static_cast<Tick_type>(
                    std::chrono::duration_cast<std::chrono::microseconds>(
                        std::chrono::steady_clock::now().time_since_epoch()
                    )
                    .count()
                )
            }
        };
    }

    
    Real_time::Observation Clock::started_at()
    {
        auto t_real = Real_time::Clock::now();
        auto t_mono = Monotonic::Clock::now();

        return Real_time::Observation { t_real - t_mono };
    }


    Observation now()
    {
        return Clock::now();            
    }

    // ------------------------------------------------------------------------------------------------------------
    //
    void sleep_for(const Duration& sleep_period)
    {
        std::chrono::microseconds duration { sleep_period.ticks() };
        std::this_thread::sleep_for(duration);
    }


    void sleep_until(const Observation& wakeup_time)
    {
        std::chrono::time_point<std::chrono::steady_clock, std::chrono::microseconds> time { 
            std::chrono::microseconds { wakeup_time.since_epoch().ticks() }
        };

        std::this_thread::sleep_until(time);
    }


    void sleep_for(const Thread_safe::Duration& sleep_period)
    {
        std::chrono::microseconds duration { sleep_period.ticks() };
        std::this_thread::sleep_for(duration);
    }


    void sleep_until(const Thread_safe::Monotonic::Observation& wakeup_time)
    {
        std::chrono::time_point<std::chrono::steady_clock, std::chrono::microseconds> time { 
            std::chrono::microseconds { wakeup_time.since_epoch().ticks() }
        };

        std::this_thread::sleep_until(time);
    }


    // ------------------------------------------------------------------------------------------------------------
    //
    Duration microseconds_since(const Observation& obs)
    {
        return (Clock::now() - obs);
    }


    Duration milliseconds_since(const Observation& obs)
    {
        return to_nearest_millisecond(Clock::now() - obs);
    }


    Duration seconds_since(const Observation& obs)
    {
        return to_nearest_second(Clock::now() - obs);
    }

} // namespace Navtech::Time::Montonic