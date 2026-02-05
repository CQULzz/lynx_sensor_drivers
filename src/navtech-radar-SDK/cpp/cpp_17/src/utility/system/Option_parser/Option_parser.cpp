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
#include <iostream>
#include <sstream>
#include <filesystem>

#include "Option_parser.h"
#include "Log.h"
#include "string_helpers.h"

namespace Navtech::Utility {

    // ------------------------------------------------------------------------
    // Constructors
    //
    Option_parser::Option_parser(std::initializer_list<Option_group> input_nouns) :
        nouns   { input_nouns }
    {
    }


    // If only given an option list, create a "global" options noun
    //
    Option_parser::Option_parser(std::initializer_list<Option> option_list) :
        nouns               { { "", option_list } },
        has_global_options  { option_list.size() > 0 }
    {
    }

    
    Option_parser::Option_parser(
        std::initializer_list<Option_group>   noun_list,
        std::initializer_list<Option> option_list
    ) :
        nouns               { noun_list },
        has_global_options  { option_list.size() > 0 }
    {
        nouns.emplace_back("", option_list);
    }


    // ------------------------------------------------------------------------
    // Public methods
    //
    Option_parser& Option_parser::add_noun(const Option_group noun)
    {
        nouns.emplace_back(noun);
        return *this;
    }


    Option_parser& Option_parser::add_noun(Option_group&& noun)
    {
        nouns.emplace_back(std::move(noun));
        return *this;
    }


    std::string Option_parser::usage() const
    {
        // Print global options
        std::ostringstream oss { };

        oss << "Usage:" << std::endl;
        
        oss << "\t" << name << "\n";

        for (const auto& noun : nouns) {
            oss << noun.usage();
        }
        oss << std::endl;
        oss << std::endl;

        oss << "Options:" << std::endl;
        for (const auto& noun : nouns) {
            oss << noun.help() << std::endl;
        }

        oss << std::endl;

        return oss.str();
    }


    const Option& Option_parser::global(std::string_view option) const
    {
        try {

            if (std::find(nouns.begin(), nouns.end(), "") == nouns.end()) {
                throw std::invalid_argument(
                    "[" + name + "] has no global options"
                );
            }

            return operator[]("")[option];
        }
        catch (const std::exception& ex) {
            std::cout << ex.what() << std::endl;
            std::cout << usage() << std::endl;
            exit(-1);
        }
    }


    void Option_parser::parse(int argc, const char* const argv[])
    {
        // Extract the executable name from the argv array
        //
        name = std::filesystem::path(argv[0]).filename().string();


        // Turn the elements into tokens
        //
        auto tokens = tokenise(argc, argv);

        // Assume that every option prior to a noun is a global option
        // Grab everything until it reaches the first command
        // Let the nouns deal with throwing exceptions for unknown options
        //
        auto current_noun = has_global_options ? std::find(nouns.begin(), nouns.end(), "") : nouns.end();

        std::vector<std::string> collected_tokens { };
        
        // This is a bad way of getting specific errors,
        // it should probably be reworked
        //
        try
        {   
            for (auto& token : tokens) {
                auto noun_itr = std::find(nouns.begin(), nouns.end(), token);

                if (noun_itr != nouns.end()) {
                    current_noun->parse(collected_tokens);
                    collected_tokens.clear();
                    current_noun = noun_itr;
                    continue;
                }
                // If the current iterator points to nouns.end(),
                // then there are no global options and the first argument is bad
                //
                if (current_noun == nouns.end()) {
                    throw std::invalid_argument(
                        "Invalid option or command: [" + token + "]"
                    );
                }

                collected_tokens.emplace_back(token);
            }
        }
        catch(const std::exception& ex)
        {
            std::cout << std::endl;
            std::cout << "ERROR - " << ex.what() << std::endl;
            std::cout << usage() << std::endl;

            exit(-1);
        }
        

        current_noun->parse(collected_tokens);

        for (auto& noun: nouns) {
            noun.check();
        }
    }


    const Option_group& Option_parser::operator[](const std::string & noun) const
    {
        auto itr = std::find(nouns.begin(), nouns.end(), noun);

        if (itr == nouns.end()) {
            throw std::invalid_argument("Unrecognised Option_group: " + noun);
        }

        return *itr;
    }


    // ------------------------------------------------------------------------
    // Private methods
    //
    std::vector<std::string> Option_parser::tokenise(int argc, const char* const argv[])
    {
        std::vector<std::string> tokens { };

        for (auto i { 1 }; i < argc; ++i) {
            tokens.emplace_back(trim(argv[i]));
        }

        return tokens;
    }
    
} // namespace Navtech::Utility
