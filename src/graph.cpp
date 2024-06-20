#include "utils.hpp"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/kamada_kawai_spring_layout.hpp>
#include <boost/graph/random_layout.hpp>
#include <boost/graph/topology.hpp>
#include <random>
#include <unordered_map>
#include <unordered_set>

constexpr unsigned int SCREEN_WIDTH = 1500;
constexpr unsigned int SCREEN_HEIGHT = 1000;

constexpr unsigned int MAX_NODES = 1000;

constexpr float CIRCLE_RADIUS = 20.0f;

Camera2D camera;

struct Vec {
    int key;
};
struct Edge {
    double weight;
};

typedef boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS, Vec, Edge> simplegraph_t;

typedef boost::graph_traits<simplegraph_t>::vertex_descriptor vertex_t;
typedef boost::graph_traits<simplegraph_t>::edge_descriptor edge_t;

typedef boost::rectangle_topology<std::minstd_rand> topology_t;
typedef topology_t::point_type point_t;

simplegraph_t graph;
vertex_t vertices[MAX_NODES];
std::vector<point_t> position_map;
std::minstd_rand random_gen;
topology_t topo(random_gen, 0.0, 0.0, 500.0, 500.0);

vertex_t sel0 = 0, sel1 = 0;
bool b_sel0 = false, b_sel1 = false;

void UpdateDrawFrame();
void CalculateLayout();
int main() {

    position_map.reserve(MAX_NODES);

    for (int i = 0; i < 10; i++) {
        vertices[i] = boost::add_vertex({i}, graph);
    }
    position_map.resize(boost::num_vertices(graph));

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "basic window");

    camera.offset = Vector2{(float)SCREEN_WIDTH / 2.0f, (float)SCREEN_HEIGHT / 2.0f};
    camera.rotation = 0.0f;
    camera.target = Vector2{0.0f, 0.0f};
    camera.zoom = 2.0f;

    CalculateLayout();
    EnableCursor();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    SetTargetFPS(144);
    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        UpdateDrawFrame();
    }
#endif

    CloseWindow();

    return 0;
}

void tarjan(const vertex_t current, const vertex_t parent, int &time, std::unordered_set<vertex_t> &visited,
            std::unordered_map<vertex_t, int> &discovery, std::unordered_map<vertex_t, int> &lowpoint,
            std::unordered_set<vertex_t> &aps) {
    time++;
    size_t children = 0;
    bool isroot = current == parent;

    visited.insert(current);
    discovery[current] = lowpoint[current] = time;

    boost::graph_traits<simplegraph_t>::adjacency_iterator iter, end;
    for (boost::tie(iter, end) = boost::adjacent_vertices(current, graph); iter != end; iter++) {
        if (visited.find(*iter) == visited.end()) // not visited
        {
            children++;
            tarjan(*iter, current, time, visited, discovery, lowpoint, aps);
            lowpoint[current] = std::min(lowpoint[current], lowpoint[*iter]);
            if (!isroot && lowpoint[*iter] >= discovery[current]) {
                std::cout << current << std::endl;
                aps.insert(current);
            }
        } else {
            lowpoint[current] = std::min(lowpoint[current], discovery[*iter]);
        }
    }

    if (isroot && children >= 2) {
        std::cout << current << std::endl;
        aps.insert(current);
    }
}
std::unordered_set<vertex_t> aps;
void CalculateLayout() {
    int time;
    std::unordered_set<vertex_t> visited;
    std::unordered_map<vertex_t, int> discovery;
    std::unordered_map<vertex_t, int> lowpoint;
    aps.clear();

    boost::graph_traits<simplegraph_t>::vertex_iterator iter, end;
    for (boost::tie(iter, end) = boost::vertices(graph); iter != end; iter++) {
        if (aps.find(*iter) == aps.end()) {
            tarjan(*iter, *iter, time, visited, discovery, lowpoint, aps);
        }
    }

    auto position_map_property =
        boost::make_iterator_property_map(position_map.begin(), boost::get(boost::vertex_index, graph));

    boost::random_graph_layout(graph, position_map_property, topo);
    boost::kamada_kawai_spring_layout(graph, position_map_property, boost::get(&Edge::weight, graph), topo,
                                      boost::edge_length(100.0));
}

void CalculatePhysics(float_t dt, float_t ft) {
    static float_t last = 0.0f;
    if (ft - last > 1.0f) {
        last = ft;
        // boost::add_vertex(Vec{static_cast<int>(boost::num_vertices(graph) + 1)}, graph);
        // position_map.push_back(point_t{});
        // CalculateLayout();
    }
}

