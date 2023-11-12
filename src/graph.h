namespace CubesGraph
{
    bool MakeConnected(const int vertex_count, std::vector<std::pair<int, int>>& initial_edges);
    bool MakeBiConnected(const int vertex_count, std::vector<std::pair<int, int>>& initial_edges);
    bool MakeMaximalPlanar(const int vertex_count, std::vector<std::pair<int, int>>& initial_edges);
    bool GetCoordinates(const int vertex_count, std::vector<std::pair<int, int>>& initial_edges, std::vector<std::pair<int, int>>& coordinates);
    bool RearrangeGraph(const int vertex_count, const std::vector<std::pair<int, int>>& initial_edges, std::vector<std::pair<int, int>>& coordinates);
}
