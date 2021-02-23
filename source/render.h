#ifndef RENDER_H
#define RENDER_H

#include "color.h"
#include "language.h"

struct Vertex {
    f32 x;
    f32 y;
    f32 z;
};

struct Point {
    f32 x;
    f32 y;
};

struct Triangle {
    i32 vertex_indices[3];
    Color *color;
};

struct Model {
    v3 *vertices;
    i32 vertices_count;

    Triangle *triangles;
    i32 triangles_count;
};

struct Object {
    Model *model;

    v3 scale;
    v3 rotation_axis;
    f32 angle_degrees;
    v3 translation;
};

struct Camera {
    v3 position;
    v3 rotation_axis;
    f32 angle_degrees;
};

struct Render_Buffer {
    void *memory;
    i32 width;
    i32 height;
    i32 pitch;
};

void update_and_render(Render_Buffer *buffer);

#endif  // RENDER_H