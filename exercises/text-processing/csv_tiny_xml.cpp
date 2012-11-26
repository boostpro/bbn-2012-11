// Copyright David Abrahams 2008. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#include "boost/tokenizer.hpp"
#include "boost/token_functions.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/format.hpp"
#include "boost/lexical_cast.hpp"

#include <fstream>
#include <iostream>
#include <vector>
#include <string>

// Use Boost.Tokenizer to parse the input
// Use Boost.StringAlgo to turn column headings into tag names
// Use Boost.Format to write the output
// Bonus: use Boost.LexicalCast to inject an <age> tag with a value computed from birth_year

typedef std::vector<std::string> stringvec;

stringvec
parse_line(std::istream& s)
{
    $writeme$
}

int main(int argc, char* argv[])
{
    std::ifstream in(argv[1]);
    
    stringvec headings = parse_line(in);

    // headings.push_back(std::string("age"));
    
    for (;;)
    {
        stringvec fields = parse_line(in);
        if (fields.size() == 0) break;
        $writeme$
    }
}
