// Copyright Dave Abrahams 2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef JSON_DWA2012114_HPP
# define JSON_DWA2012114_HPP

# include <map>
# include <vector>
# include <string>
# include <boost/any.hpp>
# include <ostream>

namespace json {

typedef boost::any value;
typedef 
  std::map<std::string, value>
object;

typedef std::vector<value> array;

typedef std::string         string;
typedef long double         number;
typedef bool                boolean;
typedef struct null {}      null;

// JSON is recursive, so forward declarations are de rigeur
std::ostream& operator<<(std::ostream& s, value const& x);

// Elements of the map need a streaming operator.  Because neither
// type in the pair has json:: as an associated namespace, it can't be
// found via argument-dependent lookup.  It precedes the print()
// function template below so that it can be found via ordinary lookup.
inline std::ostream& operator<<(
    std::ostream& os, object::value_type const& x)
{
    return os << "\"" << x.first << "\": " << x.second;
}

// Print the elements of container x, bracketed by the open and
// close text, and separated by commas.
template <class Container>
void print(
    std::ostream& os, Container const& x,
    char const* open = "[ ", char const* close = " ]")
{
    char const* prefix = open;
    typedef typename Container::const_iterator iter;
    for (iter p = x.begin(); p != x.end(); ++p)
    {
        os << prefix << *p;
        prefix = ", ";
    }
    os << close;
}

// Nothing associates value with this namespace, so this operator
// won't be found via ADL.  For it to be found, you have to bring it
// into scope with a using-declaration or -directive.
inline std::ostream& operator<<(std::ostream& s, value const& x)
{
    if (string const* y = boost::any_cast<string>(&x))
        s << '"' << *y << '"';
    else if (number const* y = boost::any_cast<number>(&x))
        s << *y;
    else if (object const* y = boost::any_cast<object>(&x))
        print(s, *y, "{ ", " }");
    else if (array const* y = boost::any_cast<array>(&x))
        print(s, *y);
    else if (bool const* y = boost::any_cast<bool>(&x))
        s << (*y ? "true" : "false");
    else if (null const* y = boost::any_cast<null>(&x))
        s << "null";
    return s;
}

}

#endif // DWA2012114_HPP
