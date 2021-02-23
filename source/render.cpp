

// #include "render.h"

#include "render.h"

#include <stdlib.h>  // malloc. TODO: get rid of maloc?

#include "color.h"
#include "language.h"
#include "math_utils.h"

global f32 viewport_width = 1;
global f32 viewport_height = 1;
global f32 z_plane_distance = 1.0f;

internal Point
viewport_to_pixel(Point point, i32 width, i32 height) {
    Point result;

    f32 image_x = (point.x * width / (f32)viewport_width);
    f32 image_y = (point.y * height / (f32)viewport_height);
    result.x = (image_x + width) / 2;
    result.y = (image_y + height) / 2;
    return result;
}

internal Point
project_vertex_to_pixel(v4 vert, i32 width, i32 height) {
    Point result;
    result.x = vert.x * z_plane_distance / (f32)vert.z;
    result.y = vert.y * z_plane_distance / (f32)vert.z;
    return viewport_to_pixel(result, width, height);
}

internal void render_set_pixel(Render_Buffer *image, i32 x, i32 y, Color color) {
    if (x >= image->width || y >= image->height)
        return;
    i32 channel_number = 4;
    i32 index = (y * image->width + x) * channel_number;

    if (index < 0)
        return;

    u8 *pixels = (u8 *)image->memory;

    pixels[index++] = color.b;
    pixels[index++] = color.g;
    pixels[index++] = color.r;
}

internal void clear_screen(Render_Buffer *image, Color clear_color) {
    for (i32 y = 0; y < image->height; ++y) {
        for (i32 x = 0; x < image->width; ++x) {
            render_set_pixel(image, x, y, clear_color);
        }
    }
}

internal void image_draw_line(Render_Buffer *image, i32 x0, i32 y0, i32 x1, i32 y1, Color color) {
    if (abs_i32(x1 - x0) > abs_i32(y1 - y0)) {
        if (x0 > x1) {
            swap_i32(&x0, &x1);
            swap_i32(&y0, &y1);
        }

        i32 *y_values = (i32 *)malloc(sizeof(y_values) * (x1 - x0 + 1));
        interpolate_i32(x0, y0, x1, y1, y_values);
        for (i32 x = x0; x <= x1; x += 1) {
            render_set_pixel(image, (i32)x, y_values[x - x0], color);
        }
        free(y_values);
    } else {
        if (y0 > y1) {
            swap_i32(&x0, &x1);
            swap_i32(&y0, &y1);
        }

        i32 *x_values = (i32 *)malloc(sizeof(x_values) * (y1 - y0 + 1));
        interpolate_i32(y0, x0, y1, x1, x_values);
        for (i32 y = y0; y <= y1; y += 1) {
            render_set_pixel(image, x_values[y - y0], (i32)y, color);
        }
        free(x_values);
    }
}

internal void image_draw_triangle_wireframe(Render_Buffer *image, i32 x0, i32 y0, i32 x1, i32 y1, i32 x2, i32 y2, Color color) {
    image_draw_line(image, x0, y0, x1, y1, color);
    image_draw_line(image, x1, y1, x2, y2, color);
    image_draw_line(image, x2, y2, x0, y0, color);
}

internal void image_draw_triangle_filled_shaded(Render_Buffer *image,
                                                i32 x0, i32 y0, i32 x1, i32 y1, i32 x2, i32 y2,
                                                Color fill_color,
                                                f32 h0, f32 h1, f32 h2) {
    // sort points. y0 < y1 < y2
    if (y1 < y0) {
        swap_i32(&y1, &y0);
        swap_i32(&x1, &x0);
    }
    if (y2 < y0) {
        swap_i32(&y2, &y0);
        swap_i32(&x2, &x0);
    }
    if (y2 < y1) {
        swap_i32(&y2, &y1);
        swap_i32(&x2, &x1);
    }

    i32 x01_length = y1 - y0 + 1;
    i32 x12_length = y2 - y1 + 1;
    i32 x012_length = x01_length + x12_length - 1;  // NOTE: -1 substracts the duplicate x1 value

    i32 *x012 = (i32 *)malloc(sizeof(x012) * x012_length);
    f32 *color_intensity_012 = (f32 *)malloc(sizeof(color_intensity_012) * x012_length);

    i32 *x01 = x012;
    f32 *color_intensity_01 = color_intensity_012;
    interpolate_i32(y0, x0, y1, x1, x012);
    interpolate_f32(y0, h0, y1, h1, color_intensity_01);

    i32 *x12 = x01 + x01_length - 1;
    f32 *color_intensity_12 = color_intensity_012 + x01_length - 1;
    interpolate_i32(y1, x1, y2, x2, x12);
    interpolate_f32(y1, h1, y2, h2, color_intensity_12);

    i32 x02_length = y2 - y0 + 1;
    i32 *x02 = (i32 *)malloc(sizeof(x02) * x02_length);
    f32 *color_intensity_02 = (f32 *)malloc(sizeof(color_intensity_02) * x02_length);
    interpolate_i32(y0, x0, y2, x2, x02);
    interpolate_f32(y0, h0, y2, h2, color_intensity_02);

    i32 *x_left;
    i32 *x_right;
    f32 *intensity_left;
    f32 *intensity_right;

    i32 middle = x02_length / 2;
    if (x02[middle] < x012[middle]) {
        x_left = x02;
        x_right = x012;

        intensity_left = color_intensity_02;
        intensity_right = color_intensity_012;
    } else {
        x_left = x012;
        x_right = x02;

        intensity_left = color_intensity_012;
        intensity_right = color_intensity_02;
    }

    for (i32 y = y0; y <= y2; y++) {
        i32 x_left_value = x_left[y - y0];
        i32 x_right_value = x_right[y - y0];

        i32 intensity_segment_length = x_right_value - x_left_value + 1;
        f32 *intensity_segment = (f32 *)malloc(sizeof(intensity_segment) * intensity_segment_length);
        interpolate_f32(x_left_value, intensity_left[y - y0], x_right_value, intensity_right[y - y0], intensity_segment);

        for (i32 x = x_left_value; x <= x_right_value; x++) {
            f32 color_intensity = intensity_segment[x - x_left_value];
            Color shaded_color = fill_color;
            shaded_color.r = (u8)(shaded_color.r * color_intensity);
            shaded_color.g = (u8)(shaded_color.g * color_intensity);
            shaded_color.b = (u8)(shaded_color.b * color_intensity);

            render_set_pixel(image, x, y, shaded_color);
        }
        free(intensity_segment);
    }

    free(x012);
    free(color_intensity_012);
    free(x02);
    free(color_intensity_02);
}

