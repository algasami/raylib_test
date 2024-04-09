/*******************************************************************************************
 *   raylib [core] example - Basic window (adapted for HTML5 platform)
 ********************************************************************************************/

#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#define SCREEN_WIDTH 1500
#define SCREEN_HEIGHT 1000

#define DOT_ROW 100
#define DOT_COL 100

#define BORDER_RADIUS 800.0f

#define SPAWN_RADIUS 500.0f

#define LIGHT_SPEED 10000.0f

#define MAX_OBJECTS 50
Camera2D camera;

typedef struct {
    Vector2 pos, vel, acc;
    float_t mass, radius;
    Color color;
} Object2D;

Object2D objects[MAX_OBJECTS];
size_t object_count = MAX_OBJECTS;
size_t focus_id = 0;
uint32_t b_focused = 0, b_slowmo = 1;

void UpdateDrawFrame();
int main() {
    srand((uint32_t)time(NULL));
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "basic window");

    camera.offset =
        (Vector2){(float)SCREEN_WIDTH / 2.0f, (float)SCREEN_HEIGHT / 2.0f};
    camera.rotation = 0.0f;
    camera.target = (Vector2){0.0f, 0.0f};
    camera.zoom = 2.0f;

    for (size_t i = 0; i < MAX_OBJECTS; i++) {
        float_t rx = ((float_t)rand() / (float_t)RAND_MAX);
        float_t ry = ((float_t)rand() / (float_t)RAND_MAX);
        objects[i].acc = (Vector2){0.0f, 0.0f};
        objects[i].pos = (Vector2){rx * 2.0f * SPAWN_RADIUS - SPAWN_RADIUS,
                                   ry * 2.0f * SPAWN_RADIUS - SPAWN_RADIUS};
        objects[i].vel = (Vector2){0.0f, 0.0f};
        objects[i].mass = Clamp(((float_t)rand() / (float_t)RAND_MAX) * 2000.0f,
                                100.0f, 10000.0f);
        objects[i].radius = objects[i].mass * 0.01f;
        objects[i].color =
            (Color){(uint8_t)(rx * 255.0f), (uint8_t)(ry * 255.0f), 100, 255};
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

void CalculatePhysics(float_t dt) {
    if (b_slowmo) {
        dt = dt / 50.0f;
    }
    for (size_t i = 0; i < object_count; i++) {
        objects[i].acc = (Vector2){0.0f, 0.0f};
    }

    for (size_t i = 0; i < object_count; i++) {
        Object2D *const a = objects + i;
        for (size_t j = i + 1; j < object_count; j++) {
            Object2D *const b = objects + j;

            Vector2 vab = Vector2Subtract(b->pos, a->pos);
            float_t magsqr = Vector2MagnitudeSqr(vab);
            if (sqrtf(magsqr) < a->radius + b->radius) {
                continue;
            }
            float_t force_mag = (a->mass * b->mass) * 100.0f / magsqr;

            a->acc = Vector2Add(
                a->acc, Vector2Scale(Vector2Unit(vab), force_mag / a->mass));
            b->acc = Vector2Add(
                b->acc, Vector2Scale(Vector2Unit(vab), -force_mag / b->mass));
        }
    }

    for (size_t i = 0; i < object_count; i++) {

        // TODO: This elastic collision is inaccurate! Fix!
        if (Vector2Distance(objects[i].pos, (Vector2){0.0f, 0.0f}) >
            BORDER_RADIUS) {
            objects[i].vel = Vector2Scale(objects[i].vel, -1.0f);
        }
        objects[i].acc =
            Vector2Add(objects[i].acc,
                       Vector2Scale(objects[i].vel, -0.02f * objects[i].radius *
                                                        objects[i].radius));
        objects[i].pos = Vector2Add(
            objects[i].pos,
            Vector2Add(Vector2Scale(objects[i].vel, dt),
                       Vector2Scale(objects[i].acc, 0.5f * dt * dt)));
        objects[i].vel =
            Vector2Add(objects[i].vel, Vector2Scale(objects[i].acc, dt));
    }
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
    } else if (b_focused) {
        camera.target = objects[focus_id].pos;
    }

    if (IsKeyPressed(KEY_T)) {
        b_slowmo = !b_slowmo;
    }

    if (IsKeyPressed(KEY_LEFT)) {
        focus_id = focus_id ? focus_id - 1 : MAX_OBJECTS - 1;
        b_focused = 1;
    } else if (IsKeyPressed(KEY_RIGHT)) {
        focus_id = (focus_id + 1) % MAX_OBJECTS;
        b_focused = 1;
    }

    camera.zoom =
        Clamp(camera.zoom + GetMouseWheelMove() * 300.0f * dt, 2.0f, 1000.0f);

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
        if (b_focused) {
            b_focused = 0;
        } else {
            for (size_t i = 0; i < MAX_OBJECTS; i++) {
                float_t dist = Vector2Distance(cursorPos, objects[i].pos);
                if (dist < objects[i].radius) {
                    focus_id = i;
                    b_focused = 1;
                    break;
                }
            }
        }
    }

    CalculatePhysics(dt);
    BeginDrawing();
    {
        BeginMode2D(camera);
        for (size_t i = 0; i < object_count; i++) {
            DrawCircle((int32_t)objects[i].pos.x, (int32_t)objects[i].pos.y,
                       objects[i].radius, objects[i].color);
        }
        DrawCircleLines(0, 0, BORDER_RADIUS, WHITE);
        EndMode2D();
        DrawFPS(0, 0);
        if (b_focused)
            DrawText(TextFormat("Focused: %u", focus_id), 0, 30, 30, GREEN);
        if (b_slowmo)
            DrawText("Slowmo Active", 0, 60, 30, GREEN);
    }
    // stop command buffer recording
    EndDrawing();
}