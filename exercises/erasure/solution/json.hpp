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

bool logging = false;

#ifndef NDEBUG
# include <iostream>
# define LOG(x) (logging ? std::cout << "{ " << x << " }" << std::endl : std::cout)
#else
# define LOG(x)
#endif

template <class T>
struct displayer
{
    displayer(T const& x) : x(x) {}
    T const& x;
    friend std::ostream& operator<<(std::ostream& os, displayer const& a)
    {
        return os << "<" << a.x << ">" << "(" << typeid(T).name() << "@" << &a.x << ")";
    }
};

template <class T>
displayer<T> display(T const& x)
{
    return displayer<T>(x);
}

namespace json {

struct value;

typedef 
  std::map<std::string, value>
object;
std::ostream& operator<<(std::ostream&, object const&);

typedef std::vector<value> array;
std::ostream& operator<<(std::ostream&, array const&);

typedef std::string         string;
typedef long double         number;
typedef bool                boolean;
typedef struct null {}      null;

std::ostream& operator<<(std::ostream& os, null)
{
    return os << "null";
}

struct storage
{
    virtual ~storage() {}
    virtual storage* clone() const = 0;
    virtual void print(std::ostream& s) const = 0;
};

template <class T>
struct store : storage
{
    store(T const& x)
        : val(x) {}
    
    storage* clone() const { return new store<T>(val); }
    void print(std::ostream& s) const { s << val; }
    
    T val;
};

struct print_any
{
    template <class T>
    print_any(T const& rhs)
        : stored(new store<T>(rhs))
    {}
    
    friend std::ostream& operator<<(std::ostream& s, print_any const& x)
    {
        x.stored->print(s);
        return s;
    }

    print_any(print_any const& rhs)
        : stored(rhs.stored->clone())
    {
        LOG("copied " << *this << " " << this << "<--" << &rhs);
    }

    ~print_any() { delete stored; }

    friend void swap(print_any& lhs, print_any& rhs)
    {
        LOG("swap " << lhs << " and " << rhs);
        std::swap(lhs.stored, rhs.stored);
    }

    print_any& operator=(print_any rhs)
    {
        LOG("assign " << *this << " = " << rhs);
        swap(*this, rhs);
        return *this;
    }
 private:
    storage const* stored;
};

struct value
{
    value(object const& x) : stored(x) {}
    value(array const& x) : stored(x) {}
# ifdef NO_ENABLE_IF
    value(string const& x) : stored(x) {}
    template <class T>
    value(T const& x) : stored(number(x)) {}
    value(char const* x) : stored(string(x)) {}
# else 
    template <class T>
    value(
        T const& x,
        typename boost::enable_if<boost::is_arithmetic<T> >::type* = 0
    ) : stored(number(x)) {}
    
    template <class T>
    value(
        T const& x,
        typename boost::enable_if<boost::is_convertible<T,string> >::type* = 0
    ) : stored(string(x)) {}
# endif 
    value(boolean const& x) : stored(x) {}
    value(null x = null()) : stored(x) {}

    friend std::ostream& operator<<(std::ostream& s, value const& x)
    {
        return s << x.stored;
    }
 private:
    print_any stored;
};

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
std::ostream& print(
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
    return os << close;
}

std::ostream& operator<<(std::ostream& os, object const& obj)
{
    return print(os, obj, "{ ", " }");
}

std::ostream& operator<<(std::ostream& os, array const& a)
{
    return print(os, a);
}
}

#endif // DWA2012114_HPP
