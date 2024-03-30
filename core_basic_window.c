/*******************************************************************************************
 *   raylib [core] example - Basic window (adapted for HTML5 platform)
 ********************************************************************************************/

#include "raylib.h"
#include "raymath.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

int screenWidth = 800;
int screenHeight = 450;
Camera3D camera = {0};
Mesh cubeMesh;
Model cubeModel;
Vector3 cubePos = {0.0, 0.0, 0.0};

void UpdateDrawFrame();
int main() {
    // Initialization
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "basic window");

    camera.position = (Vector3){10.0f, 10.0f, 10.0f}; // Camera position
    camera.target = (Vector3){0.0f, 0.0f, 0.0f};      // Camera looking at point
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;                    // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE; // Camera projection type

    cubeMesh = GenMeshCube(2.0, 4.0, 2.0);
    cubeModel = LoadModelFromMesh(cubeMesh);

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    SetTargetFPS(60);
    DisableCursor();
    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        UpdateDrawFrame();
    }
#endif
    CloseWindow();
    return 0;
}

void UpdateDrawFrame() {
    const float dt = GetFrameTime();
    UpdateCamera(&camera, CAMERA_FREE);
    cubeModel.transform =
        MatrixMultiply(MatrixRotateX(dt * 2 * PI / 180), cubeModel.transform);
    ClearBackground(BLACK);

    // start command buffer recording
    BeginDrawing();
    {
        // start 3D mode recording
        BeginMode3D(camera);
        DrawModel(cubeModel, cubePos, 1.0, RED);
        DrawModelWires(cubeModel, cubePos, 1.0, BLACK);

        // stop 3D mode recording
        EndMode3D();

        DrawFPS(0, 0);
    }
    // stop command buffer recording
    EndDrawing();
}