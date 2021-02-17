
#ifndef COLOR_H
#define COLOR_H

#include "language.h"

typedef struct Color {
    u8 r, g, b;
} Color;

global Color color_white = {255, 255, 255};
global Color color_red = {255, 0, 0};
global Color color_green = {0, 255, 0};
global Color color_blue = {0, 0, 255};
global Color color_orange = {225, 95, 65};
global Color color_purple = {87, 75, 144};
global Color color_yellow = {247, 215, 148};

#endif // COLOR_H