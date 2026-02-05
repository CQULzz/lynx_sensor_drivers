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
#ifndef LOG_H
#define LOG_H

#include <string>
#include <sstream>

#include "Active.h"
#include "pointer_types.h"
#include "Log_format.h"
#include "Log_level.h"
#include "Time_utils.h"

// ---------------------------------------------------------------------------------------------------------------------
// Asynchronous system logging
// ===========================
//
// The Log type provides asynchronous logging facilities for the SDK.
// An extern (global) Log object is provided that starts on application startup and stops on exit from main.
// 
// Logging
// -------
// The basic call to add to the log is write(). By default this displays the message as logging level 'info'. You can
// output at a different level by either:
// - Adding the logging level as a second parameter to write():
//   syslog.write("This is a message", Logging_level::debug);
//
// - By using a write-overload call:
//   syslog.debug("This is a message");
//
// Logging level
// -------------
// The min_level() call allows logging output to be filtered by level.  The default is 'info' - any messages written
// with a logging level of 'info' or above will be displayed.
// The logging level can be updated at runtime by either supplying a Logging_level enum value, or by providing the
// string equivalent. The second approach allows the logging level to be adjusted via a command-line option, for example.
//
// Logging format
// --------------
// The Log implements its formatting via a pImpl Log_formatter object. There are three basic formatters provided:
// - Message only:      This formatter just outputs the provided message text
//
// - Simple:            This formatter outputs time + logging level + message text. 
//                      This is the default output format
//
// - High precision:    As 'simple' but time is logged to the millisecond.
//
// Logging format can be changed at runtime using the format() calls. As with logging level, either a Log_format enum 
// or equivalent string can be provided.
//
// Log streams
// -----------
// in some cases it may be useful to create an output stream (e.g. like std::cout). The Log contains its own std::stream
// variant for this purpose.
// The Log Stream object has the same interface and functionality as a std::ostream with one exception: clear() empties
// the Stream, as this makes using the Stream more intuitive.
// A Log Stream can be used as an IO buffer, then written to the Log using the same write() interface as std::strings.
// For example:
// 
// Log::Stream log_strm { };
// log_strm << "This is a message" << std::endl;
// syslog.debug(log_strm);
//
// ---------------------------------------------------------------------------------------------------------------------

namespace Navtech::Utility {

    class Log : public Utility::Active {
    public:
        class Stream : public std::stringstream {
        public:
            // Override clear() to empty the stream
            // (as you might expect to happen)
            //
            void clear()
            {
                std::stringstream().swap(*this);
            }
        };

        Log();
        ~Log();

        void write(const std::string& text);                         
        void write(const Stream& stream);                            

        void write(const std::string& text, Logging_level level);    
        void write(const Stream& stream,    Logging_level level);    

        void debug(const std::string& text);                         
        void debug(const Stream& stream);                            

        void info(const std::string& text);                          
        void info(const Stream& stream);                             

        void warning(const std::string& text);                       
        void warning(const Stream& stream);                          

        void error(const std::string& text);                         
        void error(const Stream& stream);                            

        void critical(const std::string& text);                      
        void critical(const Stream& stream);                         

        enum class Break_type { dots, dashes, line, double_line };
        void linebreak(Break_type br = Break_type::line);

        void min_level(Logging_level level);
        void min_level(const std::string& level_str);

        void format(Log_format log_format);
        void format(const std::string& log_format);

    protected:
        void on_start() override;
        void on_stop()  override;

    private:
        Logging_level           min_log_level { Logging_level::info };
        Log_format              log_format    { Log_format::simple };
        owner_of<Log_formatter> log_formatter { };

        void write_impl(const Time::Real_time::Observation& now, const std::string& text, Logging_level level);
        void allocate_formatter();      
    };


    extern Log syslog;


} // namespace Navtech::Utility

#endif // LOG_H