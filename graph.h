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

bool makeConnected(const int vertex_count, std::vector<std::pair<int, int>>& initial_edges)
{
    // Sort edges
    std::sort(initial_edges.begin(), initial_edges.end(), [](std::pair<int, int> a, std::pair<int, int> b)
        {
            return a.first < b.first || (a.first == b.first && a.second < b.second);
        });

    // Create graph
    typedef adjacency_list <vecS, vecS, undirectedS, property<vertex_index_t, int>, property<edge_index_t, int>> graph;
    graph g(vertex_count);
    for (const auto& pe : initial_edges)
        add_edge(pe.first, pe.second, g);

    // Create the planar embedding
    typedef std::vector<std::vector<graph_traits<graph>::edge_descriptor>> embedding_storage_t;
    typedef boost::iterator_property_map<embedding_storage_t::iterator, property_map<graph, vertex_index_t>::type> embedding_t;
    embedding_storage_t embedding_storage(num_vertices(g));
    embedding_t embedding(embedding_storage.begin(), get(vertex_index, g));

    // Test for planarity
    if (boyer_myrvold_planarity_test(boyer_myrvold_params::graph = g, boyer_myrvold_params::embedding = embedding))
    {
        std::cout << "Graph is planar" << std::endl;
    }
    else
    {
        std::cout << "Graph is not planar" << std::endl;
        return false;
    }

    //Initialize the interior edge index
    property_map<graph, edge_index_t>::type e_index = get(edge_index, g);
    graph_traits<graph>::edges_size_type edge_count = 0;
    graph_traits<graph>::edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
        put(e_index, *ei, edge_count++);

    // Output edges before make connected
    std::cout << "Before make connected: " << std::endl;
    for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
        std::cout << *ei << std::endl;

    // Make connected
    make_connected(g);

    // Get new edges
    initial_edges.clear();
    for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
        initial_edges.push_back(std::make_pair(ei->m_source, ei->m_target));
    std::sort(initial_edges.begin(), initial_edges.end(), [](std::pair<int, int> a, std::pair<int, int> b)
        {
            return a.first < b.first || (a.first == b.first && a.second < b.second);
        });

    return true;
}

bool makeBiConnected(const int vertex_count, std::vector<std::pair<int, int>>& initial_edges)
{
    // Sort edges
    std::sort(initial_edges.begin(), initial_edges.end(), [](std::pair<int, int> a, std::pair<int, int> b)
        {
            return a.first < b.first || (a.first == b.first && a.second < b.second);
        });

    // Create graph
    typedef adjacency_list <vecS, vecS, undirectedS, property<vertex_index_t, int>, property<edge_index_t, int>> graph;
    graph g(vertex_count);
    for (const auto& pe : initial_edges)
        add_edge(pe.first, pe.second, g);

    // Create the planar embedding
    typedef std::vector<std::vector<graph_traits<graph>::edge_descriptor>> embedding_storage_t;
    typedef boost::iterator_property_map<embedding_storage_t::iterator, property_map<graph, vertex_index_t>::type> embedding_t;
    embedding_storage_t embedding_storage(num_vertices(g));
    embedding_t embedding(embedding_storage.begin(), get(vertex_index, g));
    if (boyer_myrvold_planarity_test(boyer_myrvold_params::graph = g, boyer_myrvold_params::embedding = embedding))
    {
        std::cout << "Graph is planar" << std::endl;
    }
    else
    {
        std::cout << "Graph is not planar" << std::endl;
        return false;
    }

    //Initialize the interior edge index
    property_map<graph, edge_index_t>::type e_index = get(edge_index, g);
    graph_traits<graph>::edges_size_type edge_count = 0;
    graph_traits<graph>::edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
        put(e_index, *ei, edge_count++);

    // Output edges before make biconnected
    std::cout << "Before make biconnected: " << std::endl;
    for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
        std::cout << *ei << std::endl;

    // Make biconnected
    make_biconnected_planar(g, &embedding[0]);

    // Get new edges
    initial_edges.clear();
    for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
        initial_edges.push_back(std::make_pair(ei->m_source, ei->m_target));
    std::sort(initial_edges.begin(), initial_edges.end(), [](std::pair<int, int> a, std::pair<int, int> b)
        {
            return a.first < b.first || (a.first == b.first && a.second < b.second);
        });

    return true;
}

