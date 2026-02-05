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
#include "Threadsafe_monotonic.h"
#include "Monotonic_observation.h"
#include "Realtime_observation.h"
#include "Threadsafe_duration.h"
#include "Time_ops.h"


namespace Navtech::Time::Thread_safe::Monotonic {

    Observation::Observation(const Time::Monotonic::Observation& init) :
        Time::Monotonic::Observation { init }
    {
    }


    Observation::Observation(const Observation& other) : Time::Monotonic::Observation { other }
    {
        std::lock_guard other_lock { other.mtx };
        std::lock_guard this_lock  { this->mtx };

        this->time = other.time;
    }
    

    Observation& Observation::operator=(Observation rhs)
    {
        swap(*this, rhs);
        return *this;
    }
    
    
    void swap(Observation& lhs, Observation& rhs)
    {
        using std::swap;
        swap(lhs.time, rhs.time);
    }


    Time::Duration Observation::since_epoch() const
    {
        std::lock_guard lock { mtx };
        return Time::Monotonic::Observation::since_epoch();
    }


    Time::Real_time::Observation Observation::to_real_time() const
    {
        std::lock_guard lock { mtx };
        return Time::Monotonic::Observation::to_real_time();
    }


    timespec Observation::to_timespec() const
    {
        std::lock_guard lock { mtx };
        return Time::Monotonic::Observation::to_timespec();
    }


    Observation& Observation::operator+=(const Time::Duration& rhs)
    {
        std::lock_guard lock { mtx };
        Time::Monotonic::Observation::operator+=(rhs);
        return *this;
    }


    Observation& Observation::operator-=(const Time::Duration& rhs)
    {
        std::lock_guard lock { mtx };
        Time::Monotonic::Observation::operator-=(rhs);
        return *this;
    }


    Observation& Observation::operator+=(const Time::Thread_safe::Duration& rhs)
    {
        std::lock_guard lock { mtx };
        Time::Monotonic::Observation::operator+=(Time::Duration { rhs.ticks() });
        return *this;
    }


    Observation& Observation::operator-=(const Time::Thread_safe::Duration& rhs)
    {
        std::lock_guard lock { mtx };
        Time::Monotonic::Observation::operator-=(Time::Duration { rhs.ticks() });
        return *this;
    }


    // ------------------------------------------------------------------------------------------------------------
    //
    Duration microseconds_since(const Observation& obs)
    {
        return (Time::Monotonic::Clock::now() - obs);
    }


    Duration milliseconds_since(const Observation& obs)
    {
        return to_nearest_millisecond(Time::Monotonic::Clock::now() - obs);
    }


    Duration seconds_since(const Observation& obs)
    {
        return to_nearest_second(Time::Monotonic::Clock::now() - obs);
    }


} // namespace Navtech::Time::Thread_safe::Monotonic