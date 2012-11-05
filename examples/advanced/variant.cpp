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
#include <boost/variant.hpp>
#ifdef USE_XPRESSIVE
# include <boost/xpressive/xpressive.hpp>
#else
# include <boost/algorithm/string.hpp>
# include <boost/range.hpp>
# include <boost/range/as_array.hpp>
#endif

#include <boost/phoenix.hpp>
#include <boost/phoenix/object.hpp>

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

#define USE_MOVE

#ifdef USE_MOVE
# define COPY_ASSIGN_REF(t) BOOST_COPY_ASSIGN_REF(t)
# define COPYABLE_AND_MOVABLE(t) BOOST_COPYABLE_AND_MOVABLE(t)
# define MOVE(x) boost::move(x)
#else
# define COPYABLE_AND_MOVABLE(t)
# define COPY_ASSIGN_REF(t) t const&
# define MOVE(x) x
#endif 

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

    json_string& operator=(COPY_ASSIGN_REF(json_string) rhs) // Copy assignment
    {
        LOG("json_string copy assign");
        rep_t::operator=(rhs);
        return *this;
    }

#ifdef USE_MOVE
    // Move constructor
    json_string(BOOST_RV_REF(json_string) rhs)            //Move constructor
        : rep_t(MOVE(rhs))
    {
        LOG("json_string move");
    }

    json_string& operator=(BOOST_RV_REF(json_string) rhs) //Move assignment
    {
        LOG("json_string move assign");
        rep_t::operator=(MOVE(rhs));
        return *this;
    }
#endif 

 private:
    COPYABLE_AND_MOVABLE(json_string)
};

static const char chars_needing_escape[] = "\\/\"\b\f\n\r\t";
static const char escape_suffixes[] = "\\/\"bfnrt";

BOOST_STATIC_ASSERT(sizeof(chars_needing_escape) == sizeof(escape_suffixes));

