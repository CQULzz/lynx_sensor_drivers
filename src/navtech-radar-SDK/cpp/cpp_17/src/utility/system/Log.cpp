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
#include <iostream>

#include "Log.h"
#include "Time_utils.h"


namespace Navtech::Utility {

    // Global instance
    //
    Log syslog { };


    Log::Log() : Active { "Console log" }
    {
        allocate_formatter();
        Active::start();
    }


    Log::~Log()
    {
        Active::stop();
        Active::join();
    }


    void Log::on_start()
    {
        debug("Starting log...");
        debug("Output format [" + to_string(log_format) + "]");
        debug("Logging level [" + to_string(min_log_level) + "]");
    }


    void Log::on_stop()
    {
        debug("Stopping log...");
    }

    
    void Log::write(const std::string& text, Logging_level level)
    {
        async_call(&Log::write_impl, this, Time::Real_time::now(), text, level);
    }


    void Log::write_impl(const Time::Real_time::Observation& now, const std::string& text, Logging_level level)
    {
        if (level < min_log_level) return;

        log_formatter->time(now);
        log_formatter->level(level);
        log_formatter->message(text);

        std::cout << log_formatter->get() << std::endl;
    }


    void Log::write(const std::string& text)
    {
        write(text, Logging_level::info);
    }


    void Log::write(const Log::Stream& stream)
    {
        write(stream.str(), Logging_level::info);
    }


    void Log::write(const Log::Stream& stream, Logging_level level)
    {
        write(stream.str(), level);
    }


    void Log::debug(const std::string& text)
    {
        write(text, Logging_level::debug);
    }


    void Log::debug(const Log::Stream& stream)
    {
        write(stream.str(), Logging_level::debug);
    }


    void Log::info(const std::string& text)
    {
        write(text, Logging_level::info);
    }


    void Log::info(const Log::Stream& stream)
    {
        write(stream.str(), Logging_level::info);
    }


    void Log::warning(const std::string& text)
    {
        write(text, Logging_level::warning);
    }


    void Log::warning(const Log::Stream& stream)
    {
        write(stream.str(), Logging_level::warning);
    }


    void Log::error(const std::string& text)
    {
        write(text, Logging_level::error);
    }


    void Log::error(const Log::Stream& stream)
    {
        write(stream.str(), Logging_level::error);
    }


    void Log::critical(const std::string& text)
    {
        write(text, Logging_level::critical);
    }


    void Log::critical(const Log::Stream& stream)
    {
        write(stream.str(), Logging_level::critical);
    }


    void Log::linebreak(Log::Break_type br)
    {
        switch (br) {
        case Break_type::dots:
            async_call([] { std::cout << ". . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ." << std::endl; });
            break;

        case Break_type::dashes:
            async_call([] { std::cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -" << std::endl; });
            break;
        
        case Break_type::line:
            async_call([] { std::cout << "-------------------------------------------------------------------------------" << std::endl; });
            break;

        case Break_type::double_line:
            async_call([] { std::cout << "===============================================================================" << std::endl; });
            break;
        }   
    }

    void Log::min_level(Logging_level level)
    {
        async_call(
            [this](Logging_level l) 
            { 
                min_log_level = l;
                write_impl(Time::Real_time::now(), "Logging level set to [" + to_string(l) + "]", Logging_level::info);
            }, 
            level
        );
    }


    void Log::min_level(const std::string& level_str)
    {
        min_level(logging_level_from_string(level_str));
    }


    void Log::allocate_formatter()
    {
        switch (log_format) {
        case Log_format::message_only:
            log_formatter = allocate_owned<Message_only>();
            break;

        case Log_format::simple:
            log_formatter = allocate_owned<Simple>();
            break;

        case Log_format::high_precision:
            log_formatter = allocate_owned<High_precision>();
            break;

        default:
            log_formatter = allocate_owned<Simple>();
            break;
        }
    }


    void Log::format(Log_format log_fmt)
    {
        async_call(
            [this](Log_format l) 
            { 
                log_format = l;
                allocate_formatter();
            }, 
            log_fmt
        );
    }


    void Log::format(const std::string& log_fmt)
    {
        format(log_format_from_string(log_fmt));
    }

} // namespace Navtech::Utility