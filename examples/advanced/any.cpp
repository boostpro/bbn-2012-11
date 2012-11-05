// Copyright Dave Abrahams 2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#include <boost/move/move.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/container/vector.hpp>
#include <boost/container/string.hpp>
#include <boost/cstdint.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/foreach.hpp>
#include <boost/static_assert.hpp>
#include <boost/io/ios_state.hpp>
#include <boost/any.hpp>
#ifdef USE_XPRESSIVE
# include <boost/xpressive/xpressive.hpp>
#else
# include <boost/algorithm/string.hpp>
# include <boost/range.hpp>
# include <boost/range/as_array.hpp>
#endif

#include <boost/phoenix.hpp>
#include <boost/phoenix/object.hpp>

#include <boost/bind.hpp>
#include <boost/operators.hpp>

#include <cassert>

#include <vector>
#include <ostream>

bool logging = false;

#ifndef NDEBUG
# include <iostream>
# define LOG(x) do { if (logging) std::cout << x << std::endl; } while(0)
#else
# define LOG(x)
#endif

struct json_null {};

std::ostream& operator<<(std::ostream& s, json_null const& x)
{
    return s << "null";
}

typedef long double json_float;

#ifndef BOOST_NO_INT64_T
typedef boost::int64_t json_integer;
#else
typedef boost::int32_t json_integer;
#endif 

struct json_string : boost::container::string
{
    typedef boost::container::string rep_t;
    
    json_string(char const* p) : rep_t(p) {}

    json_string(rep_t rhs) { rhs.swap(*this); }
    
    json_string() {}
    
    json_string(json_string const& rhs)
        : rep_t(rhs)
    {
        LOG("json_string copy: " << rhs);
    }

    template <class Iterator>
    json_string(Iterator begin, Iterator end)
        : rep_t(begin, end)
    {
    }

    json_string& operator=(BOOST_COPY_ASSIGN_REF(json_string) rhs) // Copy assignment
    {
        LOG("json_string copy assign");
        rep_t::operator=(rhs);
        return *this;
    }

    // Move constructor
    json_string(BOOST_RV_REF(json_string) rhs)            //Move constructor
        : rep_t(boost::move(rhs))
    {
        LOG("json_string move");
    }

    json_string& operator=(BOOST_RV_REF(json_string) rhs) //Move assignment
    {
        LOG("json_string move assign");
        rep_t::operator=(boost::move(rhs));
        return *this;
    }

 private:
    BOOST_COPYABLE_AND_MOVABLE(json_string)
};

inline std::ostream& operator<<(std::ostream& os, json_string const& s)
{
#ifdef USE_XPRESSIVE 
    typedef boost::xpressive::basic_regex<json_string::const_iterator> pattern_t;
    static pattern_t escape_me = pattern_t::compile("[/\"\\\\]");
    return os << "\"" << boost::xpressive::regex_replace(s, escape_me, "\\$&").c_str() << "\"";
#else
    typedef boost::container::string string;
    string const& rep = s;
    using namespace boost::phoenix::placeholders;
    using namespace boost::phoenix;
    using namespace boost::phoenix::local_names;
    
    static const char chars_to_escape[] = "\\/\"\b\f\n\r\t";
    static const char repl_chars[] = "\\/\"bfnrt";
    BOOST_STATIC_ASSERT(sizeof(chars_to_escape) == sizeof(repl_chars));
    
    return os << "\""
              << boost::find_format_all_copy(
                  rep,
                  boost::token_finder(boost::is_any_of(chars_to_escape)),
                  let ( _c = find(boost::as_array(chars_to_escape), arg1[0]) )[
                      string("\\") + val(repl_chars)[_c - &chars_to_escape[0]]
                  ]
              )
              << "\"";
#endif
}
    
struct json_value;
typedef boost::container::vector<json_value> json_array;
typedef boost::container::flat_map<json_string, json_value> json_object;

struct json_value
  : boost::totally_ordered<json_value>
{
 private:
    typedef boost::any stored_type;
    
    template <class F>
    friend typename F::result_type json_apply( F op, json_value const& x );

    friend bool operator==(json_value const& x, json_value const& y);
    friend bool operator<(json_value const& x, json_value const& y);
    friend std::ostream& operator<<(std::ostream& s, json_value const& x);
    
 public:
    json_value() {}
    
    template <class T>
    json_value(T const& x)
        : stored_value(x)
    {
    }
        
    json_value(json_value const& rhs)
      : stored_value(rhs.stored_value)
    {
        LOG("json_value copy: " << *this);
    }

    template <class T>
    json_value& operator=(T const& x)
    {
        stored_value = x;
        return *this;
    }
    

    json_value& operator=(BOOST_COPY_ASSIGN_REF(json_value) rhs) // Copy assignment
    {
        stored_value = rhs.stored_value;
        return *this;
    }

    // Move constructor
    json_value(BOOST_RV_REF(json_value) rhs)            //Move constructor
    {
        stored_value.swap(rhs.stored_value);
        LOG("json_value move");
        rhs.stored_value = stored_type();
    }

    json_value& operator=(BOOST_RV_REF(json_value) rhs) //Move assignment
    {
        LOG("json_value move assign");
        if (this != &rhs) {
            stored_value = stored_type();
            stored_value.swap(rhs.stored_value);
        }
        return *this;
    }

    friend void swap(json_value& lhs, json_value& rhs)
    {
        lhs.stored_value.swap(rhs.stored_value);
    }
 private:
    BOOST_COPYABLE_AND_MOVABLE(json_value)
    stored_type stored_value;
};

