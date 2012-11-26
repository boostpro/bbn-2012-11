// Copyright David Abrahams 2008. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#define _GLIBCXX_DEBUG 1
#include "boost/regex.hpp"
#include "boost/lambda/lambda.hpp"
#include "boost/lambda/if.hpp"
#include "boost/lambda/bind.hpp"
#include "boost/lexical_cast.hpp"
#include <string>
#include <cassert>
#include <numeric>
#include <algorithm>
#include <iostream>

// Replace all occurrences of numbers by the character ‘#’
std::string replace_numbers(std::string const& s)
{
    static boost::regex pat("\\d+");
    return boost::regex_replace(s, pat, "#");
}

// Find and sum all integer numbers
int sum_numbers(std::string const& s)
{
    boost::regex number("(\\d+)");

    using namespace boost::lambda;

    return std::accumulate(
        boost::sregex_token_iterator(s.begin(), s.end(), number),
        boost::sregex_token_iterator(),
        0,
        _1 + bind(&boost::lexical_cast<int,std::string>, _2)
    );
}

// Split the text into tokens delimited by any sequence of punctuation characters or whitespace
void print_tokens(std::string const& s)
{
    boost::regex no_token("([?,:;.'\"!&]|\\s)+");
    
    using namespace boost::lambda;
    std::for_each(
        boost::sregex_token_iterator(s.begin(), s.end(), no_token, -1),
        boost::sregex_token_iterator(s.end(), s.end(), no_token, -1),
        std::cout << _1 << '\n'
    );
    
}

boost::regex date("(\\d\\d?)/(\\d\\d?)/(\\d{4})");

// Print all dates, converting from US format “2/21/2008” to European format “21. 2. 2008”
void print_us_dates_as_euro(std::string const& s)
{
    
    using namespace boost::lambda;

    int const matches[] = {1,2,3};
    int i = 0;
    std::for_each(
        boost::sregex_token_iterator(s.begin(), s.end(), date, matches),
        boost::sregex_token_iterator(),
        std::cout << _1 << if_then_else_return(++var(i) % 3, ". ", "\n")
    );
}

// Change all dates from US format “2/21/2008” to European format “21. 2. 2008”
std::string convert_us_dates_to_euro(std::string& s)
{
    return regex_replace(s, date, "\\2. \\1. \\3");
}

int main()
{
    std::string input("The answer is 42, but can you give me 1 good question on 9/8/1964?");
    assert(replace_numbers(input)
           == "The answer is #, but can you give me # good question on #/#/#?");

    assert(sum_numbers(input) == 2024);

    print_tokens(input);
    print_us_dates_as_euro(input);
    std::cout <<  convert_us_dates_to_euro(input) << std::endl;
}
        
