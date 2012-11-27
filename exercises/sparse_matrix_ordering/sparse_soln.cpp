// Copyright David Abrahams 2008. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#define NOMINMAX
#define _GLIBCXX_DEBUG 1

#include <vector>
#include <cstdlib>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <cassert>
#include <vector>
#include <set>
#include <utility>
#include <limits>
#include <cstdlib>
#include <ctime>

#include "boost/next_prior.hpp"
#include "boost/foreach.hpp"
#include "boost/iterator/counting_iterator.hpp"
#include "boost/graph/cuthill_mckee_ordering.hpp"
#include "boost/iterator/permutation_iterator.hpp"


//
// Matrix "library" starts here.  Do not modify this code in solving
// your problem.  The goal is to retroactively adapt matrices::sparse
// to the necessary concepts to make this program work
//
namespace matrices {

struct sparse
{
    sparse(std::size_t nrows, std::size_t nstored)
      : data(nstored),
        row_start(nrows + 1),
        column_index(nstored)
    {
        row_start[nrows] = nstored;
    }

    friend void swap(sparse& x, sparse& y)
    {
        std::swap(x.row_start,y.row_start);
        std::swap(x.column_index,y.column_index);
        std::swap(x.data,y.data);
    }

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
    

    std::size_t dim() const
    {
        return row_start.size() - 1;
    }
    
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
        return row_start.end() - 1;
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
        return row_start.end() - 1;
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
    std::vector<double> data;
    std::vector<std::size_t> row_start;
    std::vector<std::size_t> column_index;
};

// Return true iff s with p applied has a nonzero (stored) element at (row, col)
template <class InversePermutation>
bool has_nonzero(sparse const& s, InversePermutation p, std::size_t row, std::size_t col)
{
    row = p[row];
    col = p[col];
    std::size_t const* r = &*(s.row_start_begin() + row);
    return std::binary_search(s.column_index_begin() + r[0], s.column_index_begin() + r[1], col);
}

// Print an 'X' for every stored element of p(s) and a '.' for the rest
template <class InversePermutation>
std::ostream& print_nonzero(std::ostream& o, sparse const& s, InversePermutation p)
{
    std::size_t dim = s.dim();
    
    for (std::size_t row = 0; row != dim; ++row)
    {
        for (std::size_t col = 0; col != dim; ++col)
        {
            o << (has_nonzero(s, p, row, col) ? "X " : ". ");
        }
        o << std::endl;
    }
    return o;
}

// Print s unpermuted
std::ostream& operator<<(std::ostream& o, sparse const& s)
{
    return print_nonzero(o, s, boost::make_counting_iterator(std::size_t(0)));
}

}

//
// Your playground starts here!
//
namespace boost
{
  template <>
  struct graph_traits<matrices::sparse>
  {
      // You will need to fill out this template specialization to
      // satisfy the VertexListGraph and IncidenceGraph concepts
      typedef std::size_t vertex_descriptor;
      typedef matrices::sparse::column_index_const_iterator edge_descriptor;
      typedef undirected_tag directed_category;
      typedef disallow_parallel_edge_tag edge_parallel_category;
      struct traversal_category : vertex_list_graph_tag, incidence_graph_tag {};

      static vertex_descriptor null_vertex() { return std::numeric_limits<std::size_t>::max(); }

      // Fulfill VertexListGraph concept requirements
      typedef boost::counting_iterator<vertex_descriptor> vertex_iterator;
      typedef std::size_t vertices_size_type;

      // Fulfill IncidenceGraph concept requirements
      typedef boost::counting_iterator<edge_descriptor> out_edge_iterator;
      typedef std::size_t degree_size_type;
  };
}

namespace matrices
{
  //
  // You will also need to define some free functions in this
  // namespace to satisfy the VertexListGraph and IncidenceGraph
  // requirements.
  //

  // Convenience typedefs
  typedef boost::graph_traits<sparse>::vertex_iterator sparse_vertex_iterator;
  typedef boost::graph_traits<sparse>::out_edge_iterator sparse_out_edge_iterator;
  typedef boost::graph_traits<sparse>::edge_descriptor sparse_edge_descriptor;
  typedef boost::graph_traits<sparse>::vertex_descriptor sparse_vertex_descriptor;
  typedef boost::graph_traits<sparse>::degree_size_type sparse_degree_size_type;
  typedef boost::graph_traits<sparse>::degree_size_type sparse_vertices_size_type;

