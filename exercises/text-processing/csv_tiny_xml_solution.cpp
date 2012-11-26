// Copyright David Abrahams 2008. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#include "boost/tokenizer.hpp"
#include "boost/token_functions.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/format.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/foreach.hpp"
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
    std::string line;
    if (!std::getline(s, line))
    {
        return stringvec();
    }
    else
    {
        boost::tokenizer<boost::escaped_list_separator<char> >
            tokens(line, boost::escaped_list_separator<char>());
    
        return stringvec(
            tokens.begin(), tokens.end());
    }
}

int main(int argc, char* argv[])
{
    std::ifstream in(argv[1]);
    
    stringvec headings = parse_line(in);

    for (stringvec::iterator p = headings.begin()
                           , e = headings.end();
         p != e; ++p)
    {
        boost::trim(*p);
        boost::to_lower(*p);
        boost::replace_all(*p, " ", "_");
    }

    std::size_t byi = std::find(headings.begin(), headings.end(), "birth_year")
        - headings.begin();

    headings.push_back(std::string("age"));
    
    for (;;)
    {
        stringvec fields = parse_line(in);
        if (fields.size() == 0) break;

        fields.push_back(
            boost::lexical_cast<std::string>(
                2008 - boost::lexical_cast<int>(boost::trim_copy(fields[byi]))));
        
        std::cout << "<record>\n";
        for (unsigned i = 0; i < headings.size() && i < fields.size(); ++i)
        {
            boost::trim(fields[i]);
            std::cout << boost::format("    <%1%>%2%</%1%>\n") % headings[i] % fields[i];
        }
        
        std::cout << "</record>\n";
    }
}
