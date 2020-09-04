#include <stdlib.h>
#include <windows.h>

#include "..\language.h"
#include "..\math_utils.h"
#include "..\image.c"

global Image global_image = {50, 50, 3};

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
    Image *image = &global_image;
    image->pixels = (u8 *)calloc(image->width * image->height * image->channel_number, sizeof(image->pixels));

    image_draw_line(image, 7, 5, 10, 40, color_white);
    image_draw_line(image, 30, 20, 50, 0, color_red);

	image_save(image, "../render.png");
    free(image->pixels);
    return 0;
}
