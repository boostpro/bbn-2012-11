// Copyright David Abrahams 2008. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "boost/spirit.hpp"
#include "boost/spirit/phoenix.hpp"

namespace spirit = boost::spirit;

struct calculator : spirit::grammar< $writeme$ >
{
    $writeme$
};

int main()
{
    using namespace phoenix;
    double result = -33;
    calculator calc;
    spirit::parse_info<> parsed = spirit::parse("3 * (1 + 22 / 7)", calc[ var(result) = arg1 ], spirit::space_p);
    std::cout << "result = " << result << std::endl;
    assert(result > 12.4 && result < 12.5);
    return !parsed.full;
}
