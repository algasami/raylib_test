#include "utils.hpp"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>

constexpr unsigned int SCREEN_WIDTH = 1500;
constexpr unsigned int SCREEN_HEIGHT = 1000;

Camera2D camera;

struct Vec {
    int key;
};
struct Edge {
    int weight;
};

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, Vec, Edge> SimpleGraph;

SimpleGraph graph;

void UpdateDrawFrame();
int main() {

    for (int i = 0; i < 10; i++) {
        auto vert = boost::add_vertex({i}, graph);
    }
    for (int i = 0; i < 9; i++) {
        for (int j = i + 1; j < 10; j++) {
        }
    }

    boost::print_graph(graph, boost::get(&Vec::key, graph));

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "basic window");

    camera.offset = Vector2{(float)SCREEN_WIDTH / 2.0f, (float)SCREEN_HEIGHT / 2.0f};
    camera.rotation = 0.0f;
    camera.target = Vector2{0.0f, 0.0f};
    camera.zoom = 2.0f;

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

void CalculatePhysics(float_t dt) {}

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

    CalculatePhysics(dt);
    BeginDrawing();
    {
        BeginMode2D(camera);
        EndMode2D();
        DrawFPS(0, 0);
    }
    // stop command buffer recording
    EndDrawing();
}