  boost::graph_traits<sparse>::vertices_size_type
  inline num_vertices(sparse const& s)
  {
      return s.row_start_end() - s.row_start_begin();
  }
  
  std::pair<sparse_vertex_iterator, sparse_vertex_iterator>
  inline vertices(sparse const& s)
  {
      return std::make_pair(
          sparse_vertex_iterator(0), 
          sparse_vertex_iterator(num_vertices(s))
      );
  }

  inline sparse_vertex_descriptor source(sparse_edge_descriptor e, sparse const& s)
  {
      sparse::row_start_const_iterator p = std::upper_bound(
          s.row_start_begin(), s.row_start_end(), e - s.column_index_begin());
      
      return p - s.row_start_begin();
  }
  
  inline sparse_vertex_descriptor target(sparse_edge_descriptor e, sparse const& s)
  {
      return *e;
  }

  std::pair<sparse_out_edge_iterator, sparse_out_edge_iterator>
  inline out_edges(sparse_vertex_descriptor u, sparse const& s)
  {
      return std::make_pair(
          s.row_start_begin()[u] + s.column_index_begin(),
          s.row_start_begin()[u+1] + s.column_index_begin());
  }

  inline sparse_degree_size_type out_degree(sparse_vertex_descriptor u, sparse const& s)
  {
      return s.row_start_begin()[u+1] - s.row_start_begin()[u];
  }
}

/************************************************************************************
  Note: If you have an older version of Boost you will need to apply
  this patch to boost/graph/detail/sparse_ordering.hpp.  There appear to
  be a couple of "const"s missing.

Index: sparse_ordering.hpp
===================================================================
--- sparse_ordering.hpp	(revision 47085)
+++ sparse_ordering.hpp	(working copy)
@@ -128,7 +128,7 @@
   //
   template <class Graph, class Vertex, class ColorMap, class DegreeMap>
   Vertex 
-  pseudo_peripheral_pair(Graph& G, const Vertex& u, int& ecc,
+  pseudo_peripheral_pair(Graph const& G, const Vertex& u, int& ecc,
                          ColorMap color, DegreeMap degree)
   {
     typedef typename property_traits<ColorMap>::value_type ColorValue;
@@ -152,7 +152,7 @@
   // of the ordering generated by RCM.
   //
   template <class Graph, class Vertex, class Color, class Degree> 
-  Vertex find_starting_node(Graph& G, Vertex r, Color color, Degree degree)
+  Vertex find_starting_node(Graph const& G, Vertex r, Color color, Degree degree)
   {
     Vertex x, y;
     int eccen_r, eccen_x;

*************************************************************************************/

//
// Your playground ends here!
//
int main()
{
    using matrices::sparse;
    
    std::size_t const dim = 40;
    double const sparsity = 0.1;
    std::size_t val_max = 10;

    std::srand( std::time(0));
    typedef std::pair<std::size_t, std::size_t> coord;
    std::set<coord> filled;
    std::size_t sz = std::size_t(dim*dim*sparsity);
    while (filled.size() < sz)
    {
        std::size_t row = std::rand() * 1.0 * dim / RAND_MAX;
        std::size_t col = std::rand() * 1.0 * dim / RAND_MAX;
        filled.insert(std::make_pair(row,col));
        filled.insert(std::make_pair(col,row));
    }

    sparse s(dim, filled.size());
    
    sparse::iterator data = s.begin();
    sparse::column_index_iterator col_index = s.column_index_begin();
    sparse::row_start_iterator row_starts = s.row_start_begin();
    
    std::size_t last_row = 0;
    row_starts[last_row] = 0;
    for (std::set<coord>::iterator p = filled.begin(), e = filled.end(); p != e; ++p)
    {
        assert( filled.find( std::make_pair( p->second, p->first ) )  != filled.end() );

        while (last_row < p->first)
        {
            row_starts[++last_row] = data - s.begin();
            assert( col_index - s.column_index_begin() == data - s.begin() );
        }
        *data++ = 1;
        *col_index++ = p->second;
    }

    assert( last_row == dim - 1 );
    assert( data == s.end() );
    assert( col_index == s.column_index_end() );
    
    std::cout << s;

    std::vector<std::size_t> order(dim);
    
    // get a reverse cuthill-mckee ordering
    boost::cuthill_mckee_ordering( s, order.begin(), boost::identity_property_map() );

    std::cout << "\n\n============================\n\n";
    

    // print permuted
    matrices::print_nonzero(
        std::cout, s, order);
}
