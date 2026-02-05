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
#include <iomanip>
#include <sstream>

#include "Realtime_observation.h"
#include "Duration.h"
#include "Monotonic_observation.h"
#include "Threadsafe_duration.h"
#include "Threadsafe_monotonic.h"
#include "Time_ops.h"


namespace Navtech::Time::Real_time {

    Observation null_time { };

    // ------------------------------------------------------------------------------------------------------------
    //
    Observation::Observation(const Duration& init) : 
        time { std::chrono::microseconds { init.ticks() } }
    {
    }


    Observation::Observation(const time_t& init) : 
        time { 
            std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::system_clock::from_time_t(init).time_since_epoch()
            )
        }
    {
    }


    Observation::Observation(const timespec& init) : 
        time {
            std::chrono::microseconds { (init.tv_sec * 1'000'000) + (init.tv_nsec / 1000) }
        }
    {
    }


    Duration Observation::since_epoch() const
    {
        return Duration {
            static_cast<Tick_type>(
                time
                .time_since_epoch()
                .count()
            )
        };
    }


    int Observation::year() const
    {
        auto time = to_time_t();
        auto tm = std::gmtime(&time);

        return tm->tm_year + 1900;
    }


    int Observation::month() const
    {
        auto time = to_time_t();
        auto tm = std::gmtime(&time);

        return tm->tm_mon + 1;
    }


    int Observation::day() const
    {
        auto time = to_time_t();
        auto tm = std::gmtime(&time);

        return tm->tm_mday;
    }


    int Observation::hour() const
    {
        auto time = to_time_t();
        auto tm = std::gmtime(&time);

        return tm->tm_hour;
    }


    int Observation::minute() const
    {
        auto time = to_time_t();
        auto tm = std::gmtime(&time);

        return tm->tm_min;
    }


    int Observation::second() const
    {
        auto time = to_time_t();
        auto tm = std::gmtime(&time);

        return tm->tm_sec;
    }


    Duration Observation::get_subseconds() const
    {
        using std::chrono::time_point;
        using std::chrono::duration;
        using std::ratio_multiply;
        using std::ratio;
        using std::chrono::hours;
        using std::chrono::minutes;
        using std::chrono::seconds;
        using std::chrono::duration_cast;
        using days = duration<int, ratio_multiply<hours::period, ratio<24>>::type>;

        auto since_epoch = time.time_since_epoch();

        days d = duration_cast<days>(since_epoch);
        since_epoch -= d;
        hours h = duration_cast<hours>(since_epoch);
        since_epoch -= h;
        minutes m = duration_cast<minutes>(since_epoch);
        since_epoch -= m;
        seconds s = duration_cast<seconds>(since_epoch);
        since_epoch -= s;

        return Duration { static_cast<Tick_type>(since_epoch.count()) };
    }


    int Observation::milliseconds() const
    {
        return static_cast<int>(get_subseconds().ticks() / 1000);
    }


    int Observation::microseconds() const
    {
        return static_cast<int>(get_subseconds().ticks() - (milliseconds() * 1000));
    }


    Observation& Observation::format_as(std::string_view fmt)
    {
        format = fmt;
        return *this;
    }


    std::string Observation::to_string() const
    {
        using std::chrono::system_clock;
        using std::ostringstream;
        using std::string_view;
        using std::string;
        using std::put_time;
        using std::gmtime;
        using std::fixed;
        using std::setprecision;
        using std::round;

        ostringstream os { };
        const time_t c_time  { system_clock::to_time_t(time) };
        string_view msec_fmt { R"(%ms)" };
        string_view usec_fmt { R"(%us)" };
        string output_format { format };
        string postscript    { };

        // Parse and remove special markers for msec and usec.
        // It is expected the %ms/&us markers will be the last
        // formatters in the string.
        // If there are any 'postscript' characters after the %ms/%us
        // formatter strip them out to a temporary holder, to be added at
        // the end of the output string.
        //
        bool show_msec { false };
        bool show_usec { false };

        if (auto pos = output_format.find(msec_fmt); pos != string::npos) {
            auto postscript_sz = output_format.size() - (pos + msec_fmt.length());
            
            postscript = output_format.substr(
                pos + msec_fmt.length(), 
                postscript_sz
            );
            
            output_format.replace(
                pos, 
                msec_fmt.length() + postscript_sz, 
                ""
            );
            show_msec = true;
        }

        if (auto pos = output_format.find(usec_fmt); pos != string::npos) {
            auto postscript_sz = output_format.size() - (pos + usec_fmt.length());
            
            postscript = output_format.substr(
                pos + usec_fmt.length(), 
                postscript_sz
            );
            
            output_format.replace(
                pos, 
                usec_fmt.length() + postscript_sz, 
                ""
            );
            show_usec = true;
        }

        // Output the 'normal' formatted string
        //
        os << put_time(gmtime(&c_time), output_format.c_str());

        // Add milliseconds, or microseconds, as required
        //
        if (show_msec) {
            os << fixed << setprecision(3);
            os << static_cast<unsigned int>(round(get_subseconds().in_usec() / 1000.0));
        }

        if (show_usec) {
            os << fixed << setprecision(6);
            os << get_subseconds().in_usec();
        }

        // Finish off with the postscript text
        //
        os << postscript;

        return os.str();
    }


    time_t Observation::to_time_t() const
    {
        return std::chrono::system_clock::to_time_t(time);
    }


    timespec Observation::to_timespec() const
    {
        // Timespec returns its sub-seconds as nsec
        //
        return timespec {
            to_time_t(),
            static_cast<long>(get_subseconds().in_usec() * 1000)
        };
    }


    Monotonic::Observation Observation::to_monotonic() const
    {
        return Monotonic::Observation { this->since_epoch() };
    }


    Observation& Observation::operator+=(const Duration& d)
    {
        std::chrono::microseconds offset { d.ticks() };
        time += offset;
        return *this;
    }


    Observation& Observation::operator-=(const Duration& d)
    {
        std::chrono::microseconds offset { d.ticks() };
        time -= offset;
        return *this;
    }


    std::ostream& operator<<(std::ostream& os, const Observation& obs)
    {
        os << obs.to_string();
        return os;
    }


    Duration abs_diff(const Observation& lhs, const Observation& rhs)
    {
        if (lhs >= rhs) return Duration { lhs.since_epoch() - rhs.since_epoch() };
        else            return Duration { rhs.since_epoch() - lhs.since_epoch() };
    }


    // ------------------------------------------------------------------------------------------------------------
    //
    Observation Clock::now()
    {
        return Observation { 
            Duration {
                static_cast<Tick_type>(
                    std::chrono::duration_cast<std::chrono::microseconds>(
                        std::chrono::system_clock::now().time_since_epoch()
                    )
                    .count()
                )
            }
        };
    }


    Observation now()
    {
        return Clock::now();            
    }


    // ------------------------------------------------------------------------------------------------------------
    //
    Duration microseconds_since(const Observation& obs)
    {
        return (Clock::now() - obs);
    }


    Duration milliseconds_since(const Observation& obs)
    {
        return to_nearest_millisecond(Clock::now() - obs);
    }


    Duration seconds_since(const Observation& obs)
    {
        return to_nearest_second(Clock::now() - obs);
    }


} // namespace Navtech::Time::Real_time