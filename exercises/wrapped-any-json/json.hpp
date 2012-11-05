// Copyright Dave Abrahams 2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef JSON_DWA2012114_HPP
# define JSON_DWA2012114_HPP

# include <map>
# include <vector>
# include <string>
# include <boost/any.hpp>
# include <boost/utility/enable_if.hpp>
# include <boost/type_traits/is_arithmetic.hpp>
# include <boost/type_traits/is_convertible.hpp>
# include <ostream>

namespace json {

struct value;

typedef 
  std::map<std::string, value>
object;

typedef std::vector<value> array;

typedef std::string         string;
typedef long double         number;
typedef bool                boolean;
typedef struct null {}      null;

struct value
{
    value(object const& x) : stored(x) {}
    value(array const& x) : stored(x) {}
    value(string const& x) : stored(x) {}

    template <class T>
    value(
        T const& x,
        typename boost::enable_if<boost::is_arithmetic<T> >::type* = 0
    ) : stored(number(x)) {}

    /* $$ writeme $$ */
    
    value(boolean const& x) : stored(x) {}
    value(null x = null()) : stored(x) {}

    friend std::ostream& operator<<(std::ostream& s, value const& x);
 private:
    boost::any stored;
};

// JSON is recursive, so forward declarations are de rigeur
std::ostream& operator<<(std::ostream& s, value const& x);

// object::value_type is now associated with this namespace, so
// this operator can be found via ADL.
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

// value is now associated with this namespace, so this operator
// can be found via ADL.
inline std::ostream& operator<<(std::ostream& s, value const& x)
{
    if (string const* y = boost::any_cast<string>(&x.stored))
        s << '"' << *y << '"';
    else if (number const* y = boost::any_cast<number>(&x.stored))
        s << *y;
    else if (object const* y = boost::any_cast<object>(&x.stored))
        print(s, *y, "{ ", " }");
    else if (array const* y = boost::any_cast<array>(&x.stored))
        print(s, *y);
    else if (bool const* y = boost::any_cast<bool>(&x.stored))
        s << (*y ? "true" : "false");
    else if (null const* y = boost::any_cast<null>(&x.stored))
        s << "null";
    return s;
}

}

#endif // DWA2012114_HPP
