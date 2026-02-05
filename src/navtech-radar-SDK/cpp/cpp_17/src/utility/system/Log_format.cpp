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

#include "Log_format.h"

namespace Navtech::Utility {

    // -----------------------------------------------------------------------------------------------------------------
    //
    void Message_only::time(const Time::Real_time::Observation& now [[maybe_unused]])
    {
        // Do nothing
    }


    void Message_only::level(Logging_level level [[maybe_unused]])
    {
        // Do nothing
    }   


    void Message_only::message(const std::string& text)
    {
        msg_str = text;
    }   


    std::string Message_only::get() const
    {
        return msg_str;
    }


    // -----------------------------------------------------------------------------------------------------------------
    //
    void Simple::time(const Time::Real_time::Observation& now)
    {
        Time::Real_time::Observation t { now };

        time_str = t.format_as("%T").to_string();
    }


    void Simple::level(Logging_level level)
    {
        level_str = std::string { "[" + to_string(level) + "]" };
    }   


    void Simple::message(const std::string& text)
    {
        msg_str = text;
    }   


    std::string Simple::get() const
    {
        std::stringstream strm { };

        strm << std::left << std::setw(12) << time_str << " : ";
        strm << std::left << std::setw(10) << level_str;
        strm << " - ";
        strm << msg_str;

        return strm.str();
    }



    // -----------------------------------------------------------------------------------------------------------------
    //                             
    void High_precision::time(const Time::Real_time::Observation& now)
    {
        Time::Real_time::Observation t { now };

        time_str = t.format_as("%T.%ms").to_string();
    }


    std::string High_precision::get() const
    {
        std::stringstream strm { };

        strm << std::left << std::setw(15) << time_str << " : ";
        strm << std::left << std::setw(10) << level_str;
        strm << " - ";
        strm << msg_str;

        return strm.str();
    }

} // namespace Navtech::Utility