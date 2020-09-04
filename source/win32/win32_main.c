#include <stdlib.h>
#include <windows.h>
#include "..\language.h"
#include "..\math_utils.h"

#define STB_IMAGE_IMPLEMENTATION
#include "..\vendor\stb_image\stb_image.h"

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "..\vendor\stb_image\stb_image_write.h"

typedef struct Image
{
	const i32 width;
	const i32 height;
	const i32 channel_number;
	u8 *pixels;
} Image;

typedef struct Color
{
	u8 r, g, b;
} Color;
global Color color_white = {255, 255, 255};
global Color color_red = {255, 0, 0};

void draw_line(Image *image, i32 x0, i32 y0, i32 x1, i32 y1, Color color);

void image_set_pixel(Image *image, i32 x, i32 y, Color color)
{
	if (x >= image->width || y >= image->height)
		return;
	i32 index = (y * image->width + x) * image->channel_number;

	if (index < 0)
		return;

	image->pixels[index++] = color.r;
	image->pixels[index++] = color.g;
	image->pixels[index++] = color.b;
}

global Image global_image = {50, 50, 3};

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
{
	Image *image = &global_image;
	image->pixels = (u8 *)calloc(image->width * image->height * image->channel_number, sizeof(image->pixels));
	i32 a = 10;
	i32 b = 20;
	swap_i32(&a, &b);

	draw_line(image, 7, 5, 10, 40, color_white);
	draw_line(image, 30, 20, 50, 0, color_red);

	stbi_flip_vertically_on_write(1);
	stbi_write_png(
		"../render.png",
		image->width, image->height, image->channel_number,
		image->pixels, 0);

	free(image->pixels);
	return 0;
}

void draw_line(Image *image, i32 x0, i32 y0, i32 x1, i32 y1, Color color)
{
	if (abs_i32(x1 - x0) > abs_i32(y1 - y0))
	{
		if (x0 > x1)
		{
			swap_i32(&x0, &x1);
			swap_i32(&y0, &y1);
		}

		i32 *y_values = (i32 *)malloc(sizeof(y_values) * (x1 - x0));
		interpolate(x0, y0, x1, y1, y_values);
		for (i32 x = x0; x < x1; x += 1)
		{
			image_set_pixel(image, (i32)x, y_values[x - x0], color);
		}
	}
	else
	{
		if (y0 > y1)
		{
			swap_i32(&x0, &x1);
			swap_i32(&y0, &y1);
		}

		i32 *x_values = (i32 *)malloc(sizeof(x_values) * (y1 - y0));
		interpolate(y0, x0, y1, x1, x_values);
		for (i32 y = y0; y < y1; y += 1)
		{
			image_set_pixel(image, x_values[y - y0], (i32)y, color);
		}
	}
}