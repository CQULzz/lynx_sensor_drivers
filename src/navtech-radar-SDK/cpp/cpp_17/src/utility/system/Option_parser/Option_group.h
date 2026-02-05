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
#ifndef NOUN_H
#define NOUN_H

#include <initializer_list>
#include <string_view>
#include <vector>

#include "Option.h"

namespace Navtech::Utility {

    class Option_group {
    public:
        Option_group(std::string_view name, std::initializer_list<Option> Options);

        operator bool() const {
            return noun_found; 
        }

        Option_group& add_option(const Option Option);
        Option_group& add_option(Option&& Option);

        bool operator==(std::string_view noun) const;
        const Option& operator[](std::string_view option) const;
        const Option& operator[](const char* option) const;

        std::string usage() const;
        std::string help() const;

        const std::string& name() const { return noun_name; }
        void parse(const std::vector<std::string>& tokens);

        void check();

    private:
        std::string         noun_name    { };
        std::vector<Option> options { };
        bool                noun_found { false };
    };
} // namespace Navtech::Utility

#endif