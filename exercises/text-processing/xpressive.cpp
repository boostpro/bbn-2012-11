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
    assert($writeme$
           == "The answer is #, but can you give me # good question on #/#/#?");

    // Find and sum all integer numbers
    using namespace boost::lambda;
    int sum = std::accumulate(
		$writeme$
    );

    assert(sum == 2024);

    // Split the text into tokens delimited by any sequence of
    // punctuation characters or whitespace
    $writeme$

    // Print all dates, converting from US format “2/21/2008” to
    // European format “21. 2. 2008”
    $writeme$
        
    // Change all dates from US format “2/21/2008” to European format “21. 2. 2008”
    std::cout << $writeme$ << std::endl;
}

int main()
{
    std::cout << "=== dynamic ===\n\n";
    x::sregex const number = $writeme$
    x::sregex const no_token = $writeme$
    x::sregex const date = $writeme$

    test(number,no_token,date);

    std::cout << "\n=== static ===\n\n";
    using namespace boost::xpressive;
    test(
        $writeme$,
        $writeme$,
        $writeme$
    );
}
        
