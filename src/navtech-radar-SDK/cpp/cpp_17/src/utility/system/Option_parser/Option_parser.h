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
#ifndef OPTION_PARSER_H
#define OPTION_PARSER_H

#include <string_view>
#include <unordered_set>
#include <initializer_list>

#include "Option_group.h"
#include "Option.h"

namespace Navtech::Utility {

    class Option_parser {
    public:
        Option_parser() = default;
        Option_parser(std::initializer_list<Option_group> noun_list);
        // For *some* compatibility with the old option parser
        //
        Option_parser(std::initializer_list<Option> option_list);
        Option_parser(
            std::initializer_list<Option_group> noun_list,
            std::initializer_list<Option> option_list
        );

        Option_parser& add_noun(const Option_group noun);
        Option_parser& add_noun(Option_group&& option);

        std::string usage() const;

        const Option& global(std::string_view option) const;

        void parse(int argc, const char* const argv[]);

        const Option_group& operator[](const std::string & option) const;

    private:
        std::string         name    { "SDK application" };
        std::vector<Option_group>   nouns   { };
        bool has_global_options { false };

        std::vector<std::string> tokenise(int argc, const char* const argv[]);
    };

} // namespace Navtech::Utility


#endif
