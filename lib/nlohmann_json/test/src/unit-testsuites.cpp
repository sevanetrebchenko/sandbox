/*
    __ _____ _____ _____
 __|  |   __|     |   | |  JSON for Modern C++ (test suite)
|  |  |__   |  |  | | | |  version 3.9.1
|_____|_____|_____|_|___|  https://github.com/nlohmann/json

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2013-2019 Niels Lohmann <http://nlohmann.me>.

Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "doctest_compatibility.h"

#include <nlohmann/json.hpp>
using nlohmann::json;

#include <fstream>
#include <test_data.hpp>

TEST_CASE("compliance tests from nlohmann_json.org")
{
    // test cases are from https://json.org/JSON_checker/

    SECTION("expected failures")
    {
        for (auto filename :
                {
                    //TEST_DATA_DIRECTORY "/json_tests/fail1.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_tests/fail2.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_tests/fail3.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_tests/fail4.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_tests/fail5.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_tests/fail6.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_tests/fail7.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_tests/fail8.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_tests/fail9.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_tests/fail10.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_tests/fail11.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_tests/fail12.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_tests/fail13.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_tests/fail14.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_tests/fail15.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_tests/fail16.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_tests/fail17.nlohmann_json",
                    //TEST_DATA_DIRECTORY "/json_tests/fail18.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_tests/fail19.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_tests/fail20.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_tests/fail21.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_tests/fail22.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_tests/fail23.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_tests/fail24.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_tests/fail25.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_tests/fail26.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_tests/fail27.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_tests/fail28.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_tests/fail29.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_tests/fail30.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_tests/fail31.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_tests/fail32.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_tests/fail33.nlohmann_json"
                })
        {
            CAPTURE(filename)
            std::ifstream f(filename);
            json _;
            CHECK_THROWS_AS(_ = json::parse(f), json::parse_error&);
        }
    }

    SECTION("no failures with trailing literals (relaxed)")
    {
        // these tests fail above, because the parser does not end on EOF;
        // they succeed when the operator>> is used, because it does not
        // have this constraint
        for (auto filename :
                {
                    TEST_DATA_DIRECTORY "/json_tests/fail7.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_tests/fail8.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_tests/fail10.nlohmann_json",
                })
        {
            CAPTURE(filename)
            std::ifstream f(filename);
            json j;
            CHECK_NOTHROW(f >> j);
        }
    }

    SECTION("expected passes")
    {
        for (auto filename :
                {
                    TEST_DATA_DIRECTORY "/json_tests/pass1.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_tests/pass2.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_tests/pass3.nlohmann_json"
                })
        {
            CAPTURE(filename)
            std::ifstream f(filename);
            json j;
            CHECK_NOTHROW(f >> j);
        }
    }
}

TEST_CASE("compliance tests from nativejson-benchmark")
{
    // test cases from https://github.com/miloyip/nativejson-benchmark/blob/master/src/main.cpp

    SECTION("doubles")
    {
        auto TEST_DOUBLE = [](const std::string & json_string, const double expected)
        {
            CAPTURE(json_string)
            CAPTURE(expected)
            CHECK(json::parse(json_string)[0].get<double>() == Approx(expected));
        };

        TEST_DOUBLE("[0.0]", 0.0);
        TEST_DOUBLE("[-0.0]", -0.0);
        TEST_DOUBLE("[1.0]", 1.0);
        TEST_DOUBLE("[-1.0]", -1.0);
        TEST_DOUBLE("[1.5]", 1.5);
        TEST_DOUBLE("[-1.5]", -1.5);
        TEST_DOUBLE("[3.1416]", 3.1416);
        TEST_DOUBLE("[1E10]", 1E10);
        TEST_DOUBLE("[1e10]", 1e10);
        TEST_DOUBLE("[1E+10]", 1E+10);
        TEST_DOUBLE("[1E-10]", 1E-10);
        TEST_DOUBLE("[-1E10]", -1E10);
        TEST_DOUBLE("[-1e10]", -1e10);
        TEST_DOUBLE("[-1E+10]", -1E+10);
        TEST_DOUBLE("[-1E-10]", -1E-10);
        TEST_DOUBLE("[1.234E+10]", 1.234E+10);
        TEST_DOUBLE("[1.234E-10]", 1.234E-10);
        TEST_DOUBLE("[1.79769e+308]", 1.79769e+308);
        TEST_DOUBLE("[2.22507e-308]", 2.22507e-308);
        TEST_DOUBLE("[-1.79769e+308]", -1.79769e+308);
        TEST_DOUBLE("[-2.22507e-308]", -2.22507e-308);
        TEST_DOUBLE("[4.9406564584124654e-324]", 4.9406564584124654e-324); // minimum denormal
        TEST_DOUBLE("[2.2250738585072009e-308]", 2.2250738585072009e-308); // Max subnormal double
        TEST_DOUBLE("[2.2250738585072014e-308]", 2.2250738585072014e-308); // Min normal positive double
        TEST_DOUBLE("[1.7976931348623157e+308]", 1.7976931348623157e+308); // Max double
        TEST_DOUBLE("[1e-10000]", 0.0);                                   // must underflow
        TEST_DOUBLE("[18446744073709551616]",
                    18446744073709551616.0);    // 2^64 (max of uint64_t + 1, force to use double)
        TEST_DOUBLE("[-9223372036854775809]",
                    -9223372036854775809.0);    // -2^63 - 1(min of int64_t + 1, force to use double)
        TEST_DOUBLE("[0.9868011474609375]",
                    0.9868011474609375);          // https://github.com/miloyip/rapidjson/issues/120
        TEST_DOUBLE("[123e34]", 123e34);                                  // Fast Path Cases In Disguise
        TEST_DOUBLE("[45913141877270640000.0]", 45913141877270640000.0);
        TEST_DOUBLE("[2.2250738585072011e-308]",
                    2.2250738585072011e-308);
        //TEST_DOUBLE("[1e-00011111111111]", 0.0);
        //TEST_DOUBLE("[-1e-00011111111111]", -0.0);
        TEST_DOUBLE("[1e-214748363]", 0.0);
        TEST_DOUBLE("[1e-214748364]", 0.0);
        //TEST_DOUBLE("[1e-21474836311]", 0.0);
        TEST_DOUBLE("[0.017976931348623157e+310]", 1.7976931348623157e+308); // Max double in another form

        // Since
        // abs((2^-1022 - 2^-1074) - 2.2250738585072012e-308) = 3.109754131239141401123495768877590405345064751974375599... ¡Á 10^-324
        // abs((2^-1022) - 2.2250738585072012e-308) = 1.830902327173324040642192159804623318305533274168872044... ¡Á 10 ^ -324
        // So 2.2250738585072012e-308 should round to 2^-1022 = 2.2250738585072014e-308
        TEST_DOUBLE("[2.2250738585072012e-308]",
                    2.2250738585072014e-308);

        // More closer to normal/subnormal boundary
        // boundary = 2^-1022 - 2^-1075 = 2.225073858507201136057409796709131975934819546351645648... ¡Á 10^-308
        TEST_DOUBLE("[2.22507385850720113605740979670913197593481954635164564e-308]",
                    2.2250738585072009e-308);
        TEST_DOUBLE("[2.22507385850720113605740979670913197593481954635164565e-308]",
                    2.2250738585072014e-308);

        // 1.0 is in (1.0 - 2^-54, 1.0 + 2^-53)
        // 1.0 - 2^-54 = 0.999999999999999944488848768742172978818416595458984375
        TEST_DOUBLE("[0.999999999999999944488848768742172978818416595458984375]", 1.0); // round to even
        TEST_DOUBLE("[0.999999999999999944488848768742172978818416595458984374]",
                    0.99999999999999989); // previous double
        TEST_DOUBLE("[0.999999999999999944488848768742172978818416595458984376]", 1.0); // next double
        // 1.0 + 2^-53 = 1.00000000000000011102230246251565404236316680908203125
        TEST_DOUBLE("[1.00000000000000011102230246251565404236316680908203125]", 1.0); // round to even
        TEST_DOUBLE("[1.00000000000000011102230246251565404236316680908203124]", 1.0); // previous double
        TEST_DOUBLE("[1.00000000000000011102230246251565404236316680908203126]",
                    1.00000000000000022); // next double

        // Numbers from https://github.com/floitsch/double-conversion/blob/master/test/cctest/test-strtod.cc

        TEST_DOUBLE("[72057594037927928.0]", 72057594037927928.0);
        TEST_DOUBLE("[72057594037927936.0]", 72057594037927936.0);
        TEST_DOUBLE("[72057594037927932.0]", 72057594037927936.0);
        TEST_DOUBLE("[7205759403792793199999e-5]", 72057594037927928.0);
        TEST_DOUBLE("[7205759403792793200001e-5]", 72057594037927936.0);

        TEST_DOUBLE("[9223372036854774784.0]", 9223372036854774784.0);
        TEST_DOUBLE("[9223372036854775808.0]", 9223372036854775808.0);
        TEST_DOUBLE("[9223372036854775296.0]", 9223372036854775808.0);
        TEST_DOUBLE("[922337203685477529599999e-5]", 9223372036854774784.0);
        TEST_DOUBLE("[922337203685477529600001e-5]", 9223372036854775808.0);

        TEST_DOUBLE("[10141204801825834086073718800384]", 10141204801825834086073718800384.0);
        TEST_DOUBLE("[10141204801825835211973625643008]", 10141204801825835211973625643008.0);
        TEST_DOUBLE("[10141204801825834649023672221696]", 10141204801825835211973625643008.0);
        TEST_DOUBLE("[1014120480182583464902367222169599999e-5]", 10141204801825834086073718800384.0);
        TEST_DOUBLE("[1014120480182583464902367222169600001e-5]", 10141204801825835211973625643008.0);

        TEST_DOUBLE("[5708990770823838890407843763683279797179383808]",
                    5708990770823838890407843763683279797179383808.0);
        TEST_DOUBLE("[5708990770823839524233143877797980545530986496]",
                    5708990770823839524233143877797980545530986496.0);
        TEST_DOUBLE("[5708990770823839207320493820740630171355185152]",
                    5708990770823839524233143877797980545530986496.0);
        TEST_DOUBLE("[5708990770823839207320493820740630171355185151999e-3]",
                    5708990770823838890407843763683279797179383808.0);
        TEST_DOUBLE("[5708990770823839207320493820740630171355185152001e-3]",
                    5708990770823839524233143877797980545530986496.0);

        {
            char n1e308[312];   // '1' followed by 308 '0'
            n1e308[0] = '[';
            n1e308[1] = '1';
            for (int j = 2; j < 310; j++)
            {
                n1e308[j] = '0';
            }
            n1e308[310] = ']';
            n1e308[311] = '\0';
            TEST_DOUBLE(n1e308, 1E308);
        }

        // Cover trimming
        TEST_DOUBLE(
            "[2.22507385850720113605740979670913197593481954635164564802342610972482222202107694551652952390813508"
            "7914149158913039621106870086438694594645527657207407820621743379988141063267329253552286881372149012"
            "9811224514518898490572223072852551331557550159143974763979834118019993239625482890171070818506906306"
            "6665599493827577257201576306269066333264756530000924588831643303777979186961204949739037782970490505"
            "1080609940730262937128958950003583799967207254304360284078895771796150945516748243471030702609144621"
            "5722898802581825451803257070188608721131280795122334262883686223215037756666225039825343359745688844"
            "2390026549819838548794829220689472168983109969836584681402285424333066033985088644580400103493397042"
            "7567186443383770486037861622771738545623065874679014086723327636718751234567890123456789012345678901"
            "e-308]",
            2.2250738585072014e-308);
    }

    SECTION("strings")
    {
        auto TEST_STRING = [](const std::string & json_string, const std::string & expected)
        {
            CAPTURE(json_string)
            CAPTURE(expected)
            CHECK(json::parse(json_string)[0].get<std::string>() == expected);
        };

        TEST_STRING("[\"\"]", "");
        TEST_STRING("[\"Hello\"]", "Hello");
        TEST_STRING("[\"Hello\\nWorld\"]", "Hello\nWorld");
        //TEST_STRING("[\"Hello\\u0000World\"]", "Hello\0World");
        TEST_STRING("[\"\\\"\\\\/\\b\\f\\n\\r\\t\"]", "\"\\/\b\f\n\r\t");
        TEST_STRING("[\"\\u0024\"]", "\x24");         // Dollar sign U+0024
        TEST_STRING("[\"\\u00A2\"]", "\xC2\xA2");     // Cents sign U+00A2
        TEST_STRING("[\"\\u20AC\"]", "\xE2\x82\xAC"); // Euro sign U+20AC
        TEST_STRING("[\"\\uD834\\uDD1E\"]", "\xF0\x9D\x84\x9E");  // G clef sign U+1D11E
    }

    SECTION("roundtrip")
    {
        // test cases are from https://github.com/miloyip/nativejson-benchmark/tree/master/test/data/roundtrip

        for (auto filename :
                {
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip01.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip02.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip03.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip04.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip05.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip06.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip07.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip08.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip09.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip10.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip11.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip12.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip13.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip14.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip15.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip16.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip17.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip18.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip19.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip20.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip21.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip22.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip23.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip24.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip25.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip26.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip27.nlohmann_json",
                    //TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip28.nlohmann_json", // incompatible with roundtrip24
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip29.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip30.nlohmann_json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip31.nlohmann_json"
                    //TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip32.nlohmann_json" // same as roundtrip31
                })
        {
            CAPTURE(filename)
            std::ifstream f(filename);
            std::string json_string( (std::istreambuf_iterator<char>(f) ),
                                     (std::istreambuf_iterator<char>()) );

            CAPTURE(json_string)
            json j = json::parse(json_string);
            CHECK(j.dump() == json_string);
        }
    }
}

TEST_CASE("test suite from nlohmann_json-test-suite")
{
    SECTION("read all sample.nlohmann_json")
    {
        // read a file with all unicode characters stored as single-character
        // strings in a JSON array
        std::ifstream f(TEST_DATA_DIRECTORY "/json_testsuite/sample.nlohmann_json");
        json j;
        CHECK_NOTHROW(f >> j);

        // the array has 3 elements
        CHECK(j.size() == 3);
    }
}

TEST_CASE("nlohmann_json.org examples")
{
    // here, we list all JSON values from https://json.org/example

    SECTION("1.nlohmann_json")
    {
        std::ifstream f(TEST_DATA_DIRECTORY "/nlohmann_json.org/1.nlohmann_json");
        json j;
        CHECK_NOTHROW(f >> j);
    }

    SECTION("2.nlohmann_json")
    {
        std::ifstream f(TEST_DATA_DIRECTORY "/nlohmann_json.org/2.nlohmann_json");
        json j;
        CHECK_NOTHROW(f >> j);
    }

    SECTION("3.nlohmann_json")
    {
        std::ifstream f(TEST_DATA_DIRECTORY "/nlohmann_json.org/3.nlohmann_json");
        json j;
        CHECK_NOTHROW(f >> j);
    }

    SECTION("4.nlohmann_json")
    {
        std::ifstream f(TEST_DATA_DIRECTORY "/nlohmann_json.org/4.nlohmann_json");
        json j;
        CHECK_NOTHROW(f >> j);
    }

    SECTION("5.nlohmann_json")
    {
        std::ifstream f(TEST_DATA_DIRECTORY "/nlohmann_json.org/5.nlohmann_json");
        json j;
        CHECK_NOTHROW(f >> j);
    }
    SECTION("FILE 1.nlohmann_json")
    {
        std::unique_ptr<std::FILE, decltype(&std::fclose)> f(std::fopen(TEST_DATA_DIRECTORY "/nlohmann_json.org/1.nlohmann_json", "r"), &std::fclose);
        json _;
        CHECK_NOTHROW(_ = json::parse(f.get()));
    }

    SECTION("FILE 2.nlohmann_json")
    {
        std::unique_ptr<std::FILE, decltype(&std::fclose)> f(std::fopen(TEST_DATA_DIRECTORY "/nlohmann_json.org/2.nlohmann_json", "r"), &std::fclose);
        json _;
        CHECK_NOTHROW(_ = json::parse(f.get()));
    }

    SECTION("FILE 3.nlohmann_json")
    {
        std::unique_ptr<std::FILE, decltype(&std::fclose)> f(std::fopen(TEST_DATA_DIRECTORY "/nlohmann_json.org/3.nlohmann_json", "r"), &std::fclose);
        json _;
        CHECK_NOTHROW(_ = json::parse(f.get()));
    }

    SECTION("FILE 4.nlohmann_json")
    {
        std::unique_ptr<std::FILE, decltype(&std::fclose)> f(std::fopen(TEST_DATA_DIRECTORY "/nlohmann_json.org/4.nlohmann_json", "r"), &std::fclose);
        json _;
        CHECK_NOTHROW(_ = json::parse(f.get()));
    }

    SECTION("FILE 5.nlohmann_json")
    {
        std::unique_ptr<std::FILE, decltype(&std::fclose)> f(std::fopen(TEST_DATA_DIRECTORY "/nlohmann_json.org/5.nlohmann_json", "r"), &std::fclose);
        json _;
        CHECK_NOTHROW(_ = json::parse(f.get()));
    }
}

TEST_CASE("RFC 7159 examples")
{
    // here, we list all JSON values from the RFC 7159 document

    SECTION("7. Strings")
    {
        CHECK(json::parse("\"\\u005C\"") == json("\\"));
        CHECK(json::parse("\"\\uD834\\uDD1E\"") == json("𝄞"));
        CHECK(json::parse("\"𝄞\"") == json("𝄞"));
    }

    SECTION("8.3 String Comparison")
    {
        CHECK(json::parse("\"a\\b\"") == json::parse("\"a\u005Cb\""));
    }

    SECTION("13 Examples")
    {
        {
            auto json_contents = R"(
            {
                 "Image": {
                     "Width":  800,
                     "Height": 600,
                     "Title":  "View from 15th Floor",
                     "Thumbnail": {
                         "Url":    "http://www.example.com/image/481989943",
                         "Height": 125,
                         "Width":  100
                     },
                     "Animated" : false,
                     "IDs": [116, 943, 234, 38793]
                   }
               }
            )";

            CHECK_NOTHROW(json(json_contents));
        }

        {
            auto json_contents = R"(
                [
                    {
                       "precision": "zip",
                       "Latitude":  37.7668,
                       "Longitude": -122.3959,
                       "Address":   "",
                       "City":      "SAN FRANCISCO",
                       "State":     "CA",
                       "Zip":       "94107",
                       "Country":   "US"
                    },
                    {
                       "precision": "zip",
                       "Latitude":  37.371991,
                       "Longitude": -122.026020,
                       "Address":   "",
                       "City":      "SUNNYVALE",
                       "State":     "CA",
                       "Zip":       "94085",
                       "Country":   "US"
                    }
            ])";
            CHECK_NOTHROW(json(json_contents));
        }

        CHECK(json::parse("\"Hello world!\"") == json("Hello world!"));
        CHECK(json::parse("42") == json(42));
        CHECK(json::parse("true") == json(true));
    }
}

TEST_CASE("nst's JSONTestSuite")
{
    SECTION("test_parsing")
    {
        SECTION("y")
        {
            for (auto filename :
                    {
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_array_arraysWithSpaces.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_array_empty-string.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_array_empty.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_array_ending_with_newline.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_array_false.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_array_heterogeneous.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_array_null.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_array_with_1_and_newline.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_array_with_leading_space.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_array_with_several_null.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_array_with_trailing_space.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_number.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_number_0e+1.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_number_0e1.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_number_after_space.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_number_double_close_to_zero.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_number_double_huge_neg_exp.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_number_int_with_exp.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_number_minus_zero.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_number_negative_int.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_number_negative_one.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_number_negative_zero.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_number_real_capital_e.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_number_real_capital_e_neg_exp.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_number_real_capital_e_pos_exp.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_number_real_exponent.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_number_real_fraction_exponent.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_number_real_neg_exp.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_number_real_pos_exponent.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_number_real_underflow.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_number_simple_int.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_number_simple_real.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_number_too_big_neg_int.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_number_too_big_pos_int.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_number_very_big_negative_int.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_object.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_object_basic.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_object_duplicated_key.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_object_duplicated_key_and_value.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_object_empty.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_object_empty_key.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_object_escaped_null_in_key.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_object_extreme_numbers.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_object_long_strings.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_object_simple.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_object_string_unicode.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_object_with_newlines.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_1_2_3_bytes_UTF-8_sequences.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_UTF-16_Surrogates_U+1D11E_MUSICAL_SYMBOL_G_CLEF.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_accepted_surrogate_pair.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_accepted_surrogate_pairs.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_allowed_escapes.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_backslash_and_u_escaped_zero.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_backslash_doublequotes.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_comments.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_double_escape_a.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_double_escape_n.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_escaped_control_character.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_escaped_noncharacter.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_in_array.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_in_array_with_leading_space.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_last_surrogates_1_and_2.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_newline_uescaped.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_nonCharacterInUTF-8_U+10FFFF.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_nonCharacterInUTF-8_U+1FFFF.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_nonCharacterInUTF-8_U+FFFF.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_null_escape.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_one-byte-utf-8.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_pi.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_simple_ascii.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_space.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_three-byte-utf-8.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_two-byte-utf-8.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_u+2028_line_sep.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_u+2029_par_sep.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_uEscape.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_unescaped_char_delete.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_unicode.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_unicodeEscapedBackslash.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_unicode_2.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_unicode_U+200B_ZERO_WIDTH_SPACE.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_unicode_U+2064_invisible_plus.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_unicode_escaped_double_quote.nlohmann_json",
                        // TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_utf16.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_utf8.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_string_with_del_character.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_structure_lonely_false.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_structure_lonely_int.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_structure_lonely_negative_real.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_structure_lonely_null.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_structure_lonely_string.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_structure_lonely_true.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_structure_string_empty.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_structure_trailing_newline.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_structure_true_in_array.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_structure_whitespace_array.nlohmann_json"
                    }
                )
            {
                CAPTURE(filename)
                std::ifstream f(filename);
                json j;
                CHECK_NOTHROW(f >> j);
            }
        }

        SECTION("n")
        {
            for (auto filename :
                    {
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_array_1_true_without_comma.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_array_a_invalid_utf8.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_array_colon_instead_of_comma.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_array_comma_after_close.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_array_comma_and_number.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_array_double_comma.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_array_double_extra_comma.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_array_extra_close.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_array_extra_comma.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_array_incomplete.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_array_incomplete_invalid_value.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_array_inner_array_no_comma.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_array_invalid_utf8.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_array_items_separated_by_semicolon.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_array_just_comma.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_array_just_minus.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_array_missing_value.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_array_newlines_unclosed.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_array_number_and_comma.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_array_number_and_several_commas.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_array_spaces_vertical_tab_formfeed.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_array_star_inside.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_array_unclosed.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_array_unclosed_trailing_comma.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_array_unclosed_with_new_lines.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_array_unclosed_with_object_inside.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_incomplete_false.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_incomplete_null.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_incomplete_true.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_++.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_+1.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_+Inf.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_-01.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_-1.0..nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_-2..nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_-NaN.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_.-1.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_.2e-3.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_0.1.2.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_0.3e+.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_0.3e.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_0.e1.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_0_capital_E+.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_0_capital_E.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_0e+.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_0e.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_1.0e+.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_1.0e-.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_1.0e.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_1_000.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_1eE2.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_2.e+3.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_2.e-3.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_2.e3.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_9.e+.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_Inf.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_NaN.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_U+FF11_fullwidth_digit_one.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_expression.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_hex_1_digit.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_hex_2_digits.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_infinity.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_invalid+-.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_invalid-negative-real.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_invalid-utf-8-in-bigger-int.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_invalid-utf-8-in-exponent.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_invalid-utf-8-in-int.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_minus_infinity.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_minus_sign_with_trailing_garbage.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_minus_space_1.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_neg_int_starting_with_zero.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_neg_real_without_int_part.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_neg_with_garbage_at_end.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_real_garbage_after_e.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_real_with_invalid_utf8_after_e.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_real_without_fractional_part.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_starting_with_dot.nlohmann_json",
                        //TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_then_00.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_with_alpha.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_with_alpha_char.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_number_with_leading_zero.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_object_bad_value.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_object_bracket_key.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_object_comma_instead_of_colon.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_object_double_colon.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_object_emoji.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_object_garbage_at_end.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_object_key_with_single_quotes.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_object_missing_colon.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_object_missing_key.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_object_missing_semicolon.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_object_missing_value.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_object_no-colon.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_object_non_string_key.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_object_non_string_key_but_huge_number_instead.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_object_pi_in_key_and_trailing_comma.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_object_repeated_null_null.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_object_several_trailing_commas.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_object_single_quote.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_object_trailing_comma.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_object_trailing_comment.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_object_trailing_comment_open.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_object_trailing_comment_slash_open.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_object_trailing_comment_slash_open_incomplete.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_object_two_commas_in_a_row.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_object_unquoted_key.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_object_unterminated-value.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_object_with_single_string.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_object_with_trailing_garbage.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_single_space.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_string_1_surrogate_then_escape u.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_string_1_surrogate_then_escape u1.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_string_1_surrogate_then_escape u1x.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_string_1_surrogate_then_escape.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_string_UTF-16_incomplete_surrogate.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_string_UTF8_surrogate_U+D800.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_string_accentuated_char_no_quotes.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_string_backslash_00.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_string_escape_x.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_string_escaped_backslash_bad.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_string_escaped_ctrl_char_tab.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_string_escaped_emoji.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_string_incomplete_escape.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_string_incomplete_escaped_character.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_string_incomplete_surrogate_escape_invalid.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_string_invalid-utf-8-in-escape.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_string_invalid_backslash_esc.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_string_invalid_unicode_escape.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_string_invalid_utf-8.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_string_invalid_utf8_after_escape.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_string_iso_latin_1.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_string_leading_uescaped_thinspace.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_string_lone_utf8_continuation_byte.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_string_no_quotes_with_bad_escape.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_string_overlong_sequence_2_bytes.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_string_overlong_sequence_6_bytes.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_string_overlong_sequence_6_bytes_null.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_string_single_doublequote.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_string_single_quote.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_string_single_string_no_double_quotes.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_string_start_escape_unclosed.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_string_unescaped_crtl_char.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_string_unescaped_newline.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_string_unescaped_tab.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_string_unicode_CapitalU.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_string_with_trailing_garbage.nlohmann_json",
                        //!TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_100000_opening_arrays.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_3C.3E.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_3Cnull3E.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_U+2060_word_joined.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_UTF8_BOM_no_data.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_array_trailing_garbage.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_array_with_extra_array_close.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_array_with_unclosed_string.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_ascii-unicode-identifier.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_capitalized_True.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_close_unopened_array.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_comma_instead_of_closing_brace.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_double_array.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_end_array.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_incomplete_UTF8_BOM.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_lone-invalid-utf-8.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_lone-open-bracket.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_no_data.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_null-byte-outside-string.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_number_with_trailing_garbage.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_object_followed_by_closing_object.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_object_unclosed_no_value.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_object_with_comment.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_object_with_trailing_garbage.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_open_array_apostrophe.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_open_array_comma.nlohmann_json",
                        //!TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_open_array_object.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_open_array_open_object.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_open_array_open_string.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_open_array_string.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_open_object.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_open_object_close_array.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_open_object_comma.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_open_object_open_array.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_open_object_open_string.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_open_object_string_with_apostrophes.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_open_open.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_single_point.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_single_star.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_trailing_#.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_uescaped_LF_before_string.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_unclosed_array.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_unclosed_array_partial_null.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_unclosed_array_unfinished_false.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_unclosed_array_unfinished_true.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_unclosed_object.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_unicode-identifier.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_whitespace_U+2060_word_joiner.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_whitespace_formfeed.nlohmann_json"
                    }
                )
            {
                CAPTURE(filename)
                std::ifstream f(filename);
                json _;
                CHECK_THROWS_AS(_ = json::parse(f), json::parse_error&);
            }
        }

        SECTION("n -> y (relaxed)")
        {
            // these tests fail above, because the parser does not end on EOF;
            // they succeed when the operator>> is used, because it does not
            // have this constraint
            for (auto filename :
                    {
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_array_comma_after_close.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_array_extra_close.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_object_trailing_comment.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_object_trailing_comment_open.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_object_trailing_comment_slash_open.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_object_trailing_comment_slash_open_incomplete.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_object_with_trailing_garbage.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_string_with_trailing_garbage.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_array_trailing_garbage.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_array_with_extra_array_close.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_close_unopened_array.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_double_array.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_number_with_trailing_garbage.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_object_followed_by_closing_object.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_object_with_trailing_garbage.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/n_structure_trailing_#.nlohmann_json"
                    }
                )
            {
                CAPTURE(filename)
                std::ifstream f(filename);
                json j;
                CHECK_NOTHROW(f >> j);
            }
        }

        SECTION("i -> y")
        {
            for (auto filename :
                    {
                        // we do not pose a limit on nesting
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/i_structure_500_nested_arrays.nlohmann_json",
                        // we silently ignore BOMs
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/i_structure_UTF-8_BOM_empty_object.nlohmann_json",
                        // we accept and forward non-characters
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/i_string_unicode_U+10FFFE_nonchar.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/i_string_unicode_U+1FFFE_nonchar.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/i_string_unicode_U+FDD0_nonchar.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/i_string_unicode_U+FFFE_nonchar.nlohmann_json"
                    }
                )
            {
                CAPTURE(filename)
                std::ifstream f(filename);
                json j;
                CHECK_NOTHROW(f >> j);
            }
        }

        // numbers that overflow during parsing
        SECTION("i/y -> n (out of range)")
        {
            for (auto filename :
                    {
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/i_number_neg_int_huge_exp.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/i_number_pos_double_huge_exp.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_number_huge_exp.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_number_real_neg_overflow.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/y_number_real_pos_overflow.nlohmann_json"
                    }
                )
            {
                CAPTURE(filename)
                std::ifstream f(filename);
                json j;
                CHECK_THROWS_AS(f >> j, json::out_of_range&);
            }
        }

        SECTION("i -> n")
        {
            for (auto filename :
                    {
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/i_object_key_lone_2nd_surrogate.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/i_string_1st_surrogate_but_2nd_missing.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/i_string_1st_valid_surrogate_2nd_invalid.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/i_string_UTF-16_invalid_lonely_surrogate.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/i_string_UTF-16_invalid_surrogate.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/i_string_UTF-8_invalid_sequence.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/i_string_incomplete_surrogate_and_escape_valid.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/i_string_incomplete_surrogate_pair.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/i_string_incomplete_surrogates_escape_valid.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/i_string_inverted_surrogates_U+1D11E.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/i_string_lone_second_surrogate.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/i_string_not_in_unicode_range.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite/test_parsing/i_string_truncated-utf-8.nlohmann_json"
                    }
                )
            {
                CAPTURE(filename)
                std::ifstream f(filename);
                json j;
                CHECK_THROWS_AS(f >> j, json::parse_error&);
            }
        }
    }
}

TEST_CASE("nst's JSONTestSuite (2)")
{
    SECTION("test_parsing")
    {
        SECTION("y")
        {
            for (auto filename :
                    {
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_array_arraysWithSpaces.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_array_empty-string.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_array_empty.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_array_ending_with_newline.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_array_false.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_array_heterogeneous.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_array_null.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_array_with_1_and_newline.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_array_with_leading_space.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_array_with_several_null.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_array_with_trailing_space.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_number.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_number_0e+1.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_number_0e1.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_number_after_space.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_number_double_close_to_zero.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_number_int_with_exp.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_number_minus_zero.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_number_negative_int.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_number_negative_one.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_number_negative_zero.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_number_real_capital_e.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_number_real_capital_e_neg_exp.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_number_real_capital_e_pos_exp.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_number_real_exponent.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_number_real_fraction_exponent.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_number_real_neg_exp.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_number_real_pos_exponent.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_number_simple_int.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_number_simple_real.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_object.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_object_basic.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_object_duplicated_key.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_object_duplicated_key_and_value.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_object_empty.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_object_empty_key.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_object_escaped_null_in_key.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_object_extreme_numbers.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_object_long_strings.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_object_simple.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_object_string_unicode.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_object_with_newlines.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_1_2_3_bytes_UTF-8_sequences.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_accepted_surrogate_pair.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_accepted_surrogate_pairs.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_allowed_escapes.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_backslash_and_u_escaped_zero.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_backslash_doublequotes.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_comments.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_double_escape_a.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_double_escape_n.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_escaped_control_character.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_escaped_noncharacter.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_in_array.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_in_array_with_leading_space.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_last_surrogates_1_and_2.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_nbsp_uescaped.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_nonCharacterInUTF-8_U+10FFFF.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_nonCharacterInUTF-8_U+FFFF.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_null_escape.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_one-byte-utf-8.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_pi.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_reservedCharacterInUTF-8_U+1BFFF.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_simple_ascii.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_space.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_surrogates_U+1D11E_MUSICAL_SYMBOL_G_CLEF.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_three-byte-utf-8.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_two-byte-utf-8.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_u+2028_line_sep.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_u+2029_par_sep.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_uEscape.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_uescaped_newline.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_unescaped_char_delete.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_unicode.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_unicodeEscapedBackslash.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_unicode_2.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_unicode_U+10FFFE_nonchar.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_unicode_U+1FFFE_nonchar.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_unicode_U+200B_ZERO_WIDTH_SPACE.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_unicode_U+2064_invisible_plus.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_unicode_U+FDD0_nonchar.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_unicode_U+FFFE_nonchar.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_unicode_escaped_double_quote.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_utf8.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_string_with_del_character.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_structure_lonely_false.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_structure_lonely_int.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_structure_lonely_negative_real.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_structure_lonely_null.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_structure_lonely_string.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_structure_lonely_true.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_structure_string_empty.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_structure_trailing_newline.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_structure_true_in_array.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/y_structure_whitespace_array.nlohmann_json"
                    }
                )
            {
                CAPTURE(filename)
                std::ifstream f(filename);
                json _;
                CHECK_NOTHROW(_ = json::parse(f));
                std::ifstream f2(filename);
                CHECK(json::accept(f2));
            }
        }

        SECTION("n")
        {
            for (auto filename :
                    {
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_array_1_true_without_comma.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_array_a_invalid_utf8.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_array_colon_instead_of_comma.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_array_comma_after_close.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_array_comma_and_number.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_array_double_comma.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_array_double_extra_comma.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_array_extra_close.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_array_extra_comma.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_array_incomplete.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_array_incomplete_invalid_value.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_array_inner_array_no_comma.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_array_invalid_utf8.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_array_items_separated_by_semicolon.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_array_just_comma.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_array_just_minus.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_array_missing_value.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_array_newlines_unclosed.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_array_number_and_comma.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_array_number_and_several_commas.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_array_spaces_vertical_tab_formfeed.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_array_star_inside.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_array_unclosed.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_array_unclosed_trailing_comma.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_array_unclosed_with_new_lines.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_array_unclosed_with_object_inside.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_incomplete_false.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_incomplete_null.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_incomplete_true.nlohmann_json",
                        //TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_multidigit_number_then_00.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_++.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_+1.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_+Inf.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_-01.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_-1.0..nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_-2..nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_-NaN.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_.-1.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_.2e-3.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_0.1.2.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_0.3e+.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_0.3e.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_0.e1.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_0_capital_E+.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_0_capital_E.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_0e+.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_0e.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_1.0e+.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_1.0e-.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_1.0e.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_1_000.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_1eE2.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_2.e+3.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_2.e-3.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_2.e3.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_9.e+.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_Inf.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_NaN.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_U+FF11_fullwidth_digit_one.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_expression.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_hex_1_digit.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_hex_2_digits.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_infinity.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_invalid+-.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_invalid-negative-real.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_invalid-utf-8-in-bigger-int.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_invalid-utf-8-in-exponent.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_invalid-utf-8-in-int.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_minus_infinity.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_minus_sign_with_trailing_garbage.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_minus_space_1.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_neg_int_starting_with_zero.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_neg_real_without_int_part.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_neg_with_garbage_at_end.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_real_garbage_after_e.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_real_with_invalid_utf8_after_e.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_real_without_fractional_part.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_starting_with_dot.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_with_alpha.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_with_alpha_char.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_number_with_leading_zero.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_object_bad_value.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_object_bracket_key.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_object_comma_instead_of_colon.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_object_double_colon.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_object_emoji.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_object_garbage_at_end.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_object_key_with_single_quotes.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_object_lone_continuation_byte_in_key_and_trailing_comma.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_object_missing_colon.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_object_missing_key.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_object_missing_semicolon.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_object_missing_value.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_object_no-colon.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_object_non_string_key.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_object_non_string_key_but_huge_number_instead.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_object_repeated_null_null.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_object_several_trailing_commas.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_object_single_quote.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_object_trailing_comma.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_object_trailing_comment.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_object_trailing_comment_open.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_object_trailing_comment_slash_open.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_object_trailing_comment_slash_open_incomplete.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_object_two_commas_in_a_row.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_object_unquoted_key.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_object_unterminated-value.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_object_with_single_string.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_object_with_trailing_garbage.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_single_space.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_string_1_surrogate_then_escape.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_string_1_surrogate_then_escape_u.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_string_1_surrogate_then_escape_u1.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_string_1_surrogate_then_escape_u1x.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_string_accentuated_char_no_quotes.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_string_backslash_00.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_string_escape_x.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_string_escaped_backslash_bad.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_string_escaped_ctrl_char_tab.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_string_escaped_emoji.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_string_incomplete_escape.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_string_incomplete_escaped_character.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_string_incomplete_surrogate.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_string_incomplete_surrogate_escape_invalid.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_string_invalid-utf-8-in-escape.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_string_invalid_backslash_esc.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_string_invalid_unicode_escape.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_string_invalid_utf8_after_escape.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_string_leading_uescaped_thinspace.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_string_no_quotes_with_bad_escape.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_string_single_doublequote.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_string_single_quote.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_string_single_string_no_double_quotes.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_string_start_escape_unclosed.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_string_unescaped_crtl_char.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_string_unescaped_newline.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_string_unescaped_tab.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_string_unicode_CapitalU.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_string_with_trailing_garbage.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_U+2060_word_joined.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_UTF8_BOM_no_data.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_angle_bracket_..nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_angle_bracket_null.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_array_trailing_garbage.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_array_with_extra_array_close.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_array_with_unclosed_string.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_ascii-unicode-identifier.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_capitalized_True.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_close_unopened_array.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_comma_instead_of_closing_brace.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_double_array.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_end_array.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_incomplete_UTF8_BOM.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_lone-invalid-utf-8.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_lone-open-bracket.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_no_data.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_null-byte-outside-string.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_number_with_trailing_garbage.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_object_followed_by_closing_object.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_object_unclosed_no_value.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_object_with_comment.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_object_with_trailing_garbage.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_open_array_apostrophe.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_open_array_comma.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_open_array_open_object.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_open_array_open_string.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_open_array_string.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_open_object.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_open_object_close_array.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_open_object_comma.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_open_object_open_array.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_open_object_open_string.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_open_object_string_with_apostrophes.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_open_open.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_single_eacute.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_single_star.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_trailing_#.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_uescaped_LF_before_string.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_unclosed_array.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_unclosed_array_partial_null.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_unclosed_array_unfinished_false.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_unclosed_array_unfinished_true.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_unclosed_object.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_unicode-identifier.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_whitespace_U+2060_word_joiner.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_whitespace_formfeed.nlohmann_json"
                    }
                )
            {
                CAPTURE(filename)
                std::ifstream f(filename);
                json _;
                CHECK_THROWS_AS(_ = json::parse(f), json::parse_error&);
                std::ifstream f2(filename);
                CHECK(!json::accept(f2));
            }
        }

        SECTION("n (previously overflowed)")
        {
            for (auto filename :
                    {
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_100000_opening_arrays.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/n_structure_open_array_object.nlohmann_json"
                    }
                )
            {
                CAPTURE(filename)
                std::ifstream f(filename);
                CHECK(!json::accept(f));
            }
        }

        SECTION("i -> y")
        {
            for (auto filename :
                    {
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_number_double_huge_neg_exp.nlohmann_json",
                        //TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_number_huge_exp.nlohmann_json",
                        //TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_number_neg_int_huge_exp.nlohmann_json",
                        //TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_number_pos_double_huge_exp.nlohmann_json",
                        //TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_number_real_neg_overflow.nlohmann_json",
                        //TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_number_real_pos_overflow.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_number_real_underflow.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_number_too_big_neg_int.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_number_too_big_pos_int.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_number_very_big_negative_int.nlohmann_json",
                        //TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_object_key_lone_2nd_surrogate.nlohmann_json",
                        //TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_1st_surrogate_but_2nd_missing.nlohmann_json",
                        //TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_1st_valid_surrogate_2nd_invalid.nlohmann_json",
                        //TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_UTF-16LE_with_BOM.nlohmann_json",
                        //TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_UTF-8_invalid_sequence.nlohmann_json",
                        //TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_UTF8_surrogate_U+D800.nlohmann_json",
                        //TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_incomplete_surrogate_and_escape_valid.nlohmann_json",
                        //TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_incomplete_surrogate_pair.nlohmann_json",
                        //TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_incomplete_surrogates_escape_valid.nlohmann_json",
                        //TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_invalid_lonely_surrogate.nlohmann_json",
                        //TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_invalid_surrogate.nlohmann_json",
                        //TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_invalid_utf-8.nlohmann_json",
                        //TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_inverted_surrogates_U+1D11E.nlohmann_json",
                        //TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_iso_latin_1.nlohmann_json",
                        //TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_lone_second_surrogate.nlohmann_json",
                        //TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_lone_utf8_continuation_byte.nlohmann_json",
                        //TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_not_in_unicode_range.nlohmann_json",
                        //TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_overlong_sequence_2_bytes.nlohmann_json",
                        //TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_overlong_sequence_6_bytes.nlohmann_json",
                        //TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_overlong_sequence_6_bytes_null.nlohmann_json",
                        //TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_truncated-utf-8.nlohmann_json",
                        //TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_utf16BE_no_BOM.nlohmann_json",
                        //TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_utf16LE_no_BOM.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_structure_500_nested_arrays.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_structure_UTF-8_BOM_empty_object.nlohmann_json"
                    }
                )
            {
                CAPTURE(filename)
                std::ifstream f(filename);
                json _;
                CHECK_NOTHROW(_ = json::parse(f));
                std::ifstream f2(filename);
                CHECK(json::accept(f2));
            }
        }

        SECTION("i -> n")
        {
            for (auto filename :
                    {
                        //TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_number_double_huge_neg_exp.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_number_huge_exp.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_number_neg_int_huge_exp.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_number_pos_double_huge_exp.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_number_real_neg_overflow.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_number_real_pos_overflow.nlohmann_json",
                        //TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_number_real_underflow.nlohmann_json",
                        //TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_number_too_big_neg_int.nlohmann_json",
                        //TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_number_too_big_pos_int.nlohmann_json",
                        //TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_number_very_big_negative_int.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_object_key_lone_2nd_surrogate.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_1st_surrogate_but_2nd_missing.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_1st_valid_surrogate_2nd_invalid.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_UTF-16LE_with_BOM.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_UTF-8_invalid_sequence.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_UTF8_surrogate_U+D800.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_incomplete_surrogate_and_escape_valid.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_incomplete_surrogate_pair.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_incomplete_surrogates_escape_valid.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_invalid_lonely_surrogate.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_invalid_surrogate.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_invalid_utf-8.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_inverted_surrogates_U+1D11E.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_iso_latin_1.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_lone_second_surrogate.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_lone_utf8_continuation_byte.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_not_in_unicode_range.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_overlong_sequence_2_bytes.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_overlong_sequence_6_bytes.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_overlong_sequence_6_bytes_null.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_truncated-utf-8.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_utf16BE_no_BOM.nlohmann_json",
                        TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_string_utf16LE_no_BOM.nlohmann_json",
                        //TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_structure_500_nested_arrays.nlohmann_json",
                        //TEST_DATA_DIRECTORY "/nst_json_testsuite2/test_parsing/i_structure_UTF-8_BOM_empty_object.nlohmann_json"
                    }
                )
            {
                CAPTURE(filename)
                std::ifstream f(filename);
                json _;
                CHECK_THROWS_AS(_ = json::parse(f), json::exception&); // could be parse_error or out_of_range
                std::ifstream f2(filename);
                CHECK(!json::accept(f2));
            }
        }
    }
}

namespace
{
std::string trim(const std::string& str);

// from https://stackoverflow.com/a/25829178/266378
std::string trim(const std::string& str)
{
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first)
    {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}
}

TEST_CASE("Big List of Naughty Strings")
{
    // test from https://github.com/minimaxir/big-list-of-naughty-strings
    SECTION("parsing blns.nlohmann_json")
    {
        std::ifstream f(TEST_DATA_DIRECTORY "/big-list-of-naughty-strings/blns.nlohmann_json");
        json j;
        CHECK_NOTHROW(f >> j);
    }

    // check if parsed strings roundtrip
    // https://www.reddit.com/r/cpp/comments/5qpbie/json_form_modern_c_version_210/dd12mpq/
    SECTION("roundtripping")
    {
        std::ifstream f(TEST_DATA_DIRECTORY "/big-list-of-naughty-strings/blns.nlohmann_json");
        std::string line;

        // read lines one by one, bail out on error or eof
        while (getline(f, line))
        {
            // trim whitespace
            line = trim(line);

            // remove trailing comma
            line = line.substr(0, line.find_last_of(","));

            // discard lines without at least two characters (quotes)
            if (line.size() < 2)
            {
                continue;
            }

            // check roundtrip
            CAPTURE(line)
            json j = json::parse(line);
            CHECK(j.dump() == line);
        }
    }
}
