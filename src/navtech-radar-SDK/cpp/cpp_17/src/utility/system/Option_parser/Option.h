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
#ifndef OPTION_H
#define OPTION_H

#include <initializer_list>
#include <functional>
#include <string>
#include <string_view>
#include <sstream>
#include <unordered_set>

namespace Navtech {
    enum Requires_argument  { no_argument, has_argument };
    enum Optionality        { optional, required };
} // namespace Navtech

namespace Navtech::Utility {

    class Option {
    public:
        Option(
            std::string_view    name,
            std::string_view    short_name,
            std::string_view    help_text,
            Optionality         reqd_or_opt,
            Requires_argument   argument,
            std::string_view    argument_default = ""
        );

        operator bool() const
        {
            return opt_found;
        }

        const std::string& name() const { return long_name; }
        bool operator==(std::string_view token) const;

        bool has_args() const { return has_arg; }
        void check();

        const std::string&  value() const;
        void                value(std::string_view val);

        std::string help() const;
        std::string usage() const;

        // Translation methods
        //
        template <typename T> 
        T translate_to(std::function<T(const std::string&)> translator_fn) const
        {
            static_assert(std::is_default_constructible<T>::value);

            if (arg_value.empty()) return T { };
            if (translator_fn) return translator_fn(arg_value);

            return T { };
        }

        template <typename T>
        T translate_to() const
        {
            static_assert(std::is_default_constructible<T>::value);

            if (arg_value.empty()) return T { };
            return T { arg_value };
        }

        template <typename T = int>
        T to_int() const
        {
            if (arg_value.empty()) return { };
            return static_cast<T>(std::stoi(arg_value));
        }

        int to_int() const
        {
            if (arg_value.empty()) return { };
            return std::stoi(arg_value);
        }

        float to_float() const
        {
            if (arg_value.empty()) return { };
            return std::stof(arg_value);
        }

        bool to_bool() const
        {
            if (arg_value.empty()) return { opt_found };

            bool b { };
            std::istringstream(arg_value) >> std::boolalpha >> b;

            return b;
        }
    private:
        std::string long_name   { };
        std::string short_name  { };
        std::string help_text   { };

        bool        is_optional { true };
        bool        has_arg     { false };
        bool        updated     { false };
        std::string arg_value   { };
        std::string arg_default { };
        
        bool opt_found      { false };

        std::string remove_prefix(std::string_view token) const;
    };

} // namespace Navtech::Utility
#endif