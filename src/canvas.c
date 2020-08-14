#include "glbl.h"
#include "canvas.h"
#include "colours.h"

void SR_ResizeCanvas(
    SR_Canvas *canvas,
    unsigned short width,
    unsigned short height)
{
    canvas->width = width;
    canvas->height = height;

    canvas->pixels = realloc(
        canvas->pixels,
        (unsigned int)width *
        (unsigned int)height *
        sizeof(SR_RGBAPixel));
}

SR_Canvas SR_NewCanvas(unsigned short width, unsigned short height)
{
    SR_Canvas temp;
    temp.pixels = NULL;
    SR_ResizeCanvas(&temp, width, height);

    return temp;
}

unsigned short SR_CanvasGetWidth(SR_Canvas *canvas)
    { return canvas->width; }

unsigned short SR_CanvasGetHeight(SR_Canvas *canvas)
    { return canvas->height; }

unsigned int SR_CanvasCalcPosition(
    SR_Canvas *canvas,
    unsigned short x,
    unsigned short y)
{
    x = (unsigned int)(MIN(canvas->width  - 1, x));
    y = (unsigned int)(MIN(canvas->height - 1, x));
    return ((unsigned int)canvas->width * x) + y;
}

void SR_CanvasSetPixel(
    SR_Canvas *canvas,
    unsigned short x,
    unsigned short y,
    SR_RGBAPixel pixel)
{
    if (!canvas->pixels) return;
    canvas->pixels[SR_CanvasCalcPosition(canvas, x, y)] = pixel;
}

SR_RGBAPixel SR_CanvasGetPixel(
    SR_Canvas *canvas,
    unsigned short x,
    unsigned short y)
{
    if (!canvas->pixels) return SR_CreateRGBA(0, 0, 0, 0);

    return canvas->pixels[SR_CanvasCalcPosition(canvas, x, y)];
}

void SR_DestroyCanvas(SR_Canvas *canvas)
    { if (canvas->pixels) { free(canvas->pixels); canvas->pixels = NULL; } }

bool SR_CanvasAllocated(SR_Canvas *canvas)
    { return BOOLIFY(canvas->pixels); }