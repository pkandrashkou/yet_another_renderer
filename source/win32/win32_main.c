#include <stdlib.h>
#include <windows.h>

#include "..\image.c"
#include "..\language.h"

global f32 viewport_width = 1;
global f32 viewport_height = 1;
global i32 image_width = 1000;
global i32 image_height = 1000;
global f32 z_plane_distance = 1.0f;

global Image global_image = {1000, 1000, 3};

global Point
viewport_to_pixel(Point point) {
    Point result;

    f32 image_x = (point.x * image_width / (f32)viewport_width);
    f32 image_y = (point.y * image_height / (f32)viewport_height);
    result.x = (image_x + image_width) / 2;
    result.y = (image_y + image_height) / 2;
    return result;
}

global Point
project_vertex_to_pixel(v4 vert) {
    Point result;
    result.x = vert.x * z_plane_distance / (f32)vert.z;
    result.y = vert.y * z_plane_distance / (f32)vert.z;
    return viewport_to_pixel(result);
}

typedef struct Triangle {
    i32 vertex_indices[3];
    Color *color;
} Triangle;

typedef struct Model {
    v3 *vertices;
    i32 vertices_count;

    Triangle *triangles;
    i32 triangles_count;
} Model;

typedef struct Object {
    Model *model;

    v3 scale;
    v3 rotation_axis;
    f32 angle_degrees;
    v3 translation;
} Object;

void render_object(Image *image, Object *object) {
    Model *model = object->model;
    v3 scale = object->scale;
    v3 rotation_axis = object->rotation_axis;
    f32 angle_degrees = object->angle_degrees;
    f32 angle_radians = angle_degrees * 180 / MY_PI;
    v3 translation = object->translation;

    Point *projected_vertices = malloc(sizeof(projected_vertices) * model->vertices_count);
    for (i32 vertex_index = 0; vertex_index < model->vertices_count; vertex_index++) {
        v3 vertex = model->vertices[vertex_index];
        v4 vertex_h = v4(vertex.x, vertex.y, vertex.z, 1);

        m4 transform = m4_init_diagonal(1.0f);
        transform = m4_multiply_m4(transform, m4_scale_m3(scale));
        transform = m4_multiply_m4(transform, m4_rotate(rotation_axis, angle_radians));
        transform = m4_multiply_m4(transform, m4_translate_v3(translation));
        v4 transformed_vertex = v4_multiply_m4(vertex_h, transform);
        projected_vertices[vertex_index] = project_vertex_to_pixel(transformed_vertex);
    }

    for (i32 triangle_index = 0; triangle_index < model->triangles_count; triangle_index++) {
        Triangle triangle = model->triangles[triangle_index];
        Point p1 = projected_vertices[triangle.vertex_indices[0]];
        Point p2 = projected_vertices[triangle.vertex_indices[1]];
        Point p3 = projected_vertices[triangle.vertex_indices[2]];
        image_draw_triangle_wireframe_point(image,
                                            p1,
                                            p2,
                                            p3,
                                            *triangle.color);
    }
    free(projected_vertices);
}

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
    Image *image = &global_image;
    image->pixels = (u8 *)calloc(image->width * image->height * image->channel_number, sizeof(image->pixels));

    for (i32 y = 0; y < image->height; ++y) {
        for (i32 x = 0; x < image->width; ++x) {
            for (i32 channel = 0; channel < image->channel_number; ++channel) {
                image_set_pixel(image, x, y, color_white);
            }
        }
    }

    // image_draw_triangle_filled_shaded(image, 100, 100, 400, 250, 250, 400, color_orange, 0.0f, 0.7f, 1.0f);
    // image_draw_triangle_wireframe(image, 100, 100, 400, 250, 250, 400, color_light_yellow);

    // image_draw_triangle_filled_shaded(image, 20, 20, 100, 50, 20, 400, color_purple, 1.0f, 0.2f, 0.6f);
    // image_draw_triangle_wireframe(image, 20, 20, 100, 50, 20, 400, color_white);
    Model model;

    v3 vertices[] = {
        {1, 1, 1},
        {-1, 1, 1},
        {-1, 1, -1},
        {1, 1, -1},
        {1, -1, 1},
        {-1, -1, 1},
        {-1, -1, -1},
        {1, -1, -1}};
    model.vertices = vertices;
    model.vertices_count = ArraySize(vertices);

    Triangle triangles[] = {
        {0, 1, 2, &color_red},
        {2, 3, 0, &color_red},
        {0, 3, 7, &color_green},
        {7, 4, 0, &color_green},
        {0, 4, 5, &color_blue},
        {5, 0, 1, &color_blue},
        {1, 5, 2, &color_orange},
        {2, 5, 6, &color_orange},
        {6, 2, 3, &color_yellow},
        {3, 7, 6, &color_yellow},
        {6, 5, 7, &color_purple},
        {7, 4, 5, &color_purple}};
    model.triangles = triangles;
    model.triangles_count = ArraySize(triangles);

    // Object cube;
    // cube.model = &model;
    // cube.scale = v3(2.0f, 2.0f, 2.0f);
    // cube.rotation_axis = v3(0.0f, 1.0f, 0.0f);
    // cube.angle_degrees = 30;
    // cube.translation = v3(-1.5f, 1.0f, 7.0f);

    // render_object(image, &cube);

    Object _cube;
    _cube.model = &model;
    _cube.scale = v3(1.0f, 1.0f, 1.0f);
    _cube.rotation_axis = v3(0.0f, 1.0f, 0.0f);
    _cube.angle_degrees = 30;
    _cube.translation = v3(1.0f, 0.0f, 7.0f);

    render_object(image, &_cube);

    image_save(image, "../render.png");
    free(image->pixels);
    return 0;
}
