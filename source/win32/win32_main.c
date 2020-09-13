#include <stdlib.h>
#include <windows.h>

#include "..\language.h"
#include "..\image.c"

global f32 viewport_width = 1;
global f32 viewport_height = 1;
global i32 image_width = 499;
global i32 image_height = 499;
global f32 z_plane_distance = 1.0f;

global Image global_image = {500, 500, 3};

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
    result.x = vert.x * z_plane_distance / (f32) vert.z;
    result.y = vert.y * z_plane_distance / (f32) vert.z;
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
    for(i32 vertex_index = 0; vertex_index < model->vertices_count; vertex_index++) {
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
			*triangle.color
		);
	}
    free(projected_vertices);
}

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
    Image *image = &global_image;
    image->pixels = (u8 *)calloc(image->width * image->height * image->channel_number, sizeof(image->pixels));

    // image_draw_triangle_filled_shaded(image, 100, 100, 400, 250, 250, 400, color_orange, 0.0f, 0.7f, 1.0f);
    // image_draw_triangle_wireframe(image, 100, 100, 400, 250, 250, 400, color_light_yellow);

    // image_draw_triangle_filled_shaded(image, 20, 20, 100, 50, 20, 400, color_purple, 1.0f, 0.2f, 0.6f);
    // image_draw_triangle_wireframe(image, 20, 20, 100, 50, 20, 400, color_white);
    Model model;
    
    v3 vertices[] = {
        { 1,  1,  1},
        {-1,  1,  1},
        {-1,  1, -1},
        { 1,  1, -1},
        { 1, -1,  1},
        {-1, -1,  1},
        {-1, -1, -1},
        { 1, -1, -1}
    };
    model.vertices = vertices;
    model.vertices_count = ArraySize(vertices);

    Triangle triangles[] = {
        {0, 1, 2, &color_red },
        {2, 3, 0, &color_red },
        {0, 3, 7, &color_green },
        {7, 4, 0, &color_green },
        {0, 4, 5, &color_blue },
        {5, 0, 1, &color_blue },
        {1, 5, 2, &color_orange },
        {2, 5, 6, &color_orange },
        {6, 2, 3, &color_yellow },
        {3, 7, 6, &color_yellow },
        {6, 5, 7, &color_purple },
        {7, 4, 5, &color_purple }
    };
    model.triangles = triangles;
    model.triangles_count = ArraySize(triangles);

    Object cube;
    cube.model = &model;
    cube.scale = v3(2.0f, 2.0f, 2.0f);
    cube.rotation_axis = v3(0.0f, 1.0f, 0.0f);
    cube.angle_degrees = 30;
    cube.translation = v3(1.5f, 1.0f, 7.0f);

    render_object(image, &cube);

    // Object other_cube;
    // other_cube.model = &model;
    // v3 other_cube_position = { -0.8f, 0.0f, 8.0f };
    // other_cube.position = other_cube_position;
    //  render_object(image, &other_cube);

     m4 matrix = {
         {
            {0,   1,  2,  3},
            {4,   5,  6,  7},
            {8,   9, 10, 11},
            {12, 13, 14, 15}
         }
     };

    m4 matrix_2 = {
         {
            {22,  32,  42,  52},
            {2,   5,  1,  -3},
            {12,   58, -4, 100},
            {2, 6, -156, 37}
         }
     };
     m4 result1 = m4_multiply_m4(matrix, matrix_2);

    // Point a = project_vertex_to_pixel(vertices[0]);
    // Point b = project_vertex_to_pixel(vertices[1]);
    // Point c = project_vertex_to_pixel(vertices[2]);
    // Point d = project_vertex_to_pixel(vertices[3]);

    // Point e = project_vertex_to_pixel(vertices[4]);
    // Point f = project_vertex_to_pixel(vertices[5]);
    // Point g = project_vertex_to_pixel(vertices[6]);
    // Point k = project_vertex_to_pixel(vertices[7]);

    // image_draw_line_point(image, project_vertex_to_pixel(vertices[0]), project_vertex_to_pixel(vertices[1]), color_purple);
    // image_draw_line_point(image, project_vertex_to_pixel(vertices[1]), project_vertex_to_pixel(vertices[2]), color_purple);
    // image_draw_line_point(image, project_vertex_to_pixel(vertices[2]), project_vertex_to_pixel(vertices[3]), color_purple);
    // image_draw_line_point(image, project_vertex_to_pixel(vertices[3]), project_vertex_to_pixel(vertices[0]), color_purple);

    // image_draw_line_point(image, project_vertex_to_pixel(vertices[4]), project_vertex_to_pixel(vertices[5]), color_orange);
    // image_draw_line_point(image, project_vertex_to_pixel(vertices[5]), project_vertex_to_pixel(vertices[6]), color_orange);
    // image_draw_line_point(image, project_vertex_to_pixel(vertices[6]), project_vertex_to_pixel(vertices[7]), color_orange);
    // image_draw_line_point(image, project_vertex_to_pixel(vertices[7]), project_vertex_to_pixel(vertices[4]), color_orange);

    // image_draw_line_point(image, project_vertex_to_pixel(vertices[0]), project_vertex_to_pixel(vertices[4]), color_red);
    // image_draw_line_point(image, project_vertex_to_pixel(vertices[1]), project_vertex_to_pixel(vertices[5]), color_red);
    // image_draw_line_point(image, project_vertex_to_pixel(vertices[2]), project_vertex_to_pixel(vertices[6]), color_red);
    // image_draw_line_point(image, project_vertex_to_pixel(vertices[3]), project_vertex_to_pixel(vertices[7]), color_red);

    image_save(image, "../render.png");
    free(image->pixels);
    return 0;
}
