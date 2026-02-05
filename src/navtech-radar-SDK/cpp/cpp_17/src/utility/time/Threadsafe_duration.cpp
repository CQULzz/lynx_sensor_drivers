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
#include "Threadsafe_duration.h"

namespace Navtech::Time::Thread_safe {

    Duration::Duration(const Time::Duration& d) : Time::Duration { d }
    {
    }


    Duration::Duration(const Duration& other) : Time::Duration { other }
    {
        std::lock_guard other_lock { other.mtx };
        std::lock_guard this_lock  { this->mtx };

        this->duration = other.duration;
    }


    Duration& Duration::operator=(Duration rhs)
    {
        swap(*this, rhs);
        return *this;
    }


    void swap(Duration& lhs, Duration& rhs)
    {
        using std::swap;
        swap(lhs.duration, rhs.duration);
    }


    Tick_type Duration::ticks() const
    {
        std::lock_guard lock { mtx };
        return Time::Duration::ticks();
    }


    std::uint64_t Duration::in_usec() const
    {
        std::lock_guard lock { mtx };
        return Time::Duration::in_usec();
    }


    float Duration::in_msec() const
    {
        std::lock_guard lock { mtx };
        return Time::Duration::in_msec();
    }


    float Duration::in_sec() const
    {
        std::lock_guard lock { mtx };
        return Time::Duration::in_sec();
    }


    Duration Duration::to_nearest_millisecond() const
    {
        std::lock_guard lock { mtx };
        return Time::Duration::to_nearest_millisecond();
    }


    Duration Duration::to_nearest_second() const
    {
        std::lock_guard lock { mtx };
        return Time::Duration::to_nearest_second();
    }


    std::string Duration::to_string() const
    {
        std::lock_guard lock { mtx };
        return Time::Duration::to_string();
    }


    Time::Duration Duration::unlocked() const
    {
        std::lock_guard lock { mtx };
        return Time::Duration { Time::Duration::ticks() };
    }


    Duration& Duration::operator+=(const Duration& rhs)
    {
        std::lock_guard lock { mtx };
        Time::Duration::operator+=(rhs);
        return *this;
    }


    Duration& Duration::operator-=(const Duration& rhs)
    {
        std::lock_guard lock { mtx };
        Time::Duration::operator-=(rhs);
        return *this;
    }
    

    Duration& Duration::operator*=(double rhs)
    {
        std::lock_guard lock { mtx };
        Time::Duration::operator*=(rhs);
        return *this;
    }
    

    Duration& Duration::operator/=(double rhs)
    {
        std::lock_guard lock { mtx };
        Time::Duration::operator/=(rhs);
        return *this;
    }

} // namespace Navtech::Time::Thread_safe