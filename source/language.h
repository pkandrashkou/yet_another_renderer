#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <stdint.h>

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

typedef union v3 {
    struct {
        f32 x;
        f32 y;
        f32 z;
    };
    
    struct {
        f32 r;
        f32 g;
        f32 b;
    };
    
    f32 elements[3];
} v3;

internal v3
v3_add_v3(v3 a, v3 b)
{
    v3 c = { a.x + b.x, a.y + b.y, a.z + b.z };
    return c;
}

#endif /* LANGUAGE_H */