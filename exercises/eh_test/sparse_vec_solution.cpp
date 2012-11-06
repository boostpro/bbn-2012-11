// Copyright David Abrahams 2008. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
https://github.com/boostpro/bbn-2012-11/pulls
#ifndef _GLIBCXX_DEBUG 
// If this test runs too slowly for you, comment out the next line
# define _GLIBCXX_DEBUG 1
#endif

#include <vector>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <vector>

struct sparse
{
    sparse(std::size_t nrows, std::size_t nstored)
      : data(nstored),
        row_start(nrows),
        column_index(nstored)
    {
        this->init();
    }

// built-in copy ctor works fine
#if 0  
    sparse(sparse const& rhs)
    {
        $writeme$
    }
#endif

    friend void swap(sparse& x, sparse& y)
    {
        using std::swap;
        swap(x.row_start,y.row_start);
        swap(x.column_index,y.column_index);
        swap(x.data,y.data);
    }
    
    // Quiz: what's wrong with the default assignment operator in this case?
    sparse& operator=(sparse rhs)
    {
        swap(*this,rhs);
        return *this;
    }
    
    typedef std::vector<double>::iterator iterator;
    typedef std::vector<std::size_t>::iterator row_start_iterator;
    typedef std::vector<std::size_t>::iterator column_index_iterator;
    
    typedef std::vector<double>::const_iterator const_iterator;
    typedef std::vector<std::size_t>::const_iterator row_start_const_iterator;
    typedef std::vector<std::size_t>::const_iterator column_index_const_iterator;
    
    
    const_iterator begin() const
    {
        return data.begin();
    }
    
    const_iterator end() const
    {
        return data.end();
    }
    
    row_start_const_iterator row_start_begin() const
    {
        return row_start.begin();
    }
    
    row_start_const_iterator row_start_end() const
    {
        return row_start.end();
    }

    column_index_const_iterator column_index_begin() const
    {
        return column_index.begin();
    }
    
    column_index_const_iterator column_index_end() const
    {
        return column_index.end();
    }
    
    iterator begin()
    {
        return data.begin();
    }
    
    iterator end()
    {
        return data.end();
    }
    
    row_start_iterator row_start_begin()
    {
        return row_start.begin();
    }
    
    row_start_iterator row_start_end()
    {
        return row_start.end();
    }

    column_index_iterator column_index_begin()
    {
        return column_index.begin();
    }
    
    column_index_iterator column_index_end()
    {
        return column_index.end();
    }

 private:
    void init()
    {
        row_start_iterator row = row_start_begin();
        std::size_t const nrows = row_start.size();
        std::size_t const ncols = nrows;   // a square matrix
        std::size_t const nstored = data.size();

        for (std::size_t i = 0; i != nstored; ++i)
        {
            std::size_t col = column_index[i] = i % ncols;

            if (col == 0)
                *row++ = i; // start a new row
        }

        // any remaining rows are empty.
        while (row < row_start_end())
            *row++ = nstored;
    }
    
 private:
    std::vector<double> data;
    std::vector<std::size_t> row_start;
    std::vector<std::size_t> column_index;
};

bool operator==(sparse const& lhs, sparse const& rhs)
{
    return lhs.row_start_end() - lhs.row_start_begin() == rhs.row_start_end() - rhs.row_start_begin()
        && lhs.end() - lhs.begin() == rhs.end() - rhs.begin()
        && std::equal(lhs.begin(), lhs.end(), rhs.begin())
        && std::equal(lhs.column_index_begin(), lhs.column_index_end(), rhs.column_index_begin())
        && std::equal(lhs.row_start_begin(), lhs.row_start_end(), rhs.row_start_begin());
}

bool operator!=(sparse const& lhs, sparse const& rhs)
{
    return !(lhs == rhs);
}

#include "test_main.hpp"
