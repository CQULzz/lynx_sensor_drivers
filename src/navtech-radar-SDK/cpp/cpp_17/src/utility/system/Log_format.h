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
#ifndef LOG_FORMAT_H
#define LOG_FORMAT_H

#include <string>

#include "Log_level.h"
#include "Time_utils.h"


namespace Navtech::Utility {

    enum class Log_format { message_only, simple, high_precision };

    inline std::string to_string(Log_format fmt)
    {
        switch (fmt) {
        case Log_format::message_only:      return "message only";
        case Log_format::simple:            return "simple";
        case Log_format::high_precision:    return "high precision";
        default:                            return "None";
        }
    }


    inline Log_format log_format_from_string(const std::string& str)
    {
        if (str == "message_only")      return Log_format::message_only;
        if (str == "simple")            return Log_format::simple;
        if (str == "high_precision")    return Log_format::high_precision;
        else                            return Log_format::message_only;
    }


    class Log_formatter {
    public:
        virtual ~Log_formatter() = default;

        virtual void time(const Time::Real_time::Observation& now)  = 0;
        virtual void level(Logging_level level)                     = 0;
        virtual void message(const std::string& text)               = 0;
        virtual std::string get() const                             = 0;
    };



    class Message_only : public Log_formatter {
    protected:
        void time(const Time::Real_time::Observation& now)  override;
        void level(Logging_level level)                     override;
        void message(const std::string& text)               override;
        std::string get() const                             override;

        std::string msg_str   { };
    };
    
    
    class Simple : public Message_only {
    protected:
        void time(const Time::Real_time::Observation& now)  override;
        void level(Logging_level level)                     override;
        void message(const std::string& text)               override;
        std::string get() const                             override;

        std::string time_str  { };
        std::string level_str { };
        std::string msg_str   { };
    };



    class High_precision : public Simple {
    protected:
        void time(const Time::Real_time::Observation& now)  override;
        std::string get() const                             override;
    };

} // namespace Navtech::Utility


#endif // LOG_FORMAT_H