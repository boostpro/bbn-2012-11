// Copyright David Abrahams 2008. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#define _GLIBCXX_DEBUG 1
#include "boost/xpressive/xpressive_dynamic.hpp"
#include "boost/xpressive/xpressive_static.hpp"
#include "boost/lambda/lambda.hpp"
#include "boost/lambda/if.hpp"
#include "boost/lambda/bind.hpp"
#include "boost/lexical_cast.hpp"
#include <string>
#include <cassert>
#include <numeric>
#include <algorithm>
#include <iostream>

namespace x = boost::xpressive;

typedef x::sregex_token_iterator sregex_stoken_iterator;

typedef x::sregex regex;

void test(regex number, regex no_token, regex date)
{
    std::string input("The answer is 42, but can you give me 1 good question on 9/8/1964?");
        
    // Replace all occurrences of numbers by the character ‘#’
    assert(x::regex_replace(input, number, std::string("#"))
           == "The answer is #, but can you give me # good question on #/#/#?");

    // Find and sum all integer numbers
    using namespace boost::lambda;
    int sum = std::accumulate(
        sregex_stoken_iterator(input.begin(), input.end(), number),
        sregex_stoken_iterator(input.end(), input.end(), number),
        0,
        _1 + bind(&boost::lexical_cast<int,std::string>, _2)
    );

    assert(sum == 2024);

    // Split the text into tokens delimited by any sequence of
    // punctuation characters or whitespace
    std::for_each(
        sregex_stoken_iterator(input.begin(), input.end(), no_token, -1),
        sregex_stoken_iterator(input.end(), input.end(), no_token, -1),
        std::cout << _1 << '\n'
    );

    // Print all dates, converting from US format “2/21/2008” to
    // European format “21. 2. 2008”
    int const matches[] = {2,1,3};
    int i = 0;
    std::for_each(
        sregex_stoken_iterator(input.begin(), input.end(), date, matches),
        sregex_stoken_iterator(input.end(), input.end(), date, matches),
        std::cout << _1 << if_then_else_return(++var(i) % 3, ". ", "\n")
    );
        
    // Change all dates from US format “2/21/2008” to European format “21. 2. 2008”
    std::cout << x::regex_replace(input, date, std::string("$1. $2. $3")) << std::endl;
}

int main()
{
    std::cout << "=== dynamic ===\n\n";
    x::sregex const number = x::sregex::compile("(\\d+)");
    x::sregex const no_token = x::sregex::compile("[[:punct:]\\s]+");
    x::sregex const date = x::sregex::compile("(\\d\\d?)/(\\d\\d?)/(\\d{4})");

    test(number,no_token,date);

    std::cout << "\n=== static ===\n\n";
    using namespace boost::xpressive;
    test(
        (s1 = +_d),
        +set[punct | _s],
        (s1 = _d >> !_d) >> '/' >> (s2 = _d >> !_d) >> '/' >> (s3 = repeat<4,4>( _d ))
    );
}
        
