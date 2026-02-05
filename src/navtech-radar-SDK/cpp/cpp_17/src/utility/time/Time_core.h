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
#ifndef TIME_CORE_H
#define TIME_CORE_H

#include <chrono>
#include <cstdint>

#ifdef _WIN32
    #include <WinSock2.h>
    using timeval_sec_t     = long;
    using timeval_subsec_t  = long;
#else
    using timeval_sec_t     = __time_t;
    using timeval_subsec_t  = __suseconds_t;
#endif

// Core definitions used by the time utilities

namespace Navtech::Time {

    // Tag types for operator overloading selection
    //
    class Duration_tag    { };
    class Observation_tag { };

    template <typename T>
    using is_duration = typename std::enable_if<std::is_base_of<Duration_tag, T>::value, bool>::type;

    template <typename T>
    using is_observation = typename std::enable_if<std::is_base_of<Observation_tag, T>::value, bool>::type;


    // The underlying clock representation
    //
    using Tick_type = std::int64_t;

} // namespace Navtech::Time

#endif // TIME_CORE_H