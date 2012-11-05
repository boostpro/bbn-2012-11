// Copyright Dave Abrahams 2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/foreach.hpp>
#include <boost/range.hpp>
#include <ostream>

template <class Range>
void print(std::ostream& os, Range const& x)
{
    char const* prefix = "[ ";

    typedef typename
        boost::range_value<Range>::type
        element;
    
    BOOST_FOREACH(element const& e, x)
    {
        os << prefix << e;
        prefix = ", ";
    }
    os << " ]";
}

#include <iostream>
#include <vector>
#include <list>
#include <string>

int main()
{
    std::vector<int> v(10, 33);
    print(std::cout, v);
    std::cout << std::endl;
    
    std::list<std::string> l(2, "hi ho");
    print(std::cout, l);
    std::cout << std::endl;

    char const* words[] = { "now", "is", "the", "time" };
    print(std::cout, words);
    std::cout << std::endl;

    print(std::cout, std::make_pair(words+2, words+4));
    std::cout << std::endl;
}