internal void render_object(Render_Buffer *image, Object *object, m4 camera_transform) {
    Model *model = object->model;
    v3 scale = object->scale;
    v3 rotation_axis = object->rotation_axis;
    f32 angle_degrees = object->angle_degrees;
    f32 angle_radians = angle_degrees * MY_PI / 180.0f;
    v3 translation = object->translation;

    Point *projected_vertices = (Point *)malloc(sizeof(projected_vertices) * model->vertices_count);
    for (i32 vertex_index = 0; vertex_index < model->vertices_count; vertex_index++) {
        v3 vertex = model->vertices[vertex_index];
        v4 vertex_h = v4(vertex.x, vertex.y, vertex.z, 1);

        m4 transform = m4_init_diagonal(1.0f);
        transform = m4_multiply_m4(transform, m4_scale_m3(scale));
        transform = m4_multiply_m4(transform, m4_rotate(rotation_axis, angle_radians));
        transform = m4_multiply_m4(transform, m4_translate_v3(translation));

        transform = m4_multiply_m4(transform, camera_transform);

        v4 transformed_vertex = v4_multiply_m4(vertex_h, transform);
        projected_vertices[vertex_index] = project_vertex_to_pixel(transformed_vertex, image->width, image->height);
    }

    for (i32 triangle_index = 0; triangle_index < model->triangles_count; triangle_index++) {
        Triangle triangle = model->triangles[triangle_index];
        Point p1 = projected_vertices[triangle.vertex_indices[0]];
        Point p2 = projected_vertices[triangle.vertex_indices[1]];
        Point p3 = projected_vertices[triangle.vertex_indices[2]];
        image_draw_triangle_wireframe(image, (i32)p1.x, (i32)p1.y, (i32)p2.x, (i32)p2.y, (i32)p3.x, (i32)p3.y, *triangle.color);
    }
    free(projected_vertices);
}

internal void render_scene(Render_Buffer *image, Camera camera, Object *objects, i32 objects_count) {
    f32 camera_angle_radians = camera.angle_degrees * MY_PI / 180.0f;
    m4 camera_matrix = m4_init_diagonal(1.0f);
    camera_matrix = m4_multiply_m4(camera_matrix, m4_translate_v3(v3_multiply_scalar(camera.position, -1)));
    camera_matrix = m4_multiply_m4(camera_matrix, m4_transposed(m4_rotate(camera.rotation_axis, camera_angle_radians)));

    for (i32 i = 0; i < objects_count; i++) {
        Object object = objects[i];
        render_object(image, &object, camera_matrix);
    }
}

void update_and_render(Render_Buffer *buffer) {
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
    v3 rotation_axis = v3(0.0f, 0.0f, 1.0f);
    m4 camera_matrix = m4_rotate(rotation_axis, 0);

    Object _cube;
    _cube.model = &model;
    _cube.scale = v3(1.0f, 1.0f, 1.0f);
    _cube.rotation_axis = v3(0.0f, 1.0f, 0.0f);
    _cube.angle_degrees = 0;
    _cube.translation = v3(0.0f, 0.0f, 4.0f);

    Camera camera;
    camera.position = v3(0.0f, 0.0f, 0.0f);
    camera.rotation_axis = v3(0.0f, 0.0f, 1.0f);
    camera.angle_degrees = 20;

    clear_screen(buffer, color_white);
    render_scene(buffer, camera, &_cube, 1);
}