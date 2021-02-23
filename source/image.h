#include "color.h"

typedef struct Image {
    const i32 width;
    const i32 height;
    const i32 channel_number;
    u8 *pixels;
} Image;

void image_set_pixel(Image *image, i32 x, i32 y, Color color);
void image_draw_line(Image *image, i32 x0, i32 y0, i32 x1, i32 y1, Color color);
void image_draw_line_point(Image *image, Point p1, Point p2, Color color);
void image_draw_triangle_wireframe(Image *image, i32 x0, i32 y0, i32 x1, i32 y1, i32 x2, i32 y2, Color color);
void image_draw_triangle_wireframe_point(Image *image, Point p1, Point p2, Point p3, Color color);
void image_draw_triangle_filled(Image *image, i32 x0, i32 y0, i32 x1, i32 y1, i32 x2, i32 y2, Color fill_color);
void image_draw_triangle_filled_shaded(Image *image,
                                       i32 x0, i32 y0, i32 x1, i32 y1, i32 x2, i32 y2,
                                       Color fill_color,
                                       f32 h0, f32 h1, f32 h2);
void image_save(Image *image, char const *filename);