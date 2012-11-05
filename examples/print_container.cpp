// Copyright Dave Abrahams 2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <ostream>

template <class Container>
void print(std::ostream& os, Container const& x)
{
    char const* prefix = "[ ";

    typedef typename Container::const_iterator iter;
    for (iter p = x.begin(); p != x.end(); ++p)
    {
        os << prefix << *p;
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
}
