#include <stdlib.h>
#include <windows.h>
#include "..\language.h"

#define STB_IMAGE_IMPLEMENTATION
#include "..\vendor\stb_image\stb_image.h"

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "..\vendor\stb_image\stb_image_write.h"

inline i32 abs_i32(i32 value)
{
	return value < 0 ? -value : value;
}

inline void swap_i32(i32 *lhs, i32 *rhs)
{
	i32 temp = *rhs;
	*rhs = *lhs;
	*lhs = temp;
}

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

void draw_line(Image *image, i32 x0, i32 y0, i32 x1, i32 y1, Color color);

void image_set_pixel(Image *image, i32 x, i32 y, Color color)
{
	u32 index = (y * image->width + x) * image->channel_number;

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

	// image_set_pixel(image, 49, 49, {255, 255, 255});
	draw_line(image, 0, 0, 10, 40, color_white);

	stbi_flip_vertically_on_write(1);
	stbi_write_png(
		"render.png",
		image->width, image->height, image->channel_number,
		image->pixels, 0);

	free(image->pixels);
	return 0;
}

void draw_line(Image *image, i32 x0, i32 y0, i32 x1, i32 y1, Color color)
{
	i32 dx = x1 - x0;
	i32 dy = y1 - y0;
	if (abs_i32(dx) > abs_i32(dy))
	{
		if (x0 > x1)
		{
			swap_i32(&x0, &x1);
			swap_i32(&y0, &y1);
		}

		f32 a = dx / (f32) dy;
		f32 y = (f32)y0;
		for (i32 x = x0; x < x1; x += 1)
		{
			image_set_pixel(image, (i32)x, (i32)y, color);
			y = y + a;
		}
	}
	else
	{
		if (y0 > y1)
		{
			swap_i32(&x0, &x1);
			swap_i32(&y0, &y1);
		}

		f32 a = dx / (f32) dy;
		f32 x = (f32)x0;
		for (i32 y = y0; y < y1; y += 1)
		{
			image_set_pixel(image, (i32)x, (i32)y, color);
			x = x + a;
		}
	}
}