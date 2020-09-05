#include <stdlib.h>
#include <windows.h>

#include "..\language.h"
#include "..\math_utils.h"
#include "..\image.c"

global Image global_image = {500, 500, 3};

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
    Image *image = &global_image;
    image->pixels = (u8 *)calloc(image->width * image->height * image->channel_number, sizeof(image->pixels));
    
    image_draw_triangle_filled(image, 100, 100, 400, 250, 250, 400, color_red);
    image_draw_triangle_wireframe(image, 100, 100, 400, 250, 250, 400, color_white);

	image_save(image, "../render.png");
    free(image->pixels);
    return 0;
}
