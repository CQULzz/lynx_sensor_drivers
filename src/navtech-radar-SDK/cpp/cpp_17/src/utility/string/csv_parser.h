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
#ifndef CSV_PARSER_H
#define CSV_PARSER_H

#include <string>
#include <cstring>

#include "string_helpers.h"

namespace Navtech::Utility {

    // -----------------------------------------------------------------------------------------------------------------
    // Conversion classes, for string->T
    //
    // Base class, for converting integer types
    //
    template <typename T> 
    struct Converter { 
        static T from(const std::string& str)
        {
            return static_cast<T>(std::stoi(str));
        }
    };

    template<>
    struct Converter<float> {
        static float from(const std::string& str)
        {
            return std::stof(str);
        }
    };

    template<>
    struct Converter<double> {
        static double from(const std::string& str)
        {
            return std::stod(str);
        }
    };


    template<>
    struct Converter<std::string> {
        static const std::string& from(const std::string& str)
        {
            return str;
        }
    };


    // -----------------------------------------------------------------------------------------------------------------
    //
    template <typename T>
    class CSV_parser {
    public:
        std::vector<T> parse(const std::string& csv_string);
    };


    template <typename T>
    std::vector<T> CSV_parser<T>::parse(const std::string& csv_string)
    {
        std::vector<T> output { };

        auto tokens = split(csv_string, ',');

        for (auto& token : tokens) {
            trim(token);
            if (token.size() > 0) output.emplace_back(Converter<T>::from(token));
        }
        
        return output;
    }


    // -----------------------------------------------------------------------------------------------------------------
    // Free-function version
    //
    template <typename T>
    std::vector<T> parse_csv(const std::string& csv_string)
    {
        CSV_parser<T> parser { };
        return parser.parse(csv_string);
    }


} // namespace Navtech::Utility

#endif // CSV_PARSER_H