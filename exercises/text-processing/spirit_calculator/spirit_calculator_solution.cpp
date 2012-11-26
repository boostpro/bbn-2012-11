// Copyright David Abrahams 2008. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "boost/spirit.hpp"
#include "boost/spirit/phoenix.hpp"

namespace spirit = boost::spirit;

struct calc_closure
  : spirit::closure<calc_closure, double>
{
    member1 val;
};

struct calculator : spirit::grammar<calculator, calc_closure::context_t>
{
    template <class Skip> struct definition
    {
        typedef spirit::rule<Skip, calc_closure::context_t> rule;
        rule group, fact, term, expr, top;
        
        definition(calculator const & self)
        {
            using namespace phoenix;
            top = expr[ self.val = arg1 ];
            group = '(' >> expr[group.val = arg1] >> ')';
            fact = group[fact.val = arg1] | spirit::int_p[fact.val = arg1];
            term = fact[term.val = arg1] >> *(('*' >> fact[term.val *= arg1]) | ('/' >> fact[term.val /= arg1]));
            expr = term[expr.val = arg1] >> *(('+' >> term[expr.val += arg1]) | ('-' >> term[expr.val -= arg1]));
        }
        
        rule const & start() const { return top; }
    };
};

int main()
{
    using namespace phoenix;
    double result = -33;
    spirit::parse_info<> parsed = spirit::parse("3 * (1 + 22 / 7)", calculator()[ var(result) = arg1 ], spirit::space_p);
    std::cout << "result = " << result << std::endl;
    assert(result > 12.4 && result < 12.5);
    return !parsed.full;
}
