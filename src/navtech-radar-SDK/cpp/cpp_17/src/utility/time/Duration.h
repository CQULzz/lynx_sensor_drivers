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
#ifndef DURATION_H
#define DURATION_H

#include <cmath>
#include <string>

#include "Time_core.h"

namespace Navtech::Time {

    // Forward references
    //
    namespace Thread_safe { class Duration; }

    // --------------------------------------------------------------------------------
    // A Duration represents a period of time, as a number of microseconds.
    //
    // Durations act similar to numerical types, but with modified characteristics due 
    // to the fact that represent a physical phenomenon, rather than being purely numeric.
    // 
    // - Durations may be compared for equality/inequality and greater-than/less-than.
    // - Addition represents lengthening the duration. Two Durations may be added to yield 
    //   a new Duration whose value is the simple sum of the two Durations.
    // - Subtraction represents shortening the duration. Subtracting a larger Duration 
    //   from a smaller Duration will yield a negative Duration (which is not physically 
    //   realizable).
    // - If the difference between two Durations is required, prefer Monotonic::abs_diff.
    // - Two Durations cannot be multiplied since this has no physical meaning.
    // - Two Durations may be divided. The result will be a positive integer, representing
    //   the number of times the divisor Duration goes into the numerator.
    // - Durations may be scaled by multiplying or dividing by a scale factor; either 
    //   integer or floating-point.
    //
    // See the unit tests for examples of Duration use cases
    //
    class Duration : public Duration_tag {
    public:
        constexpr Duration() = default;

        constexpr explicit Duration(Tick_type init) : duration { init }
        {
        }

        constexpr explicit Duration(const timespec& init) :
            duration { std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::seconds { init.tv_sec } + std::chrono::microseconds { init.tv_nsec / 1000 }) }
        {
        }

        constexpr explicit Duration(const timeval& init) :
            duration { std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::seconds { init.tv_sec } + std::chrono::microseconds { init.tv_usec }) }
        {
        }

        constexpr Tick_type ticks() const
        {
            return duration.count();
        }

        // Conversion to scaler types
        //
        constexpr std::uint64_t in_usec() const
        {
            return ticks();
        }

        constexpr float in_msec() const
        {
            return (in_usec() / 1000.0f);
        }

        constexpr float in_sec() const
        {
            return (in_msec() / 1000.0f);
        }

        Duration to_nearest_millisecond() const
        {
            double num_msec = std::round(duration.count() / 1000.0);
            return Duration { static_cast<Tick_type>(num_msec * 1000)  };
        }

        Duration to_nearest_second() const
        {
            double num_sec = std::round(duration.count() / 1'000'000.0);
            return Duration { static_cast<Tick_type>(num_sec * 1'000'000)  };
        }

        constexpr static Duration forever()
        {
            return Duration { static_cast<Tick_type>(Duration_type::max().count()) };
        }

        template <typename Chrono_Ty>
        Chrono_Ty to_chrono() const
        {
            return std::chrono::duration_cast<Chrono_Ty>(duration);
        }

        timespec    to_timespec() const;
        timeval     to_timeval() const;
        std::string to_string() const;

        Duration& operator+=(const Duration& rhs);
        Duration& operator-=(const Duration& rhs);
        Duration& operator*=(double rhs);
        Duration& operator/=(double rhs);

        Duration& operator+=(const Thread_safe::Duration& rhs);
        Duration& operator-=(const Thread_safe::Duration& rhs);

    protected:
        using Duration_type = std::chrono::microseconds;

        Duration_type duration { };
    };
    

    inline Duration to_nearest_millisecond(const Duration& d)
    {
        return d.to_nearest_millisecond();
    }


    inline Duration to_nearest_second(const Duration& d)
    {
        return d.to_nearest_second();
    }


    constexpr inline Duration to_usec_duration(unsigned long long val)
    {
        return Duration { static_cast<Tick_type>(val) };
    }


    constexpr inline Duration to_msec_duration(long double val)
    {
        return to_usec_duration(static_cast<unsigned long long>(val * 1000.0));
    }


    constexpr inline Duration to_sec_duration(long double val)
    {
        return to_msec_duration(val * 1000.0);
    }


    inline std::ostream& operator<<(std::ostream& os, const Duration& d)
    {
        os << d.to_string();
        return os;
    }

} // namespace Navtech::Time


// User-defined literals
//
namespace Navtech {

    constexpr Time::Duration operator""_usec(unsigned long long val)
    {
        return Time::Duration { static_cast<Time::Tick_type>(val) };
    }


    constexpr Time::Duration operator""_msec(unsigned long long val)
    {
        return operator""_usec(val * 1000ULL);
    }


    constexpr Time::Duration operator""_msec(long double val)
    {
        return operator""_usec(static_cast<Time::Tick_type>(val * 1000.0L));
    }


    constexpr Time::Duration operator""_sec(unsigned long long val)
    {
        return operator""_msec(val * 1000ULL);
    }


    constexpr Time::Duration operator""_sec(long double val)
    {
        return operator""_msec(val * 1000.0L);
    }


    constexpr Time::Duration operator""_min(unsigned long long val)
    {
        return operator""_sec(val * 60ULL);
    }


    constexpr Time::Duration operator""_min(long double val)
    {
        return operator""_sec(val * 60.0L);
    }


    constexpr Time::Duration operator""_hour(unsigned long long val)
    {
        return operator""_min(val * 60ULL);
    }


    constexpr Time::Duration operator""_hour(long double val)
    {
        return operator""_min(val * 60.0L);
    }


    constexpr Time::Duration operator""_day(unsigned long long val)
    {
        return operator""_hour(val * 24ULL);
    }


    constexpr Time::Duration operator""_day(long double val)
    {
        return operator""_hour(val * 24.0L);
    }

} // namespace Navtech

#endif // DURATION_H