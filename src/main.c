/*******************************************************************************************
 *   raylib [core] example - Basic window (adapted for HTML5 platform)
 ********************************************************************************************/

#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 1200

#define DOT_ROW 100
#define DOT_COL 100

#define LIGHT_SPEED 10000.0f

#define MAX_FIELDS 1000

Camera2D camera;

typedef struct {
    Vector2 origin;
    float_t startTime;
    float_t weight; // for g field, it's GM. for e field, it's kQ
} ForceField;

ForceField *currentFields;
Vector2 (*forces)[DOT_COL];
size_t fieldSize = 0;

void UpdateDrawFrame();
int main() {
    // Initialization
    currentFields = (ForceField *)malloc(MAX_FIELDS * sizeof(ForceField));
    forces = (Vector2(*)[DOT_COL])malloc(DOT_ROW * DOT_COL * sizeof(Vector2));
    // ^ mental gymnastics!

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "basic window");

    camera.offset =
        (Vector2){(float)SCREEN_WIDTH / 2.0f, (float)SCREEN_HEIGHT / 2.0f};
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

    free(currentFields);
    free(forces);

    return 0;
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
            Vector2Scale((Vector2){(rawPos.x / (float)SCREEN_WIDTH - 0.5f) *
                                       (float)SCREEN_WIDTH,
                                   (rawPos.y / (float)SCREEN_HEIGHT - 0.5f) *
                                       (float)SCREEN_HEIGHT},
                         1.0f / camera.zoom),
            camera.target);
    } else {
        cursorPos = camera.target;
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        currentFields[fieldSize++] = (ForceField){cursorPos, ft, 1000.0f};
    }

    // start command buffer recording
    memset(forces, 0, DOT_ROW * DOT_COL * sizeof(Vector2));
    BeginDrawing();
    {
        BeginMode2D(camera);
        for (uint32_t f = 0; f < fieldSize; f++) {
            ForceField *const field = &currentFields[f];
            float delta = ft - field->startTime;
            for (uint32_t i = 0; i < DOT_ROW; i++)
                for (uint32_t j = 0; j < DOT_COL; j++) {
                    int32_t px = i * 10, py = j * 10;
                    Vector2 pos = (Vector2){(float_t)px, (float_t)py};
                    Vector2 subVec = Vector2Subtract(field->origin, pos);
                    float_t distSqr = Vector2MagnitudeSqr(subVec);
                    if (distSqr / LIGHT_SPEED > delta)
                        continue;

                    float_t calcField = Clamp(
                        (field->weight / distSqr) * 100.0f, -20.0f, 20.0f);
                    if (absf(calcField) < 2.0f)
                        continue;

                    forces[i][j] =
                        Vector2Add(Vector2Scale(Vector2Unit(subVec), calcField),
                                   forces[i][j]);
                }
        }
        for (uint32_t i = 0; i < DOT_ROW; i++)
            for (uint32_t j = 0; j < DOT_COL; j++) {
                int32_t px = i * 10, py = j * 10;
                Vector2 f = Vector2Add(forces[i][j],
                                       (Vector2){(float_t)px, (float_t)py});
                int32_t ex = (int32_t)f.x, ey = (int32_t)f.y;
                if (ex != px || ey != py) {
                    DrawLine(px, py, ex, ey, RED);
                }
            }
        EndMode2D();
        DrawFPS(0, 0);
        DrawText(TextFormat("Fields: %d", fieldSize), 0, 30, 30, GREEN);
    }
    // stop command buffer recording
    EndDrawing();
}