inline std::ostream& operator<<(std::ostream& s, json_array const& a)
{
    char const* separator = " ";
    s << "[";
    BOOST_FOREACH(json_value const& v, a)
    {
        s << separator << v;
        separator = ", ";
    }
    s << " ]";
    return s;
}

inline std::ostream& operator<<(std::ostream& s, json_object const& o)
{
    char const* separator = " ";
    s << "{";
    BOOST_FOREACH(json_object::value_type const& v, o)
    {
        s << separator << v.first << " : " << v.second;
        separator = ", ";
    }
    s << " }";
    return s;
}

#if 0
inline std::ostream& operator<<(std::ostream& s, json_value const& x)
{
    boost::io::ios_flags_saver ifs( s );

    if (json_integer const* y = boost::any_cast<json_integer>(&x.stored_value))
        s << *y;
    if (json_float const* y = boost::any_cast<json_float>(&x.stored_value))
        s << *y;
    else if (json_string const* y = boost::any_cast<json_string>(&x.stored_value))
        s << *y;
    else if (json_array const* y = boost::any_cast<json_array>(&x.stored_value))
        s << *y;
    else if (json_object const* y = boost::any_cast<json_object>(&x.stored_value))
        s << *y;
    else if (bool const* y = boost::any_cast<bool>(&x.stored_value))
        s << std::boolalpha << *y;
    return s;
}
#endif

template <class F>
typename F::result_type json_apply( F op, json_value const& x )
{
    if (json_integer const* y = boost::any_cast<json_integer>(&x.stored_value))
        return op(*y);
    if (json_float const* y = boost::any_cast<json_float>(&x.stored_value))
        return op(*y);
    else if (json_string const* y = boost::any_cast<json_string>(&x.stored_value))
        return op(*y);
    else if (json_array const* y = boost::any_cast<json_array>(&x.stored_value))
        return op(*y);
    else if (json_object const* y = boost::any_cast<json_object>(&x.stored_value))
        return op(*y);
    else if (bool const* y = boost::any_cast<bool>(&x.stored_value))
        return op(*y);
    else {
        assert(!"shouldn't get here!");
        std::abort();
    }
}

struct stream_out
{
    typedef std::ostream& result_type;
    
    stream_out(std::ostream& s) : s(&s) {}
    
    template <class T>
    std::ostream& operator()(T const& x) const
    { return *s << x; }

    std::ostream* s;
};

inline std::ostream& operator<<(std::ostream& s, json_value const& x)
{
    boost::io::ios_flags_saver ifs( s );
    s << std::boolalpha;
    json_apply( stream_out(s), x );
    return s;
}

struct any_equal
{
    typedef bool result_type;
    
    template <class T>
    bool operator()( boost::any const& x, T const& y ) const
    {
        return *boost::any_cast<T>(&x) == y;
    }
};

inline bool operator==(json_value const& x, json_value const& y)
{
    return x.stored_value.type() == y.stored_value.type()
        && json_apply( boost::bind(any_equal(), boost::cref(x), _1), y.stored_value );
}

struct any_less
{
    typedef bool result_type;
    
    template <class T>
    bool operator()( boost::any const& x, T const& y ) const
    {
        return *boost::any_cast<T>(&x) < y;
    }
};

inline bool operator<(json_value const& x, json_value const& y)
{
    LOG(x.stored_value.type().name() <<": " << x << " <? " << y.stored_value.type().name()<<": " << y);
    return x.stored_value.type().before(y.stored_value.type())
        || !y.stored_value.type().before(x.stored_value.type())
           && json_apply( boost::bind(any_less(), boost::cref(x.stored_value), _1), y.stored_value );
}

#include <iostream>

int main(int const argc, char const*argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        if (json_string("--log") == argv[i])
            logging = true;
    }
    
    json_value v = json_string("foo bar");
    std::cout << v << std::endl << std::flush;
    json_array a;
    assert(a == a);
    a.push_back(false);
    assert(a != a[0]);
    a.push_back(json_integer(1));
    a.push_back(json_float(42.7));
    a.push_back(json_string("b\"a\"z"));
    std::cout << "---------------------------" << std::endl;
    std::cout << a << std::endl;
    

    json_object o;
    o["foo"] = json_integer(1);
    o["bar"] = json_string("baz");
    o["xxx"] = json_float(3.14);
    o["lick"] = a;
    o["pork"] = true;
    std::cout << o << std::endl;
    std::cout << "---------------------------" << std::endl;
    a.push_back(json_float(9.1));
    a.push_back(json_float(8.1));
    a.push_back(json_float(7.1));
    a.push_back(json_integer(5));
    std::sort(a.begin(), a.end());
    std::cout << a << std::endl;
}
