// Copyright Dave Abrahams 2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#include <boost/integer.hpp>
#include <boost/mpl/size_t.hpp>
#include <boost/mpl/if.hpp>
#include <boost/concept_check.hpp>
#include <boost/operators.hpp>
#include <boost/static_assert.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/graph/astar_search.hpp>
#include <boost/graph/property_maps/constant_property_map.hpp>
#include <boost/foreach.hpp>
#include <utility>
#include <map>
#include <iostream>
#include <cassert>

namespace mpl = boost::mpl;

int const knight_moves[][2] = {
    {1,2},{2,1},
    {1,-2},{-2,1},
    {-1,2},{2,-1},
    {-1,-2},{-2,-1}
};

template <unsigned Rows, unsigned Cols>
struct knights_tour
{
    static unsigned const num_positions = Rows*Cols;

    // A vertex in the tour graph is fully described by the knight's
    // current position.
    typedef unsigned vertex_descriptor;
    static unsigned x_coord(vertex_descriptor v)
    {
        return v % Cols;
    }
    
    static unsigned y_coord(vertex_descriptor v)
    {
        return v / Cols;
    }

    static vertex_descriptor pos(unsigned x, unsigned y)
    {
        return x + y * Cols;
    }

    static std::string pos_name(vertex_descriptor v)
    {
        std::stringstream s;
        char x = 'a' + x_coord(v);
        s << x << y_coord(v);
        return s.str();
    }
    
    BOOST_CONCEPT_ASSERT((boost::DefaultConstructible<vertex_descriptor>));
    BOOST_CONCEPT_ASSERT((boost::Assignable<vertex_descriptor>));
    BOOST_CONCEPT_ASSERT((boost::EqualityComparable<vertex_descriptor>));

    typedef std::pair<vertex_descriptor, signed char> edge_descriptor;

    static int next_move(vertex_descriptor const v, int i)
    {
        assert(i >= 0);
        
        int const x0 = x_coord(v);
        int const y0 = y_coord(v);
        
        while (i-- > 0)
        {
            
            int const x = x0 + knight_moves[i][0];
            int const y = y0 + knight_moves[i][1];

            if (x >= 0 && x < Rows && y >= 0 && y < Cols)
                break;
        }
        return i;
    }
    
    BOOST_CONCEPT_ASSERT((boost::DefaultConstructible<edge_descriptor>));
    BOOST_CONCEPT_ASSERT((boost::Assignable<edge_descriptor>));
    BOOST_CONCEPT_ASSERT((boost::EqualityComparable<edge_descriptor>));

    // Documented Graph requirements
    typedef boost::directed_tag directed_category;
    typedef boost::disallow_parallel_edge_tag edge_parallel_category;
    struct traversal_category
      : boost::incidence_graph_tag, boost::vertex_list_graph_tag {};

    // Incidence Graph requirements (see https://svn.boost.org/trac/boost/ticket/7741)
    struct edge_iter_base : edge_descriptor
    {
        edge_iter_base() {}
        
        edge_iter_base(vertex_descriptor v, signed char i = 8)
            : edge_descriptor(v, next_move(v, i))
        {
        }

        edge_iter_base& operator++()
        {
            this->second = next_move(this->first, this->second);
            return *this;
        }
    };

    typedef boost::counting_iterator<edge_iter_base, std::input_iterator_tag, int>
    out_edge_iterator;
    
    typedef unsigned char degree_size_type;

    friend vertex_descriptor source(edge_descriptor const& e, knights_tour)
    {
        return e.first;
    }

    friend vertex_descriptor target(edge_descriptor const& e, knights_tour)
    {
        return pos(
            x_coord(e.first) + knight_moves[e.second][0],
            y_coord(e.first) + knight_moves[e.second][1]);
    }

    friend std::pair<out_edge_iterator,out_edge_iterator>
    out_edges(vertex_descriptor const& u, knights_tour)
    {
        return std::make_pair(
            out_edge_iterator(edge_iter_base(u, 8)),
            out_edge_iterator(edge_iter_base(u, 0)));
    }
    
    friend degree_size_type
    out_degree(vertex_descriptor const& u, knights_tour)
    {
        unsigned count = 0;
        for (int i = 8; (i = next_move(u, i)) >= 0; ++count);
        return count;
    }

    // VertexListGraph requirements
    typedef boost::counting_iterator<vertex_descriptor> vertex_iterator;

    friend std::pair<vertex_iterator,vertex_iterator>
    vertices(knights_tour)
    {
        return std::make_pair(
            vertex_iterator(0),
            vertex_iterator(Rows*Cols));
    }
    
    typedef std::size_t vertices_size_type;
    friend vertices_size_type num_vertices(knights_tour)
    {
        return Rows*Cols;
    }

    // Dummies to satisfy the default implementation of graph_traits
    typedef void adjacency_iterator;
    typedef void in_edge_iterator;
    typedef void edge_iterator;
    typedef void edges_size_type;
};


template <class Tour>
struct heuristic
{
    template <class Descriptor>
    unsigned operator()(Descriptor const& d) const
    {
        return out_degree(d, Tour());
    }
};

int main()
{
    typedef knights_tour<8,8> tour;
    BOOST_CONCEPT_ASSERT((boost::IncidenceGraphConcept<tour>));
    BOOST_CONCEPT_ASSERT((boost::VertexListGraphConcept<tour>));

    tour g;
    BOOST_FOREACH(tour::vertex_descriptor v, vertices(g))
    {
        std::cout << g.pos_name(v) << ": ";
        char const* prefix = "";
        BOOST_FOREACH(tour::edge_descriptor e, out_edges(v,g))
        {
            std::cout << prefix << g.pos_name(target(e,g));
            prefix = ", ";
        }
        std::cout << std::endl;
    }
    std::cout << "======================" << std::endl;
                  
    std::map<tour::vertex_descriptor,tour::vertex_descriptor> predecessors;
    std::map<tour::vertex_descriptor,unsigned> distances;
    
    boost::astar_search(
        tour(),
        tour::vertex_descriptor(),
        heuristic<tour>(),
        boost::predecessor_map(boost::make_assoc_property_map(predecessors))
        .vertex_index_map(boost::identity_property_map())
        .weight_map(boost::make_constant_property<tour::edge_descriptor>(1))
        .distance_map(
            boost::make_assoc_property_map(distances)
        )
    );

    typedef std::pair<tour::vertex_descriptor,tour::vertex_descriptor> pred_t;
    BOOST_FOREACH(pred_t p, predecessors)
    {
        std::cout << tour::pos_name(p.second) << " --> " << tour::pos_name(p.first) << std::endl;
    }
};
