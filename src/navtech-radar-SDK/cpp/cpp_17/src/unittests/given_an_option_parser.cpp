#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Option_parser.h"

using namespace Navtech;
using namespace Navtech::Utility;

class GivenAnOptionParser : public ::testing::Test {

};


TEST_F(GivenAnOptionParser, WhenConstructedShouldNotThrow)
{
    auto create_parser = [] () {
        Option_parser option_parser {
            {
                Option_group { "cmd", {
                    Option { "--change", "-c", "Change something", required, has_argument }
                }}
            },
            {
                Option { "--file", "-f", "Filepath", optional, has_argument },
                Option { "--mod", "-m", "modifier", optional, no_argument }
            }
        };

        return option_parser;
    };

    EXPECT_NO_THROW(create_parser());
}


TEST_F(GivenAnOptionParser, AllGlobalOptionsShouldBeDetected)
{
    Option_parser option_parser {
        {
            Option { "--required", "-r", "required argument", required, no_argument },
            Option { "--file", "-f", "Filepath", optional, has_argument },
            Option { "--mod", "-m", "modifier", optional, no_argument }
        }
    };

    // This has to be done to avoid conversion from string literal to 
    // const char*
    const int argc = 5;
    char arg1[] = "program";
    char arg2[] = "-r";
    char arg3[] = "-f";
    char arg4[] = "~/documents/some_file.txt";
    char arg5[] = "-m";
    char* test_argv[argc] = { 
        arg1, arg2, arg3, arg4, arg5
    };

    ASSERT_NO_THROW(option_parser.parse(argc, test_argv));

    ASSERT_TRUE(option_parser.global("-f"));
    ASSERT_TRUE(option_parser.global("-r"));
    ASSERT_TRUE(option_parser.global("-m"));
}


TEST_F(GivenAnOptionParser, GlobalOptionsAreOnlyParsedFirst)
{
        Option_parser option_parser {
        {
            Option { "--file", "-f", "Filepath", optional, has_argument },
            Option { "--mod", "-m", "modifier", optional, no_argument }
        }
    };
}


TEST_F(GivenAnOptionParser, CommandsShouldParseOptions)
{
    Option_parser option_parser {
        Option_group {
            "cmd",
            {
                Option { "--file", "-f", "Filepath", optional, has_argument },
                Option { "--mod", "-m", "modifier", optional, no_argument }
            }
        }
    };
}


TEST_F(GivenAnOptionParser, UnrecognisedCommandDeathTest)
{
    GTEST_FLAG_SET(death_test_style, "threadsafe");

    Option_parser option_parser {
        Option_group {
            "cmd",
            {
                Option { "--file", "-f", "Filepath", optional, has_argument },
                Option { "--mod", "-m", "modifier", optional, no_argument }
            }
        }
    };

    const int argc = 2;

    const char* test_argv[argc] = { 
        "program",
        "blurgh"
    };

    // Option parser prints error message to stdout and quits
    // without throwing an exception. No message expected
    //
    ASSERT_DEATH( { option_parser.parse(argc, test_argv); }, "");
    // ASSERT_EXIT(option_parser.parse(argc, test_argv), testing::ExitedWithCode(255), "");
}


TEST_F(GivenAnOptionParser, OptionsWithNoArgumentsShouldBeDetected)
{
    Option_parser option_parser {
        {
            Option { "--mod", "-m", "Modifier", optional, no_argument },
            Option { "--float", "-f", "Floating point value", optional, has_argument }
        }
    };

    const int argc { 4 };
    const char* test_argv[argc] = {
        "program",
        "-m",
        "-f", "0.2"
    };

    option_parser.parse(argc, test_argv);

    ASSERT_TRUE(option_parser.global("-m"));
}


TEST_F(GivenAnOptionParser, PassingMoreThanOneArgToAnOptionShouldThrow)
{
    Option_parser option_parser {
        {
            Option { "--file", "-f", "Filepath", optional, has_argument, "" }
        }
    };

    // TODO replace this with something ISO-compliant
    //
    const int argc { 4 };
    const char* test_argv[argc] = { 
        "program",
        "-f",
        "~/documents/some_file.txt",
        "x",
    };

    EXPECT_THROW(option_parser.parse(4, test_argv), std::invalid_argument);
}


TEST_F(GivenAnOptionParser, OptionsWithDefaultArgsShouldReturnDefaultValue)
{
    Option_parser option_parser {
        {
            Option { "--file", "-f", "Filepath", optional, has_argument, "/some/file/path" },
            Option { "--int", "-i", "Integer", optional, has_argument, "10" },
            Option { "--float", "-f", "Float", optional, has_argument, "2.2" },
            Option { "--bool", "-b", "Bool", optional, has_argument, "true" }
        }
    };

    const char* test_argv[1] { "test" };
    option_parser.parse(1, test_argv);


    ASSERT_EQ(option_parser.global("-f").value(), "/some/file/path");
    ASSERT_EQ(option_parser.global("-i").to_int<std::uint32_t>(), 10);
}


TEST_F(GivenAnOptionParser, ParsedOptionsShouldCorrectlyConvert)
{
    Option_parser option_parser {
        {
            Option_group {
                "cmd",
                {
                    Option { "--float", "-f", "floating point value", optional, has_argument },
                    Option { "--int", "-i", "Integer Value", optional, has_argument }
                }
            }
        },
        {
            Option { "--global", "-g", "Global Value of some description", optional, has_argument }
        }
    };

    const int argc { 8 };
    const char* test_argv[argc] = { 
        "program",
        "-g", "true",
        "cmd",
        "-f", "3.1415927",
        "-i", "20"
    };

    option_parser.parse(argc, test_argv);

    auto f = option_parser["cmd"]["-f"].to_float();
    auto i = option_parser["cmd"]["-i"].to_int<std::uint32_t>();

    ASSERT_FLOAT_EQ(f, 3.1415927f);
    ASSERT_EQ(i, 20);
    ASSERT_TRUE(option_parser.global("-g").to_bool());
}


TEST_F(GivenAnOptionParser, ShouldThrowIfRequiredGlobalOptionNotFound)
{
    Option_parser option_parser {
        {
            Option { "--float", "-f", "floating point value", required, has_argument }
        }
    };

    const int argc { 1 };
    const char* test_argv[argc] { "program" };

    ASSERT_THROW(option_parser.parse(argc, test_argv), std::invalid_argument);
}


TEST_F(GivenAnOptionParser, ShouldThrowIfRequiredNounOptionNotFound)
{
    Option_parser option_parser {
        {
            Option_group {
                "cmd",
                {
                    Option { "--float", "-f", "floating point value", required, has_argument },
                    Option { "--int", "-i", "Integer Value", optional, has_argument }
                }
            }
        },
        {
            Option { "--global", "-g", "Global Value of some description", optional, has_argument }
        }
    };

    const int argc { 6 };
    const char* test_argv[argc] = { 
        "program",
        "-g", "true",
        "cmd",
        "-i", "20"
    };

    EXPECT_THROW(option_parser.parse(argc, test_argv), std::invalid_argument);
}
