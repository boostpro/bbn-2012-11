// Copyright David Abrahams 2008. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#define _GLIBCXX_DEBUG 1

#include <algorithm>

struct sparse
{
    sparse(std::size_t nrows, std::size_t nstored)
      : data(new double[nstored]), row_start(0), 
        nrows(nrows), nstored(nstored)
    {
        try
        {
            this->row_start = new std::size_t[nrows];
            this->column_index = new std::size_t[nstored];
        }
        catch(...)
        {
            delete[] this->data;
            delete[] this->row_start;
            throw;
        }
        this->init(); // establish the invariant
    }

    sparse(sparse const& rhs)
      : data(new double[rhs.nstored]), row_start(0),
        nrows(rhs.nrows), nstored(rhs.nstored)
    {
        // Extra credit: eliminate the duplication in the two
        // constructors.
        try
        {
            this->row_start = new std::size_t[nrows];
            this->column_index = new std::size_t[nstored];
        }
        catch(...)
        {
            delete[] this->row_start;
            delete[] this->data;
            throw;
        }
        std::copy(rhs.begin(), rhs.end(), begin());
        std::copy(
            rhs.row_start_begin(), 
            rhs.row_start_end(), 
            row_start_begin());
        std::copy(
            rhs.column_index_begin(), 
            rhs.column_index_end(), 
            column_index_begin());
    }

    friend void swap(sparse& x, sparse& y)
    {
        using std::swap;
        swap(x.row_start,y.row_start);
        swap(x.column_index,y.column_index);
        swap(x.data,y.data);
        swap(x.nrows,y.nrows);
        swap(x.nstored,y.nstored);
    }
    
    sparse& operator=(sparse rhs)
    {
        swap(*this,rhs);
        return *this;
    }
    
    ~sparse()
    {
        delete[] this->data;
        delete[] this->row_start;
        delete[] this->column_index;
    }

    typedef double* iterator;
    typedef std::size_t* row_start_iterator;
    typedef std::size_t*  column_index_iterator;
    
    typedef double const* const_iterator;
    typedef std::size_t const* row_start_const_iterator;
    typedef std::size_t const*  column_index_const_iterator;
    
    const_iterator begin() const
    {
        return data;
    }
    
    const_iterator end() const
    {
        return data + nstored;
    }
    
    row_start_const_iterator row_start_begin() const
    {
        return row_start;
    }
    
    row_start_const_iterator row_start_end() const
    {
        return row_start + nrows;
    }

    column_index_const_iterator column_index_begin() const
    {
        return column_index;
    }
    
    column_index_const_iterator column_index_end() const
    {
        return column_index + nstored;
    }
    
    iterator begin()
    {
        return data;
    }
    
    iterator end()
    {
        return data + nstored;
    }
    
    row_start_iterator row_start_begin()
    {
        return row_start;
    }
    
    row_start_iterator row_start_end()
    {
        return row_start + nrows;
    }

    column_index_iterator column_index_begin()
    {
        return column_index;
    }
    
    column_index_iterator column_index_end()
    {
        return column_index + nstored;
    }

 private:
    // Establish the invariant
    void init()
    {
        row_start_iterator row = row_start_begin();
        std::size_t const ncols = nrows;   // a square matrix

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
    double* data;
    std::size_t* row_start;
    std::size_t* column_index;
    std::size_t nrows;
    std::size_t nstored;
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
