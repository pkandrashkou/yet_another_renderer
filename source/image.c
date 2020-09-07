#include "image.h"

#include "language.h"
#include "math_utils.h"

#define STB_IMAGE_IMPLEMENTATION
#include "vendor\stb_image\stb_image.h"

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "vendor\stb_image\stb_image_write.h"

void image_set_pixel(Image *image, i32 x, i32 y, Color color) {
    if (x >= image->width || y >= image->height)
        return;
    i32 index = (y * image->width + x) * image->channel_number;

    if (index < 0)
        return;

    image->pixels[index++] = color.r;
    image->pixels[index++] = color.g;
    image->pixels[index++] = color.b;
}

void image_draw_line_point(Image *image, Point p1, Point p2, Color color) {
    image_draw_line(image, (i32)p1.x, (i32)p1.y, (i32)p2.x, (i32)p2.y, color);
}

void image_draw_line(Image *image, i32 x0, i32 y0, i32 x1, i32 y1, Color color) {
    if (abs_i32(x1 - x0) > abs_i32(y1 - y0)) {
        if (x0 > x1) {
            swap_i32(&x0, &x1);
            swap_i32(&y0, &y1);
        }

        i32 *y_values = (i32 *)malloc(sizeof(y_values) * (x1 - x0 + 1));
        interpolate_i32(x0, y0, x1, y1, y_values);
        for (i32 x = x0; x <= x1; x += 1) {
            image_set_pixel(image, (i32)x, y_values[x - x0], color);
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
            image_set_pixel(image, x_values[y - y0], (i32)y, color);
        }
        free(x_values);
    }
}

void image_draw_triangle_wireframe(Image *image, i32 x0, i32 y0, i32 x1, i32 y1, i32 x2, i32 y2, Color color) {
    image_draw_line(image, x0, y0, x1, y1, color);
    image_draw_line(image, x1, y1, x2, y2, color);
    image_draw_line(image, x2, y2, x0, y0, color);
}

void image_draw_triangle_filled(Image *image, i32 x0, i32 y0, i32 x1, i32 y1, i32 x2, i32 y2, Color fill_color) {
    image_draw_triangle_filled_shaded(image, x0, y0, x1, y1, x2, y2, fill_color, 1, 1, 1);
}

void image_draw_triangle_filled_shaded(Image *image,
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

            image_set_pixel(image, x, y, shaded_color);
        }
        free(intensity_segment);
    }

    free(x012);
    free(color_intensity_012);
    free(x02);
    free(color_intensity_02);
}

void image_save(Image *image, char const *filename) {
    stbi_flip_vertically_on_write(1);
    stbi_write_png(
        filename,
        image->width, image->height, image->channel_number,
        image->pixels, 0);
}