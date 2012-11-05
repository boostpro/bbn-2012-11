// Copyright Dave Abrahams 2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#include <boost/any.hpp>
#include <string>
#include <iostream>

int main()
{
    using std::string; using namespace boost;

    any s1 = string("pumpkin"), s2 = s1;

    if (string* p = any_cast<string>(&s1)) {
       std::cout << "s1 was " << *p << std::endl;
       p->replace(0, 4, "Rumplestilts");
    }
    std::cout << any_cast<string>(s1) << " ate a "
              << any_cast<string>(s2)
              << std::endl;
}