bool get_vertex_by_cursor(Vector2 vert, vertex_t &vertex_out) // pass by value because this is cheap
{
    boost::graph_traits<simplegraph_t>::vertex_iterator i, end;
    for (boost::tie(i, end) = boost::vertices(graph); i != end; i++) {
        const auto &p = position_map[*i];
        if (Vector2Distance(vert, Vector2{static_cast<float>(p[0]), static_cast<float>(p[1])}) < CIRCLE_RADIUS) {
            vertex_out = *i;
            return true;
        }
    }
    return false;
}

void UpdateDrawFrame() {
    float_t dt = GetFrameTime();
    float_t ft = (float)GetTime();

    ClearBackground(BLACK);

    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        DisableCursor();
    } else if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
        EnableCursor();
    }

    if (IsCursorHidden()) {
        camera.target = Vector2Add(Vector2Scale(GetMouseDelta(), 100.0f * dt / camera.zoom), camera.target);
    }

    camera.zoom = Clamp(camera.zoom + GetMouseWheelMove() * 300.0f * dt, 2.0f, 1000.0f);
    if (IsKeyPressed(KEY_T))
        camera.target = Vector2{static_cast<float>(position_map[0][0]), static_cast<float>(position_map[0][1])};

    // This converts our mouse cursor (local relative to window)
    // to absolute coord with camera
    Vector2 cursorPos;
    if (!IsCursorHidden()) {
        Vector2 rawPos = GetMousePosition();
        cursorPos = Vector2Add(Vector2Scale(Vector2{(rawPos.x / (float)SCREEN_WIDTH - 0.5f) * (float)SCREEN_WIDTH,
                                                    (rawPos.y / (float)SCREEN_HEIGHT - 0.5f) * (float)SCREEN_HEIGHT},
                                            1.0f / camera.zoom),
                               camera.target);
    } else {
        cursorPos = camera.target;
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        vertex_t vert_out;
        if (!b_sel0) {
            bool success = get_vertex_by_cursor(cursorPos, vert_out);
            if (success) {
                b_sel0 = true;
                sel0 = vert_out;
            }
        } else if (!b_sel1) {
            bool success = get_vertex_by_cursor(cursorPos, vert_out);
            if (success && vert_out != sel0) {
                b_sel1 = true;
                sel1 = vert_out;
            }
        }
    }

    if (b_sel0 && b_sel1) {
        boost::add_edge(sel0, sel1, graph);
        CalculateLayout();
        b_sel0 = b_sel1 = sel0 = sel1 = false;
    }

    CalculatePhysics(dt, ft);
    BeginDrawing();
    {
        BeginMode2D(camera);

        // draw vertices
        auto id_map = boost::get(boost::vertex_index, graph);
        bool visited[MAX_NODES] = {false};
        boost::graph_traits<simplegraph_t>::vertex_iterator i, end;
        boost::graph_traits<simplegraph_t>::adjacency_iterator ai, a_end;

        for (boost::tie(i, end) = boost::vertices(graph); i != end; ++i) {
            const auto &p = position_map[*i];
            for (boost::tie(ai, a_end) = boost::adjacent_vertices(*i, graph); ai != a_end; ++ai) {
                if (ai == a_end)
                    break;
                if (visited[*ai]) {
                    continue;
                }
                const auto &p2 = position_map[*ai];
                DrawLine(static_cast<int>(p[0]), static_cast<int>(p[1]), static_cast<int>(p2[0]),
                         static_cast<int>(p2[1]), RED);
            }
            visited[*i] = true;
        }
        for (boost::tie(i, end) = boost::vertices(graph); i != end; ++i) {
            const auto &p = position_map[*i];
            Color circle_color = RED;
            if ((b_sel0 && *i == sel0) || (b_sel1 && *i == sel1)) {
                circle_color = BLUE;
            } else if (aps.find(*i) != aps.end()) {
                circle_color = PURPLE;
            }
            DrawCircle(static_cast<int>(p[0]), static_cast<int>(p[1]), CIRCLE_RADIUS, circle_color);
            DrawText(std::to_string(boost::get(&Vec::key, graph)[*i]).c_str(), static_cast<int>(p[0]),
                     static_cast<int>(p[1]), 18, BLACK);
        }
        EndMode2D();
        DrawFPS(0, 0);
    }
    // stop command buffer recording
    EndDrawing();
}