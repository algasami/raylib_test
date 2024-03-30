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

uint32_t screenWidth = 1200;
uint32_t screenHeight = 1200;

Camera2D camera;

inline float Vector2Magnitude(Vector2 const x) {
    return sqrtf(x.x * x.x + x.y * x.y);
}

inline Vector2 Vector2Unit(Vector2 const x) {
    float mag = Vector2Magnitude(x);
    if (mag == 0.0f)
        return (Vector2){0.0f, 0.0f};
    return (Vector2){x.x / mag, x.y / mag};
}

void UpdateDrawFrame();
int main() {
    // Initialization
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "basic window");

    camera.offset =
        (Vector2){(float)screenWidth / 2.0f, (float)screenHeight / 2.0f};
    camera.rotation = 0.0f;
    camera.target = (Vector2){0.0f, 0.0f};
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

void UpdateDrawFrame() {
    float dt = GetFrameTime();
    float ft = (float)GetTime();

    ClearBackground(BLACK);

    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        DisableCursor();
    } else if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
        EnableCursor();
    }

    if (IsCursorHidden()) {
        camera.target =
            Vector2Add(Vector2Scale(GetMouseDelta(), 100.0f * dt / camera.zoom),
                       camera.target);
    }
    camera.zoom =
        Clamp(camera.zoom + GetMouseWheelMove() * 300.0f * dt, 2.0f, 100.0f);

    // This converts our mouse cursor (local relative to window)
    // to absolute coord with camera
    Vector2 cursorPos;
    if (!IsCursorHidden()) {
        Vector2 rawPos = GetMousePosition();
        cursorPos = Vector2Add(
            Vector2Scale((Vector2){(rawPos.x / (float)screenWidth - 0.5f) *
                                       (float)screenWidth,
                                   (rawPos.y / (float)screenHeight - 0.5f) *
                                       (float)screenHeight},
                         1.0f / camera.zoom),
            camera.target);
    } else {
        cursorPos = camera.target;
    }

    // start command buffer recording
    BeginDrawing();
    {
        BeginMode2D(camera);
        for (uint32_t i = 0; i < 100; i++)
            for (uint32_t j = 0; j < 100; j++) {
                Vector2 pos = (Vector2){(float_t)i * 10.0f, (float_t)j * 10.0f};
                float dist = Vector2DistanceSqr(pos, cursorPos);
                DrawRectangle(
                    (uint32_t)pos.x, (uint32_t)pos.y, 8, 8,
                    ColorFromHSV(0.0f, 1.0f,
                                 Clamp(10000.0f / dist, 0.0f, 100.0f) /
                                     100.0f));
            }
        DrawFPS(0, 0);
        EndMode2D();
        // stop command buffer recording
        EndDrawing();
    }
}