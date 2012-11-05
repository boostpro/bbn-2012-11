// Copyright Dave Abrahams 2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#include <boost/xpressive/xpressive.hpp>
#include <boost/foreach.hpp>
#include <iostream>
#include <fstream>


std::string load_file(char const* file)
{
    std::ifstream is(file);
    std::string s;
    s.erase();
    //
    // attempt to grow string buffer to match file size,
    // this doesn't always work...
    s.reserve(is.rdbuf()->in_avail());
    char c;
    while(is.get(c))
    {
        // use logarithmic growth stategy, in case
        // in_avail (above) returned zero:
        if(s.capacity() == s.size())
            s.reserve(s.capacity() * 3);
        s.append(1, c);
    }
    return s;
}

namespace xpr = boost::xpressive;

typedef xpr::sregex_token_iterator token_iterator;
typedef token_iterator::value_type token;

inline token_iterator tokens(std::string const& s)
{
    static xpr::sregex token_pattern = xpr::sregex::compile(
      "(?:\\s*)("
        // single characters
        "[[\\]{}:,]"
        "|" // null
        "null"
        "|" // bools
        "true|false"
        "|" // strings
        "\"(?:"
              "\\\\" "(?:[\"\\\\/bfnrt]|u[0-9a-fA-F]{4})"
          "|" "[^" "\\\\" "\"" "]"
        ")*\""
        "|" // numbers
        "-?(?:0|[1-9][0-9]*(?:[.][0-9]+)?(?:[eE][+-]?[0-9]+)?)"
      ")");

    return token_iterator(s.begin(), s.end(), token_pattern, 1);
}

#ifndef NO_TEST
int main()
{
    try
    {
        std::string input = load_file("test.json");

        BOOST_FOREACH(token t, std::make_pair(tokens(input),token_iterator()))
            std::cout << t << std::endl;
    }
    catch(std::exception const& e)
    {
        std::cout << e.what() << std::endl << std::flush;
        throw;
    }
}
#endif 
