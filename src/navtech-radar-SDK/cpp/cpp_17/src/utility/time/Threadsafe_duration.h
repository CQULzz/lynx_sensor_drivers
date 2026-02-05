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
#ifndef THREADSAFE_DURATION_H
#define THREADSAFE_DURATION_H

#include <mutex>

#include "Duration.h"


namespace Navtech::Time::Thread_safe {

    // --------------------------------------------------------------------------------
    // A Thread_safe::Duration provides a Monitor wrapper around a Duration
    // for use in multi-threaded contexts
    //
    class Duration : private Time::Duration {
    public:
        using Time::Duration::Duration;
        using Time::Duration::forever;

        Duration(const Time::Duration& d);
        Duration(const Duration& other);
        Duration& operator=(Duration rhs);
        friend void swap(Duration& lhs, Duration& rhs);

        Tick_type     ticks()   const;
        std::uint64_t in_usec() const;
        float         in_msec() const;
        float         in_sec()  const;
        
        Duration     to_nearest_millisecond() const;
        Duration     to_nearest_second() const;
        
        template <typename Chrono_Ty>
        Chrono_Ty to_chrono() const
        {
            std::lock_guard lock { mtx };
            return Time::Duration::to_chrono<Chrono_Ty>();
        }

        std::string to_string() const;

        Time::Duration unlocked() const;

        Duration& operator+=(const Duration& rhs);
        Duration& operator-=(const Duration& rhs);
        Duration& operator*=(double rhs);
        Duration& operator/=(double rhs);

    private:
        mutable std::mutex mtx { };
    };


    inline Duration to_nearest_millisecond(const Duration& d)
    {
        return d.to_nearest_millisecond();
    }


    inline Duration to_nearest_second(const Duration& d)
    {
        return d.to_nearest_second();
    }


    inline std::ostream& operator<<(std::ostream& os, const Duration& d)
    {
        os << d.to_string();
        return os;
    }

} // namespace Navtech::Time::Thread_safe


#endif // THREADSAFE_DURATION_H