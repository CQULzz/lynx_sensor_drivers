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
#ifndef LOG_LEVEL_H
#define LOG_LEVEL_H

#include <string>

namespace Navtech::Utility {

    enum class Logging_level {
        debug       = 0,
        info        = 1,
        warning     = 2,
        error       = 4,
        critical    = 8
    };


    inline constexpr bool operator==(Logging_level lhs, Logging_level rhs)
    {
        auto lhs_val { static_cast<int>(lhs) };
        auto rhs_val { static_cast<int>(rhs) };
        return lhs_val == rhs_val;
    }


    inline constexpr bool operator>(Logging_level lhs, Logging_level rhs)
    {
        auto lhs_val { static_cast<int>(lhs) };
        auto rhs_val { static_cast<int>(rhs) };
        return lhs_val > rhs_val;
    }


   inline constexpr bool operator<(Logging_level lhs, Logging_level rhs)
    {
        auto lhs_val { static_cast<int>(lhs) };
        auto rhs_val { static_cast<int>(rhs) };
        return lhs_val < rhs_val;
    }


    inline std::string to_string(Logging_level level)
    {
        switch (level) {
        case Logging_level::debug:      return "debug";
        case Logging_level::info:       return "info";
        case Logging_level::warning:    return "warning";
        case Logging_level::error:      return "error";
        case Logging_level::critical:   return "critical";
        default:                        return "unknown";
        }
    }

    inline Logging_level logging_level_from_string(const std::string& str)
    {
        if (str == "debug")     return Logging_level::debug;
        if (str == "info")      return Logging_level::info;
        if (str == "warning")   return Logging_level::warning;
        if (str == "error")     return Logging_level::error;
        if (str == "critical")  return Logging_level::critical;
        else                    return Logging_level::debug;
    }
    
} // namespace Navtech::Utility


#endif // LOG_LEVEL_H