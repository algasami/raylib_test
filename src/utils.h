#pragma once

#include <raylib.h>
#include <raymath.h>
#include <stdint.h>

extern inline float_t Vector2MagnitudeSqr(Vector2 const x) { return x.x * x.x + x.y * x.y; }
extern inline float_t Vector2Magnitude(Vector2 const x) { return sqrtf(x.x * x.x + x.y * x.y); }

/**
 * @brief A function that returns unit vector. If the magnitude is zero, it returns the vector unchanged.
 * @param x target vector
 */
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

/**
 * @brief This separates the x vector into a normal projection and a tangent one
 * @param base the normal vector to project to
 * @param x the target vector
 * @param norm_out the address of normal out
 * @param tan_out the address of tangent out
 */
extern inline void Vector2Separate(Vector2 const base, Vector2 const x, Vector2 *const norm_out,
                                   Vector2 *const tan_out) {
    *norm_out = Vector2Project(base, x);
    *tan_out = Vector2Subtract(x, *norm_out);
}