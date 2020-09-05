typedef struct Image {
    const i32 width;
    const i32 height;
    const i32 channel_number;
    u8 *pixels;
} Image;

typedef struct Color {
    u8 r, g, b;
} Color;
global Color color_white = {255, 255, 255};
global Color color_red = {255, 0, 0};
global Color color_orange = {225, 95, 65};
global Color color_purple = {87, 75, 144};
global Color color_light_yellow = {247, 215, 148};

void image_set_pixel(Image *image, i32 x, i32 y, Color color);
void image_draw_line(Image *image, i32 x0, i32 y0, i32 x1, i32 y1, Color color);
void image_draw_triangle_wireframe(Image *image, i32 x0, i32 y0, i32 x1, i32 y1, i32 x2, i32 y2, Color color);
void image_draw_triangle_filled(Image *image, i32 x0, i32 y0, i32 x1, i32 y1, i32 x2, i32 y2, Color fill_color);
void image_draw_triangle_filled_shaded(Image *image,
                                       i32 x0, i32 y0, i32 x1, i32 y1, i32 x2, i32 y2,
                                       Color fill_color,
                                       f32 h0, f32 h1, f32 h2);
void image_save(Image *image, char const *filename);