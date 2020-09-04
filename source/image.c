#include "image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "..\vendor\stb_image\stb_image.h"

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "..\vendor\stb_image\stb_image_write.h"

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

        i32 *y_values = (i32 *)malloc(sizeof(y_values) * (x1 - x0));
        interpolate(x0, y0, x1, y1, y_values);
        for (i32 x = x0; x < x1; x += 1) {
            image_set_pixel(image, (i32)x, y_values[x - x0], color);
        }
        free(y_values);
    } else {
        if (y0 > y1) {
            swap_i32(&x0, &x1);
            swap_i32(&y0, &y1);
        }

        i32 *x_values = (i32 *)malloc(sizeof(x_values) * (y1 - y0));
        interpolate(y0, x0, y1, x1, x_values);
        for (i32 y = y0; y < y1; y += 1) {
            image_set_pixel(image, x_values[y - y0], (i32)y, color);
        }
        free(x_values);
    }
}

void image_save(Image *image, char const *filename) {    
    stbi_flip_vertically_on_write(1);
    stbi_write_png(
        filename,
        image->width, image->height, image->channel_number,
        image->pixels, 0);
}