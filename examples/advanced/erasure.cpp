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
#ifdef USE_XPRESSIVE
# include <boost/xpressive/xpressive.hpp>
#else
# include <boost/algorithm/string.hpp>
# include <boost/range.hpp>
# include <boost/range/as_array.hpp>
#endif

#include <boost/phoenix.hpp>
#include <boost/phoenix/object.hpp>

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
        LOG("json_string copy");
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

struct json_base
{
    virtual ~json_base() {}
    virtual json_base* clone() const = 0;
    virtual void print(std::ostream&) const = 0;
};

template <class T>
struct json_store : json_base
{
    json_store(T x)
       : value(boost::move(x))
    {}

    virtual json_base* clone() const
    { return new json_store(value); }
    
    virtual void print(std::ostream& os) const
    {
        boost::io::ios_flags_saver ifs( os );
        os << std::boolalpha << value;
    }
    
    T value;
};

struct json_value
{
 private:
    typedef json_base* stored_type;
 public:
    json_value() : stored_value() {}
    
    ~json_value() { delete stored_value; }

    template <class T>
    json_value(T x)
        : stored_value(
            make_storage(x, boost::is_integral<T>(), boost::is_floating_point<T>())
        )
    {
    }

    json_value(char const* s)
        : stored_value(new json_store<json_string>(s))
    {}
    
    template <class T>
    json_value& operator=(T x)
    {
        stored_type p = make_storage(x, boost::is_integral<T>(), boost::is_floating_point<T>());
        delete stored_value;
        stored_value = p;
        return *this;
    }
    
    friend std::ostream& operator<<(std::ostream& os, json_value const&v)
    {
        v.stored_value->print(os);
        return os;
    }
    
    json_value(json_value const& rhs)
      : stored_value(rhs.stored_value ? rhs.stored_value->clone() : 0)
    {
        LOG("json_value copy");
    }

    json_value& operator=(BOOST_COPY_ASSIGN_REF(json_value) rhs) // Copy assignment
    {
        LOG("json_value copy assign");
        stored_type new_value = rhs.stored_value ? rhs.stored_value->clone() : 0;
        delete stored_value;
        stored_value = new_value;
        return *this;
    }

    // Move constructor
    json_value(BOOST_RV_REF(json_value) rhs)            //Move constructor
        : stored_value(rhs.stored_value)
    {
        LOG("json_value move");
        rhs.stored_value = 0;
    }

    json_value& operator=(BOOST_RV_REF(json_value) rhs) //Move assignment
    {
        LOG("json_value move assign");
        if (this != &rhs) {
            delete stored_value;
            stored_value = rhs.stored_value;
            rhs.stored_value = 0;
        }
        return *this;
    }
    
    friend void swap(json_value& lhs, json_value& rhs)
    {
        std::swap(lhs.stored_value, rhs.stored_value);
    }
 private:

    BOOST_STATIC_ASSERT(boost::is_integral<bool>::value);
    
    template <class T>
    static json_store<json_integer>* make_storage(T x, boost::true_type, boost::false_type)
    {
        return new json_store<json_integer>(x);
    }

    static json_store<bool>* make_storage(bool x, boost::true_type, boost::false_type)
    {
        return new json_store<bool>(x);
    }
    
    template <class T>
    static json_store<json_float>* make_storage(T x, boost::false_type, boost::true_type)
    {
        return new json_store<json_float>(x);
    }
    
    template <class T>
    static json_store<T>* make_storage(T& x, boost::false_type, boost::false_type)
    {
        return new json_store<T>(boost::move(x));
    }
    
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

#include <iostream>

int main()
{
    json_value v = json_string("foo bar");
    std::cout << v << std::endl << std::flush;
    json_array a;
    a.push_back(false);
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

    v = json_value(1);
}
