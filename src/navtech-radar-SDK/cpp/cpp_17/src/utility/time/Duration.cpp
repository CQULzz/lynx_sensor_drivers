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
#include <sstream>
#include <iomanip>

#include "Duration.h"
#include "Threadsafe_duration.h"
#include "Time_ops.h"


namespace Navtech::Time {

    Duration& Duration::operator+=(const Duration& rhs)
    {
        this->duration += rhs.duration;
        return *this;
    }

    Duration& Duration::operator-=(const Duration& rhs)
    {
        this->duration -= rhs.duration;
        return *this;
    }

    Duration&Duration:: operator*=(double rhs)
    {
        duration = Duration_type { static_cast<Tick_type>(std::round(this->duration.count() * rhs)) };
        return *this;
    }

    Duration& Duration::operator/=(double rhs)
    {
        if (rhs == 0.0) throw std::underflow_error { "Divide by zero!" };

        duration = Duration_type { static_cast<Tick_type>(std::round(this->duration.count() / rhs)) };
        return *this;
    }


    Duration& Duration::operator+=(const Thread_safe::Duration& rhs)
    {
        this->duration += rhs.to_chrono<std::chrono::microseconds>();
        return *this;
    }


    Duration& Duration::operator-=(const Thread_safe::Duration& rhs)
    {
        this->duration -=rhs.to_chrono<std::chrono::microseconds>();
        return *this;
    }


    timespec Duration::to_timespec() const
    {
        auto ticks       = duration.count();
        auto seconds     = static_cast<time_t>(ticks / 1'000'000);
        auto sub_seconds = static_cast<long>(ticks - (seconds * 1'000'000));

        return timespec { seconds, sub_seconds };
    }


    timeval Duration::to_timeval() const
    {
        auto seconds       = static_cast<timeval_sec_t>(ticks() / 1'000'000'000);
        auto micro_seconds = static_cast<timeval_subsec_t>((ticks() - (seconds * 1'000'000'000)) / 1000);

        return timeval { seconds, micro_seconds };
    }


    std::string Duration::to_string() const
    {
        // The duration is displayed in the most appropriate (that is, in the most 
        // human-readable) units, given the value of the duration; so:
        // - Sub-millisecond values are displayed as whole microseconds
        // - Values greated than 1msec are displayed as (fractional) millisecond values
        // - Durations greater than 1sec are displayed as (fractional) second values
        //
        auto sub_secs = [this]       { return (ticks() - ((ticks() / 1'000'000) * 1'000'000)); };
        auto msec = [sub_secs]       { return (sub_secs() / 1'000); };
        auto usec = [msec, sub_secs] { return ((sub_secs()) - (msec() * 1000)); };

        std::ostringstream os { };
#ifdef _WIN32
        std::string  units     { "us" };
#else
        std::string  units     { "μs" };  // \u03BC => greek letter mu
#endif
        unsigned int precision { 0 };
        double       divisor   { 1.0 };

        if (*this > 1_msec) { 
            divisor   = 1'000.0; 
            units     = "ms";
            precision = (usec() != 0 ? 3 : 0);
        }

        if (*this > 1_sec)  { 
            divisor = 1'000'000.0; 
            units   = "s";
            if (msec() != 0) precision = 3;
            if (usec() != 0) precision = 6;
        }

        os << std::fixed;
        os << std::setprecision(precision);
        os << ticks() / divisor;
        os << units;

        return os.str();
    }

} // namespace Navtech::Time