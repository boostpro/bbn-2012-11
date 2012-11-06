// Copyright David Abrahams 2010. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef TEST_MAIN_DWA2010422_HPP
# define TEST_MAIN_DWA2010422_HPP

#include "eh_test.hpp"
#include <cstdlib>
#include <cassert>

// Adds an invariant check to sparse, especially *before destruction*
// which will happen upon unwinding after an exception.
struct checked_sparse : sparse
{
    checked_sparse(std::size_t nrows, std::size_t nstored)
        : sparse(nrows,nstored)
    {
        check_invariant();
    }
    
    ~checked_sparse()
    {
        check_invariant();
    }
    
    void check_invariant()
    {
        std::size_t const len = this->end() - this->begin();
        assert( len == this->column_index_end() - this->column_index_begin() );
        std::size_t const nrows = this->row_start_end() - this->row_start_begin();

        std::size_t const* col_indices = len > 0 ? &*this->column_index_begin() : 0;
    
        for (std::size_t row = 0; row < nrows; ++row)
        {
            std::size_t const row_start = this->row_start_begin()[row];
            std::size_t const row_end = row + 1 < nrows ? this->row_start_begin()[row+1] : this->end() - this->begin();
            assert( row_end >= row_start );
            for (std::size_t i = row_start; i != row_end; ++i)
            {
                assert( col_indices[i] < nrows );
                assert( i == row_start || col_indices[i] > col_indices[i-1] );
            }
        }
    }
};

struct test_op
{
    typedef ::checked_sparse sparse;
    
    test_op()
        : dim1( std::rand() * 1000.0 / RAND_MAX ),
          dim2( std::rand() * 1000.0 / RAND_MAX )
    {
    }
    
    void operator()() const
    {
        sparse x = mklower(dim1);
        x.check_invariant();
        sparse y(x);
        y.check_invariant();
        assert(x == y);

        sparse z = mklower(dim2);
        z.check_invariant();
        assert( (x == z) == (dim1 == dim2) );

        z = x;
        assert(x == z);
        assert(x == y);
    }

    sparse mklower(std::size_t dim) const
    {
        // make a lower triangular matrix
        sparse s(dim, dim * (dim + 1)/2);
        sparse::iterator p = s.begin();
        sparse::row_start_iterator row_start = s.row_start_begin();
        sparse::column_index_iterator col_index = s.column_index_begin();

        for (std::size_t r = 0; r < dim; ++r)
        {
            *row_start++ = p - s.begin();
            for (std::size_t c = 0; c <= r; ++c)
            {
                *p++ = r * c;
                *col_index++ = c;
            }
        }
        return s;
    }
    
    std::size_t dim1,dim2;
};

int main()
{
    eh_test::setup();
    std::srand(314159279);

    for (std::size_t n = 0; n < 10; ++n)
        eh_test::eh_test(test_op());
    
    // Note: this assertion may not be valid for all possible tests
    // because standard libraries often intentionally create pool
    // allocators that only return memory to the global heap upon
    // termination.
    assert( eh_test::allocs == 0 );
}

#endif // TEST_MAIN_DWA2010422_HPP
