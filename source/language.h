#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <stdint.h>
#include <math.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

#define global static
#define internal static
#define local_persist static

#define ArraySize(array) (sizeof(array)) / sizeof((array)[0])

typedef union v3 {
    struct {
        f32 x;
        f32 y;
        f32 z;
    };
    
    f32 elements[3];
} v3;

internal v3
v3_init(f32 x, f32 y, f32 z) {
    v3 v = { x, y, z };
    return v;
}
#define v3(x, y, z) v3_init(x, y, z)

internal v3
v3_add_v3(v3 a, v3 b) {
    v3 c = { a.x + b.x, a.y + b.y, a.z + b.z };
    return c;
}

internal v3
v3_multiply_scalar(v3 a, f32 b) {
    v3 result = { a.x * b, a.y * b, a.z * b};
    return result;
}

internal f32
v3_length_squared(v3 v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

internal f32
v3_length(v3 v) {
    return sqrtf(v3_length_squared(v));
}

internal v3
v3_normalized(v3 v) {
    f32 length = v3_length(v);
    v3 result = {
        v.x / length,
        v.y / length,
        v.z / length
    };
    return result;
}

typedef union v4 {
    struct {
        f32 x;
        f32 y;
        f32 z;
        f32 w;
    };

    f32 elements[4];
} v4;


internal v4
v4_init(f32 x, f32 y, f32 z, f32 w) {
    v4 v = { x, y, z, w };
    return v;
}
#define v4(x, y, z, w) v4_init(x, y, z, w)

typedef struct m4 {
    f32 elements[4][4];
} m4;

internal m4
m4_init_diagonal(f32 diagonal) {
    m4 m = {
        {
            { diagonal },
            { 0.0f, diagonal },
            { 0.0f, 0.0f, diagonal },
            { 0.0f, 0.0f, 0.0f, diagonal }
        }
    };
    return m;
}

// Pre multiply. Row-Major.
internal m4
m4_multiply_m4(m4 a, m4 b) {
    m4 c = {0};
    
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            c.elements[i][j] = (a.elements[i][0] * b.elements[0][j] +
                                a.elements[i][1] * b.elements[1][j] +
                                a.elements[i][2] * b.elements[2][j] +
                                a.elements[i][3] * b.elements[3][j]);
        }
    }
    
    return c;
}

// Angle in radians.
internal m4
m4_rotate(v3 axis, f32 angle) {
    m4 result = m4_init_diagonal(1.0f);
    axis = v3_normalized(axis);

    f32 sin_theta = sinf(angle);
    f32 cos_theta = cosf(angle);
    f32 cos_value = 1.0f - cos_theta;

    result.elements[0][0] = axis.x * axis.x * cos_value + cos_theta;
    result.elements[0][1] = axis.y * axis.x * cos_value - axis.z * sin_theta;
    result.elements[0][2] = axis.z * axis.x * cos_value + axis.y * sin_theta;

    result.elements[1][0] = axis.x * axis.y * cos_value + axis.z * sin_theta;
    result.elements[1][1] = axis.y * axis.y * cos_value + cos_theta;
    result.elements[1][2] = axis.z * axis.y * cos_value - axis.x * sin_theta;

    result.elements[2][0] = axis.x * axis.z * cos_value - axis.y * sin_theta;
    result.elements[2][1] = axis.y * axis.z * cos_value + axis.x * sin_theta;
    result.elements[2][2] = axis.z * axis.z * cos_value + cos_theta;

    return result;
}

internal m4
m4_translate_v3(v3 translation) {
    m4 result = m4_init_diagonal(1.0f);
    result.elements[3][0] = translation.x;
    result.elements[3][1] = translation.y;
    result.elements[3][2] = translation.z;
    return result;
}

internal m4
m4_scale_m3(v3 scale) {
    m4 result = m4_init_diagonal(1.0f);
    result.elements[0][0] = scale.x;
    result.elements[1][1] = scale.y;
    result.elements[2][2] = scale.z;
    return result;
}

internal m4
m4_transposed(m4 m) {
    m4 result = m4_init_diagonal(1.0f);
    for (i32 i = 0; i < 4; i++) {
        for (i32 j = 0; j < 4; j++) {
            result.elements[i][j] = m.elements[j][i];
        }
    }
    return result;
}

// Pre multiply. Row-Major.
internal v4
v4_multiply_m4(v4 v, m4 m) {
    v4 result = {0};
    
    for(int i = 0; i < 4; i++) {
        result.elements[i] = (v.elements[0] * m.elements[0][i] +
                              v.elements[1] * m.elements[1][i] +
                              v.elements[2] * m.elements[2][i] +
                              v.elements[3] * m.elements[3][i]);
    }
    
    return result;
}

#endif /* LANGUAGE_H */