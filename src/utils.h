#pragma once

#include <raylib.h>
#include <raymath.h>
#include <stdint.h>

extern inline float_t Vector2MagnitudeSqr(Vector2 const x) { return x.x * x.x + x.y * x.y; }
extern inline float_t Vector2Magnitude(Vector2 const x) { return sqrtf(x.x * x.x + x.y * x.y); }

extern inline Vector2 Vector2Unit(Vector2 const x) {
    float_t mag = Vector2Magnitude(x);
    if (mag == 0.0f)
        return x;
    return (Vector2){x.x / mag, x.y / mag};
}

extern inline float_t absf(float_t x) { return x < 0.0f ? -x : x; }
extern inline Vector2 Vector2Project(Vector2 const base, Vector2 const x) {
    return Vector2Scale(base, Vector2DotProduct(base, x) / Vector2MagnitudeSqr(base));
}