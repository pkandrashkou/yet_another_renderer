#include <stdlib.h>
#include <windows.h>
#include <inttypes.h>

#define STB_IMAGE_IMPLEMENTATION
#include "..\vendor\stb_image\stb_image.h"

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "..\vendor\stb_image\stb_image_write.h"

struct Image {
	const int width;
	const int height;
	const int64_t channel_number;
	uint8_t *pixels;
};

struct Color {
	int r, g, b;
};

void draw_line(Image *image, int x0, int y0, int x1, int y1);

void image_set_pixel(Image *image, int x, int y, Color color) {
	if (x == 0 && y == 0) return;
	int index = image->width * y + x;
	
	if (index < 0) return;
	index *= image->channel_number;
	image->pixels[index++] = color.r;
	image->pixels[index++] = color.g;
	image->pixels[index++] = color.b;
}

Image image = { 50, 50, 3 };

INT 
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
	image.pixels = (uint8_t *)calloc(image.width * image.height * image.channel_number, sizeof(image.pixels));

	draw_line(&image, 0, 1, 10, 1);

	stbi_flip_vertically_on_write(true);
	stbi_write_png("render.png", image.width, image.height, image.channel_number, image.pixels, image.width * image.channel_number);
	free(image.pixels);
	return 0;
}

void draw_line(Image *image, int x0, int y0, int x1, int y1) {
	if (x0 > x1) {
		int temp_x = x0;
		x0 = x1;
		x1 = temp_x;

		int temp_y = y0;
		y0 = y1;
		y1 = temp_y;
	}

	float a = (y1 - y0) / (float)(x1 - x0);
	float y = (float)y0;
	for (float x = (float)x0; x < (float)x1; x+= 0.1) {
		image_set_pixel(image, (int)x, (int)y, { 255, 255, 255});
		y = y + a;
	}
}