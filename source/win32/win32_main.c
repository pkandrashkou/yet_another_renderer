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
project_vertex_to_pixel(Vertex vert) {
    Point result;
    result.x = vert.x * z_plane_distance / (f32) vert.z;
    result.y = vert.y * z_plane_distance / (f32) vert.z;
    return viewport_to_pixel(result);
}

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
    Image *image = &global_image;
    image->pixels = (u8 *)calloc(image->width * image->height * image->channel_number, sizeof(image->pixels));

    // image_draw_triangle_filled_shaded(image, 100, 100, 400, 250, 250, 400, color_orange, 0.0f, 0.7f, 1.0f);
    // image_draw_triangle_wireframe(image, 100, 100, 400, 250, 250, 400, color_light_yellow);

    // image_draw_triangle_filled_shaded(image, 20, 20, 100, 50, 20, 400, color_purple, 1.0f, 0.2f, 0.6f);
    // image_draw_triangle_wireframe(image, 20, 20, 100, 50, 20, 400, color_white);

    Vertex vertices[8] = {
        {-1.5, -0.5f, 2},
        {-1.5, 0.5f, 2},
        {-1, 0.5f, 2},
        {-1, -0.5f, 2},

        {-1.2f, -0.5f, 3},
        {-1.2f, 0.5f, 3},
        {-0.6f, 0.5f, 3},
        {-0.6f, -0.5f, 3}
    };

    // Point a = project_vertex_to_pixel(vertices[0]);
    // Point b = project_vertex_to_pixel(vertices[1]);
    // Point c = project_vertex_to_pixel(vertices[2]);
    // Point d = project_vertex_to_pixel(vertices[3]);

    // Point e = project_vertex_to_pixel(vertices[4]);
    // Point f = project_vertex_to_pixel(vertices[5]);
    // Point g = project_vertex_to_pixel(vertices[6]);
    // Point k = project_vertex_to_pixel(vertices[7]);

    image_draw_line_point(image, project_vertex_to_pixel(vertices[0]), project_vertex_to_pixel(vertices[1]), color_purple);
    image_draw_line_point(image, project_vertex_to_pixel(vertices[1]), project_vertex_to_pixel(vertices[2]), color_purple);
    image_draw_line_point(image, project_vertex_to_pixel(vertices[2]), project_vertex_to_pixel(vertices[3]), color_purple);
    image_draw_line_point(image, project_vertex_to_pixel(vertices[3]), project_vertex_to_pixel(vertices[0]), color_purple);

    image_draw_line_point(image, project_vertex_to_pixel(vertices[4]), project_vertex_to_pixel(vertices[5]), color_orange);
    image_draw_line_point(image, project_vertex_to_pixel(vertices[5]), project_vertex_to_pixel(vertices[6]), color_orange);
    image_draw_line_point(image, project_vertex_to_pixel(vertices[6]), project_vertex_to_pixel(vertices[7]), color_orange);
    image_draw_line_point(image, project_vertex_to_pixel(vertices[7]), project_vertex_to_pixel(vertices[4]), color_orange);

    image_draw_line_point(image, project_vertex_to_pixel(vertices[0]), project_vertex_to_pixel(vertices[4]), color_red);
    image_draw_line_point(image, project_vertex_to_pixel(vertices[1]), project_vertex_to_pixel(vertices[5]), color_red);
    image_draw_line_point(image, project_vertex_to_pixel(vertices[2]), project_vertex_to_pixel(vertices[6]), color_red);
    image_draw_line_point(image, project_vertex_to_pixel(vertices[3]), project_vertex_to_pixel(vertices[7]), color_red);

    image_save(image, "../render.png");
    free(image->pixels);
    return 0;
}