bool makeMaximalPlanar(const int vertex_count, std::vector<std::pair<int, int>>& initial_edges)
{
    // Sort edges
    std::sort(initial_edges.begin(), initial_edges.end(), [](std::pair<int, int> a, std::pair<int, int> b)
        {
            return a.first < b.first || (a.first == b.first && a.second < b.second);
        });

    // Create graph
    typedef adjacency_list <vecS, vecS, undirectedS, property<vertex_index_t, int>, property<edge_index_t, int>> graph;
    graph g(vertex_count);
    for (const auto& pe : initial_edges)
        add_edge(pe.first, pe.second, g);

    // Create the planar embedding
    typedef std::vector<std::vector<graph_traits<graph>::edge_descriptor>> embedding_storage_t;
    typedef boost::iterator_property_map<embedding_storage_t::iterator, property_map<graph, vertex_index_t>::type> embedding_t;
    embedding_storage_t embedding_storage(num_vertices(g));
    embedding_t embedding(embedding_storage.begin(), get(vertex_index, g));
    if (boyer_myrvold_planarity_test(boyer_myrvold_params::graph = g, boyer_myrvold_params::embedding = embedding))
    {
        std::cout << "Graph is planar" << std::endl;
    }
    else
    {
        std::cout << "Graph is not planar" << std::endl;
        return false;
    }

    //Initialize the interior edge index
    property_map<graph, edge_index_t>::type e_index = get(edge_index, g);
    graph_traits<graph>::edges_size_type edge_count = 0;
    graph_traits<graph>::edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
        put(e_index, *ei, edge_count++);

    // Output edges before make maximal planar
    std::cout << "Before make maximal planar: " << std::endl;
    for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
        std::cout << *ei << std::endl;

    // Make maximal planar
    make_maximal_planar(g, &embedding[0]);

    // Get new edges
    initial_edges.clear();
    for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
        initial_edges.push_back(std::make_pair(ei->m_source, ei->m_target));
    std::sort(initial_edges.begin(), initial_edges.end(), [](std::pair<int, int> a, std::pair<int, int> b)
        {
            return a.first < b.first || (a.first == b.first && a.second < b.second);
        });

    return true;
}

