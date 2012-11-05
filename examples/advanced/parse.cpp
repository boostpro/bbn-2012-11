// Copyright Dave Abrahams 2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef NO_TEST
# define NO_TEST
# define BUILD_PARSE_TEST
#endif

#include "tokenize.cpp"
#include "variant.cpp"
#include <boost/range.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <algorithm>

json_value parse_json_value(token_iterator& tokens);

inline std::string first_token_text(token_iterator const& tokens)
{
    return std::string(tokens->first, tokens->second);
}

inline void parse_literal(char const* text, token_iterator& tokens)
{
    LOG("parse_literal: " << text << " vs " << first_token_text(tokens));
    assert(boost::equal(boost::as_literal(text), *tokens));
    ++tokens;
}
    
inline json_string parse_json_string(token_iterator& tokens)
{
    LOG("parse_json_string: " << first_token_text(tokens));
    
    token representation = *tokens++;

    // Everything but the quotes
    json_string s(++representation.first, --representation.second);

    json_string::iterator dst = std::find(s.begin(), s.end(), '\\');
    json_string::iterator src = dst;
    while (src != s.end())
    {
        if (*src != '\\')
        {
            *dst = *src;
        }
        else
        {
            ++src;
            *dst = chars_needing_escape[
                std::find(
                    escape_suffixes,
                    escape_suffixes+sizeof(escape_suffixes),
                    *src) - escape_suffixes
            ];
            
        }
        ++dst;
        ++src;
    }
    s.erase(dst, src);
    return s;
}

inline json_value parse_json_array(token_iterator& tokens)
{
    LOG("parse_json_array: " << first_token_text(tokens));
    
    parse_literal("[", tokens);
    
    json_array a;
    while (*tokens->first != ']')
    {
        if (!a.empty())
            parse_literal(",", tokens);
        a.push_back( parse_json_value(tokens) );
    }

    parse_literal("]", tokens);
    return a;
}

inline json_value parse_json_object(token_iterator& tokens)
{
    LOG("parse_json_object: " << first_token_text(tokens));
    
    parse_literal("{", tokens);
    
    json_object o;
    while (*tokens->first != '}')
    {
        if (!o.empty())
            parse_literal(",", tokens);
        json_string s = parse_json_string(tokens);
        parse_literal(":", tokens);
        o[s] = parse_json_value(tokens);
    }

    parse_literal("}", tokens);
    return o;
}

inline json_value parse_json_number(token_iterator& tokens)
{
    LOG("parse_json_number: " << first_token_text(tokens));
    
    token tok = *tokens++;
    if (boost::contains(tok, "."))
        return boost::lexical_cast<json_float>(tok);
    else
        return boost::lexical_cast<json_integer>(tok);
}

inline json_value parse_json_value(token_iterator& tokens)
{
    LOG("parse_json_value: " << first_token_text(tokens));

    switch (*tokens->first)
    {
    case '[':
        return parse_json_array(tokens);
    case '{':
        return parse_json_object(tokens);
    case 'n':
        ++tokens;
        return json_null();
    case 't':
        ++tokens;
        return true;
    case 'f':
        ++tokens;
        return false;
    case '"':
        return parse_json_string(tokens);
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        return parse_json_number(tokens);
    default:
        assert(!"unknown token");
    }
}

#ifdef BUILD_PARSE_TEST
# include <iostream>

int main()
{
    std::string text = load_file("test.json");
    token_iterator toks = tokens(text);
    json_value x = parse_json_value(toks);
    std::cout << x << std::endl;
}
#endif 
