// Copyright David Abrahams 2008. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#define _GLIBCXX_DEBUG 1
#include "boost/regex.hpp"
#include <string>
#include <cassert>
#include <numeric>
#include <algorithm>
#include <iostream>

// Replace all occurrences of numbers by the character ‘#’
std::string replace_numbers(std::string const& s)
{
    $writeme$
}

// Find and sum all integer numbers
int sum_numbers(std::string const& s)
{
    $writeme$
}

// Split the text into tokens delimited by any sequence of punctuation characters or whitespace
void print_tokens(std::string const& s)
{
    $writeme$
}

boost::regex date("(\\d\\d?)/(\\d\\d?)/(\\d{4})");

// Print all dates, converting from US format “2/21/2008” to European format “21. 2. 2008”
void print_us_dates_as_euro(std::string const& s)
{
    $writeme$
}

// Change all dates from US format “2/21/2008” to European format “21. 2. 2008”
std::string convert_us_dates_to_euro(std::string& s)
{
    $writeme$
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
        
