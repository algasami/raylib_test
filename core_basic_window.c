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

int main() {
    // Initialization
    InitWindow(screenWidth, screenHeight, "basic window");

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    SetTargetFPS(60);

    Camera3D camera = {0};
    camera.position = (Vector3){10.0f, 10.0f, 10.0f}; // Camera position
    camera.target = (Vector3){0.0f, 0.0f, 0.0f};      // Camera looking at point
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;                    // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE; // Camera projection type

    Mesh cubeMesh = GenMeshCube(2.0, 4.0, 2.0);
    Model cubeModel = LoadModelFromMesh(cubeMesh);
    Vector3 cubePos = {0.0, 0.0, 0.0};

    DisableCursor();

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        UpdateCamera(&camera, CAMERA_FREE);
        cubeModel.transform =
            MatrixMultiply(MatrixRotateX(2 * PI / 180), cubeModel.transform);
        ClearBackground(BLACK);

        if (IsKeyPressed(KEY_Q)) {
            break;
        }

        BeginDrawing();
        {
            BeginMode3D(camera);
            DrawModel(cubeModel, cubePos, 1.0, RED);
            DrawModelWires(cubeModel, cubePos, 1.0, BLACK);
            EndMode3D();
        }
        EndDrawing();
    }
#endif

    CloseWindow();

    return 0;
}
