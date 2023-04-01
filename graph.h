#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/ref.hpp>
#include <vector>

#include <boost/graph/planar_canonical_ordering.hpp>
#include <boost/graph/is_straight_line_drawing.hpp>
#include <boost/graph/chrobak_payne_drawing.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>
#include <boost/graph/make_maximal_planar.hpp>
#include <boost/graph/make_biconnected_planar.hpp>
#include <boost/graph/planar_face_traversal.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/make_connected.hpp>

using namespace boost;

bool rearrangeGraph(const int vertex_count, const std::vector<std::pair<int, int>>& initial_edges, std::vector<std::pair<int, int>>& coordinates)
{
    if (vertex_count < 3)
        return false;

    // a class to hold the coordinates of the straight line embedding
    struct coord_t
    {
        std::size_t x;
        std::size_t y;
    };

    typedef adjacency_list <vecS, vecS, undirectedS, property<vertex_index_t, int>, property<edge_index_t, int>> graph;

    // Define the storage type for the planar embedding
    typedef std::vector<std::vector<graph_traits<graph>::edge_descriptor>> embedding_storage_t;
    typedef boost::iterator_property_map<embedding_storage_t::iterator, property_map<graph, vertex_index_t>::type> embedding_t;

    // Create the graph - a maximal planar graph on N>=3 vertices. The functions
    // planar_canonical_ordering and chrobak_payne_straight_line_drawing both
    // require a maximal planar graph. If you start with a graph that isn't
    // maximal planar (or you're not sure), you can use the functions
    // make_connected, make_biconnected_planar, and make_maximal planar in
    // sequence to add a set of edges to any undirected planar graph to make
    // it maximal planar.

    graph g(vertex_count);
    for (const auto& pe : initial_edges)
    {
        add_edge(pe.first, pe.second, g);
    }

    // Create the planar embedding
    embedding_storage_t embedding_storage(num_vertices(g));
    embedding_t embedding(embedding_storage.begin(), get(vertex_index, g));

    if (!boyer_myrvold_planarity_test(boyer_myrvold_params::graph = g, boyer_myrvold_params::embedding = embedding))
        return false;







    //Initialize the interior edge index
    property_map<graph, edge_index_t>::type e_index = get(edge_index, g);
    graph_traits<graph>::edges_size_type edge_count = 0;
    graph_traits<graph>::edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
        put(e_index, *ei, edge_count++);


    std::cout << "Initial edges is: " << std::endl;
    for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
        std::cout << *ei << std::endl;




    make_connected(g);
    // Re-initialize the edge index, since we just added a few edges
    edge_count = 0;
    for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
        put(e_index, *ei, edge_count++);

    std::cout << "After make_connected edges is: " << std::endl;
    for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
        std::cout << *ei << std::endl;





    make_biconnected_planar(g, &embedding[0]);

    // Re-initialize the edge index, since we just added a few edges
    edge_count = 0;
    for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
        put(e_index, *ei, edge_count++);

    std::cout << "After make_biconnected_planar edges is: " << std::endl;
    for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
        std::cout << *ei << std::endl;





    //Test for planarity again; compute the planar embedding as a side-effect
    if (boyer_myrvold_planarity_test(boyer_myrvold_params::graph = g, boyer_myrvold_params::embedding = &embedding[0]))
        std::cout << "After calling make_biconnected, the graph is still planar" << std::endl;
    else
        std::cout << "After calling make_biconnected, the graph is not planar" << std::endl;






    make_maximal_planar(g, &embedding[0]);

    // Re-initialize the edge index, since we just added a few edges
    edge_count = 0;
    for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
        put(e_index, *ei, edge_count++);

    std::cout << "After make_maximal_planar edges is: " << std::endl;
    for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
        std::cout << *ei << std::endl;





    // Test for planarity one final time; compute the planar embedding as a side-effect
    std::cout << "After calling make_maximal_planar, the final graph ";
    if (boyer_myrvold_planarity_test(boyer_myrvold_params::graph = g, boyer_myrvold_params::embedding = &embedding[0]))
        std::cout << "is planar." << std::endl;
    else
        std::cout << "is not planar." << std::endl;

    std::cout << "The final graph has " << num_edges(g) << " edges." << std::endl;





    // Find a canonical ordering
    std::vector< graph_traits< graph >::vertex_descriptor > ordering;
    planar_canonical_ordering(g, embedding, std::back_inserter(ordering));

    // Set up a property map to hold the mapping from vertices to coord_t's
    typedef std::vector< coord_t > straight_line_drawing_storage_t;
    typedef boost::iterator_property_map<straight_line_drawing_storage_t::iterator,
        property_map<graph, vertex_index_t>::type> straight_line_drawing_t;

    straight_line_drawing_storage_t straight_line_drawing_storage(num_vertices(g));
    straight_line_drawing_t straight_line_drawing(straight_line_drawing_storage.begin(), get(vertex_index, g));

    // Compute the straight line drawing
    chrobak_payne_straight_line_drawing(g, embedding, ordering.begin(), ordering.end(), straight_line_drawing);

    std::cout << "After chrobak_payne_straight_line_drawing edges is: " << std::endl;
    for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
        std::cout << *ei << std::endl;

    
    
    
    std::cout << "The straight line drawing is: " << std::endl;
    graph_traits< graph >::vertex_iterator vi, vi_end;
    for (boost::tie(vi, vi_end) = vertices(g); vi != vi_end; ++vi)
    {
        coord_t coord(get(straight_line_drawing, *vi));
        std::cout << *vi << " -> (" << coord.x << ", " << coord.y << ")" << std::endl;
    }

    // Verify that the drawing is actually a plane drawing
    if (is_straight_line_drawing(g, straight_line_drawing))
        std::cout << "Is a plane drawing." << std::endl;
    else
        std::cout << "Is not a plane drawing." << std::endl;




    coordinates.clear();
    for (boost::tie(vi, vi_end) = vertices(g); vi != vi_end; ++vi)
    {
        coord_t coord(get(straight_line_drawing, *vi));
        coordinates.push_back({ coord.x, coord.y });
    }

    return true;
}


