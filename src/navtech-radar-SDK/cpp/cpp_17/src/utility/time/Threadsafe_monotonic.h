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
#ifndef THREADSAFE_MONOTONIC_H
#define THREADSAFE_MONOTONIC_H

#include <mutex>

#include "Monotonic_observation.h"


namespace Navtech::Time::Thread_safe::Monotonic {

    // --------------------------------------------------------------------------------
    // A Thread_safe::Monotonic::Observation provides a Monitor wrapper around a 
    // Monotonic::Observation for use in multi-threaded contexts
    //
    class Observation : private Time::Monotonic::Observation {
    public:
        using Time::Monotonic::Observation::Observation;

        Observation(const Time::Monotonic::Observation& other);

        // Copy-move policy
        //
        Observation(const Observation& other);
        Observation& operator=(Observation rhs);
        friend void swap(Observation& lhs, Observation& rhs);
        
        Time::Duration since_epoch()  const;
        Time::Real_time::Observation to_real_time() const;
        timespec to_timespec() const;

        Observation& operator+=(const Time::Duration& rhs);
        Observation& operator-=(const Time::Duration& rhs);
        Observation& operator+=(const Time::Thread_safe::Duration& rhs);
        Observation& operator-=(const Time::Thread_safe::Duration& rhs);

    private:
        mutable std::mutex mtx { };
    };


    // Helper functions to convert an Observation into 
    // a duration
    //
    Duration microseconds_since(const Observation& obs);
    Duration milliseconds_since(const Observation& obs);
    Duration seconds_since(const Observation& obs);


} // namespace Navtech::Time::Thread_safe::Monotonic

#endif // THREADSAFE_MONOTONIC_H