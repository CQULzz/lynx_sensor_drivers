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
#include "Option.h"

namespace Navtech::Utility {

    // ------------------------------------------------------------------------
    // Constructors
    //
    Option::Option(
        std::string_view    name,
        std::string_view    short_name,
        std::string_view    help_text,
        Optionality         reqd_or_opt,
        Requires_argument   argument,
        std::string_view    argument_default
    ) : 
        long_name       { remove_prefix(name) },
        short_name      { remove_prefix(short_name) },
        help_text       { help_text },
        is_optional     { reqd_or_opt == Optionality::optional },
        has_arg         { argument == Requires_argument::has_argument },
        arg_value       { argument_default },
        opt_found       { argument_default != "" }
    {
    }


    // ------------------------------------------------------------------------
    // Public Methods and operators
    //
    bool Option::operator==(std::string_view token) const
    {
        auto naked_token = remove_prefix(token);
        return naked_token == long_name || naked_token == short_name;
    }
    

    void Option::check()
    {
        if (!opt_found && !is_optional) {
            throw std::invalid_argument(
                "Missing required Option [--" + long_name + "]"
            );
        }

        if (has_arg && arg_value == "") {
            throw std::invalid_argument(
                "Option [--" + long_name + "] requires one argument, but none were found"
            );
        }
    }


    const std::string& Option::value() const
    {
        return arg_value;
    }


    void Option::value(std::string_view val)
    {
        if (opt_found && updated) {
            throw std::invalid_argument(
                "Too many arguments for option [--" + long_name + "]: [" +
                arg_value + ", " + std::string { val } + "]"
            );
        }

        arg_value = val;
        updated = true;
        opt_found = true;
    }


    std::string Option::help() const
    {
        std::stringstream oss { };

        oss << (is_optional ? "[Optional] " : "[Required] ");
        oss << "--" << long_name;
        oss << ", ";
        oss << "-" << short_name << "";
        oss << " : " << help_text << " ";

        if (has_arg) {
            if (!arg_default.empty()) {
                oss << "[Default: " << arg_default << "]";
            }
        }

        return oss.str();
    }


    std::string Option::usage() const
    {
        std::stringstream oss { };

        if (is_optional) oss << "[";

        oss << "--" << long_name << " ";
        if (has_arg) oss << "<val>";

        oss << " | ";

        oss << "-" << short_name << " ";
        if (has_arg) oss << "<val>";

        if (is_optional) oss << "]";

        return oss.str();
    }


    // ------------------------------------------------------------------------
    // Private Methods
    //
    std::string Option::remove_prefix(std::string_view token) const
    {
        std::string name { token };

        // Remove leading hyphens from the name
        // to leave a 'raw' identifier
        //
        name.erase(
            std::remove_if(
                name.begin(), 
                name.end(),
                [](const char chr) { return (chr == '-'); }
            ),
            name.end()
        );

        return name;
    }
}