bool getCoordinates(const int vertex_count, std::vector<std::pair<int, int>>& initial_edges, std::vector<std::pair<int, int>>& coordinates)
{
    // Sort edges
    std::sort(initial_edges.begin(), initial_edges.end(), [](std::pair<int, int> a, std::pair<int, int> b)
        {
            return a.first < b.first || (a.first == b.first && a.second < b.second);
        });

    // Create graph
    typedef adjacency_list <vecS, vecS, undirectedS, property<vertex_index_t, int>, property<edge_index_t, int>> graph;
    graph g(vertex_count);
    for (const auto& pe : initial_edges)
        add_edge(pe.first, pe.second, g);

    // Create the planar embedding
    typedef std::vector<std::vector<graph_traits<graph>::edge_descriptor>> embedding_storage_t;
    typedef boost::iterator_property_map<embedding_storage_t::iterator, property_map<graph, vertex_index_t>::type> embedding_t;
    embedding_storage_t embedding_storage(num_vertices(g));
    embedding_t embedding(embedding_storage.begin(), get(vertex_index, g));
    if (boyer_myrvold_planarity_test(boyer_myrvold_params::graph = g, boyer_myrvold_params::embedding = embedding))
    {
        std::cout << "Graph is planar" << std::endl;
    }
    else
    {
        std::cout << "Graph is not planar" << std::endl;
        return false;
    }

    //Initialize the interior edge index
    property_map<graph, edge_index_t>::type e_index = get(edge_index, g);
    graph_traits<graph>::edges_size_type edge_count = 0;
    graph_traits<graph>::edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
        put(e_index, *ei, edge_count++);

    // Output edges before ordering
    std::cout << "Before ordering: " << std::endl;
    for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
        std::cout << *ei << std::endl;

    // Find a canonical ordering
    typedef std::vector<graph_traits<graph>::vertex_descriptor> ordering_storage_t;
    ordering_storage_t ordering;
    planar_canonical_ordering(g, embedding, std::back_inserter(ordering));

    // Output order of verticies after ordering
    ordering_storage_t::iterator oi, oi_end;
    oi_end = ordering.end();
    std::cout << "The planar canonical ordering is: ";
    for (oi = ordering.begin(); oi != oi_end; ++oi)
        std::cout << *oi << " ";
    std::cout << std::endl;

    // A class to hold the coordinates of the straight line embedding
    struct coord_t
    {
        std::size_t x;
        std::size_t y;
    };

    // Set up a property map to hold the mapping from vertices to coord_t's
    typedef std::vector< coord_t > straight_line_drawing_storage_t;
    typedef boost::iterator_property_map<straight_line_drawing_storage_t::iterator,
        property_map<graph, vertex_index_t>::type> straight_line_drawing_t;
    straight_line_drawing_storage_t straight_line_drawing_storage(num_vertices(g));
    straight_line_drawing_t straight_line_drawing(straight_line_drawing_storage.begin(), get(vertex_index, g));

    // Compute the straight line drawing
    chrobak_payne_straight_line_drawing(g, embedding, ordering.begin(), ordering.end(), straight_line_drawing);

    // Output line drawing
    std::cout << "The straight line drawing is: " << std::endl;
    graph_traits< graph >::vertex_iterator vi, vi_end;
    for (boost::tie(vi, vi_end) = vertices(g); vi != vi_end; ++vi)
    {
        coord_t coord(get(straight_line_drawing, *vi));
        std::cout << *vi << " -> (" << coord.x << ", " << coord.y << ")" << std::endl;
    }

    // Verify that the drawing is actually a plane drawing
    if (is_straight_line_drawing(g, straight_line_drawing))
        std::cout << "Is a plane drawing" << std::endl;
    else
        std::cout << "Is not a plane drawing" << std::endl;

    // Get coordinates
    coordinates.clear();
    for (boost::tie(vi, vi_end) = vertices(g); vi != vi_end; ++vi)
    {
        coord_t coord(get(straight_line_drawing, *vi));
        coordinates.push_back({ coord.x, coord.y });
    }

    return true;
}

bool rearrangeGraph(const int vertex_count, const std::vector<std::pair<int, int>>& initial_edges, std::vector<std::pair<int, int>>& coordinates)
{
    // Create the graph - a maximal planar graph on N>=3 vertices. The functions
    // planar_canonical_ordering and chrobak_payne_straight_line_drawing both
    // require a maximal planar graph. If you start with a graph that isn't
    // maximal planar (or you're not sure), you can use the functions
    // make_connected, make_biconnected_planar, and make_maximal planar in
    // sequence to add a set of edges to any undirected planar graph to make
    // it maximal planar.

    if (vertex_count < 3)
    {
        coordinates.clear();
        coordinates.push_back({0, 0});
        if (vertex_count == 2)
            coordinates.push_back({0, 1});
        return true;
    }

    std::vector<std::pair<int, int>> edges_copy(initial_edges);
    if (!makeConnected(vertex_count, edges_copy))
        return false;

    if (!makeBiConnected(vertex_count, edges_copy))
        return false;

    if (!makeMaximalPlanar(vertex_count, edges_copy))
        return false;

    if (!getCoordinates(vertex_count, edges_copy, coordinates))
        return false;

    return true;
}


