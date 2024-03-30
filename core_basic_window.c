/*******************************************************************************************
 *   raylib [core] example - Basic window (adapted for HTML5 platform)
 ********************************************************************************************/

#include "raylib.h"
#include "raymath.h"
#include <stdint.h>
#include <stdio.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#define NOISE_ROW 30
#define NOISE_COL 30

uint32_t screenWidth = 1200;
uint32_t screenHeight = 1200;

Camera3D camera;

Mesh cubeMesh;
Model cubeModels[NOISE_ROW][NOISE_COL];

void UpdateDrawFrame();
int main() {
    // Initialization
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "basic window");

    camera.fovy = 45.0f;
    camera.position = (Vector3){1.0f, 10.0f, 1.0f};
    camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    camera.projection = CAMERA_PERSPECTIVE;
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};

    cubeMesh = GenMeshCube(1.0f, 5.0f, 1.0f);

    for (size_t i = 0; i < NOISE_ROW; i++)
        for (size_t j = 0; j < NOISE_COL; j++) {
            cubeModels[i][j] = LoadModelFromMesh(cubeMesh);
        }

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

void UpdateDrawFrame() {
    float dt = GetFrameTime();
    float ft = (float)GetTime();

    if (IsCursorHidden())
        UpdateCamera(&camera, CAMERA_FREE);

    Image noiseImage = GenImagePerlinNoise(NOISE_COL, NOISE_ROW,
                                           (uint32_t)(ft * 20.0f), 0, 1.0f);

    ClearBackground(BLACK);

    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        DisableCursor();
    } else if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
        EnableCursor();
    }

    // start command buffer recording
    BeginDrawing();
    {
        BeginMode3D(camera);
        for (size_t i = 0; i < NOISE_ROW; i++)
            for (size_t j = 0; j < NOISE_COL; j++) {
                Color colorData = ((Color *)noiseImage.data)[i * NOISE_COL + j];
                float height = (float)colorData.r / 255.0f;
                DrawModel(cubeModels[i][j],
                          (Vector3){(float)i, height * 10.0f, (float)j}, 1.0f,
                          ColorFromHSV(170.0f, 1.0f, height));
            }
        EndMode3D();
        DrawFPS(0, 0);
    }
    // stop command buffer recording
    EndDrawing();
}