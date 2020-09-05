#include "image.h"

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

void image_draw_line(Image *image, i32 x0, i32 y0, i32 x1, i32 y1, Color color) {
    if (abs_i32(x1 - x0) > abs_i32(y1 - y0)) {
        if (x0 > x1) {
            swap_i32(&x0, &x1);
            swap_i32(&y0, &y1);
        }

        i32 *y_values = (i32 *)malloc(sizeof(y_values) * (x1 - x0 + 1));
        interpolate(x0, y0, x1, y1, y_values);
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
        interpolate(y0, x0, y1, x1, x_values);
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
    i32 x012_length = x01_length + x12_length - 1; // NOTE: -1 substracts the duplicate x1 value
    
    i32 *x012 = (i32 *)malloc(sizeof(x012) * x012_length);

    i32 *x01 = x012;
    interpolate(y0, x0, y1, x1, x012);
    i32 *x12 =  x01 + x01_length - 1;
    interpolate(y1, x1, y2, x2, x12);

    i32 x02_length = y2 - y0 + 1;
    i32 *x02 = (i32 *)malloc(sizeof(x02) * x02_length);
    interpolate(y0, x0, y2, x2, x02);

    i32 *x_left;
    i32 *x_right;

    i32 middle = x02_length / 2;
    if (x02[middle] < x012[middle]) {
        x_left = x02;
        x_right = x012;
    } else {
        x_left = x012;
        x_right = x02;
    }

    for (i32 y = y0; y <= y2; y++) {
        for (i32 x = x_left[y - y0]; x <= x_right[y - y0]; x++) {
            image_set_pixel(image, x, y, fill_color);
        }
    }
    
    free(x012);
    free(x02);
}

void image_save(Image *image, char const *filename) {
    stbi_flip_vertically_on_write(1);
    stbi_write_png(
        filename,
        image->width, image->height, image->channel_number,
        image->pixels, 0);
}