inline std::ostream& operator<<(std::ostream& os, json_string const& s)
{
#ifdef USE_XPRESSIVE 
    typedef boost::xpressive::basic_regex<json_string::const_iterator> pattern_t;
    static pattern_t escape_me = pattern_t::compile("[/\"\\\\]");
    return os << "\"" << boost::xpressive::regex_replace(s, escape_me, "\\$&").c_str() << "\"";
#else
    typedef json_string::rep_t string;
    string const& rep = s;
    using namespace boost::phoenix::placeholders;
    using namespace boost::phoenix;
    using namespace boost::phoenix::local_names;
    
    return os << "\""
              << boost::find_format_all_copy(
                  rep,
                  boost::token_finder(boost::is_any_of(chars_needing_escape)),
                  let ( _c = find(boost::as_array(chars_needing_escape), arg1[0]) )[
                      string("\\") + val(escape_suffixes)[_c - &chars_needing_escape[0]]
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
#if 0
    typedef boost::make_recursive_variant<
        json_null
      , bool 
      , json_string
      , json_integer
      , json_float
      , boost::container::vector<boost::recursive_variant_>
      , boost::container::flat_map<json_string, boost::recursive_variant_>
    >::type stored_type;
#else 
    typedef boost::variant <
        json_null,
        json_string,
        bool,
        json_integer,
        json_float,
        boost::recursive_wrapper<json_array>,
        boost::recursive_wrapper<json_object>
    > stored_type;
#endif
    friend bool operator==(json_value const& x, json_value const& y);
    friend bool operator<(json_value const& x, json_value const& y);
    friend std::ostream& operator<<(std::ostream& s, json_value const& x);
    
 public:
    json_value() {}
    
    template <class T>
    json_value(T x)
      : stored_value(
          to_storable(MOVE(x), boost::is_integral<T>(), boost::is_floating_point<T>())
      )
    {}

    json_value(json_value const& rhs)
        : stored_value(rhs.stored_value)
    {
        LOG("json_value copy");
    }

#ifdef USE_MOVE
    template <class T>
    json_value& operator=(T const& rhs)
    {
        json_value converted(rhs);
        swap(stored_value, converted.stored_value);
        return *this;
    }
#endif
    
    json_value& operator=(COPY_ASSIGN_REF(json_value) rhs)
    {
        LOG("json_value copy assign");
        stored_value=rhs.stored_value;
        return *this;
    }

#ifdef USE_MOVE
        // Move constructor
    json_value(BOOST_RV_REF(json_value) rhs)            //Move constructor
    {
        stored_value.swap(rhs.stored_value);
        LOG("json_value move");
        rhs.stored_value = json_null();
    }

    json_value& operator=(BOOST_RV_REF(json_value) rhs) //Move assignment
    {
        LOG("json_value move assign");
        if (this != &rhs) {
            stored_value = json_null();
            stored_value.swap(rhs.stored_value);
        }
        return *this;
    }
#endif
    
    friend void swap(json_value& lhs, json_value& rhs)
    {
        lhs.stored_value.swap(rhs.stored_value);
    }
 private:
    COPYABLE_AND_MOVABLE(json_value)
    template <class T>
    T const& to_storable(T const& x, boost::false_type, boost::false_type) { return x; }
    
    json_string to_storable(char const* x, boost::false_type, boost::false_type) { return x; }
    bool to_storable(bool const& x, boost::true_type, boost::false_type) { return x; }
    
    template <class T>
    json_integer to_storable(T x, boost::true_type, boost::false_type) { return x; }
    
    template <class T>
    json_float to_storable(T x, boost::false_type, boost::true_type) { return x; }
    
    stored_type stored_value;
};

inline std::ostream& operator<<(std::ostream& s, json_array const& a)
{
    char const* separator = " ";
    s << "[" << std::flush;
    BOOST_FOREACH(json_value const& v, a)
    {
        s << separator << v << std::flush;
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

template <class T, class F> struct with_result_type : F
{
    typedef T result_type;
    with_result_type(F x) : F(x) {}
};

template <class T, class F>
with_result_type<T,F> make_result_type(F x)
{
    return with_result_type<T, F>(x);
}

inline std::ostream& operator<<(std::ostream& s, json_value const& x)
{
    boost::io::ios_flags_saver ifs( s );
    s << std::boolalpha;
    
    using namespace boost::phoenix::placeholders;
    using namespace boost::phoenix;
    
    boost::apply_visitor( make_result_type<std::ostream&>(s << arg1), x.stored_value );
    return s;
}

template <class Derived>
struct strict_binary_predicate
{
    typedef bool result_type;
    
    template <class T, class U>
    bool operator()( T const& x, U const& y ) const
    {
        return false;
    }
    
    template <class T>
    bool operator()( T const& x, T const& y ) const
    {
        return static_cast<Derived const*>(this)->check(x,y);
    }
};

struct equal : strict_binary_predicate<equal>
{
    template <class T>
    bool check( T const& x, T const& y ) const
    {
        return x == y;
    }
};

struct less : strict_binary_predicate<less>
{
    template <class T>
    bool check( T const& x, T const& y ) const
    {
        return x < y;
    }
};

inline bool operator==(json_value const& x, json_value const& y)
{
    return boost::apply_visitor(equal(), x.stored_value, y.stored_value);
}

inline bool operator<(json_value const& x, json_value const& y)
{
    return boost::apply_visitor(less(), x.stored_value, y.stored_value);
}


// ------------ test driver --------------

#ifndef NO_TEST
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
    a.push_back(1);
    a.push_back(42.7);
    a.push_back("b\"a\"z");
    std::cout << "---------------------------" << std::endl;
    std::cout << a << std::endl;


    json_object o;
    o["foo"] = 1;
    o["bar"] = "baz";
    o["xxx"] = 3.14;
    o["lick"] = a;
    o["pork"] = true;
    std::cout << o << std::endl;
    std::cout << "---------------------------" << std::endl;
    a.push_back(9.1);
    a.push_back(8.1);
    a.push_back(7.1);
    a.push_back(5);
    std::sort(a.begin(), a.end());
    std::cout << a << std::endl;
}
#endif
