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
    canvas->ratio = (float)width / height;

    unsigned int size = (
        (unsigned int)width *
        (unsigned int)height *
        sizeof(SR_RGBAPixel));

    if (size) canvas->pixels = realloc(canvas->pixels, size);
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
    unsigned int x,
    unsigned int y)
{
    return (((unsigned int)canvas->width) * y) + x;
}

bool SR_CanvasCheckOutOfBounds(
    SR_Canvas *canvas,
    unsigned short x,
    unsigned short y)
{
    if (x >= canvas->width || y >= canvas->height) return true;
    else return false;
}

void SR_CanvasSetPixel(
    SR_Canvas *canvas,
    unsigned short x,
    unsigned short y,
    SR_RGBAPixel pixel)
{
    if (!canvas->pixels) return;
    if (SR_CanvasCheckOutOfBounds(canvas, x, y)) return;
    canvas->pixels[SR_CanvasCalcPosition(canvas, x, y)] = pixel;
}

SR_RGBAPixel SR_CanvasGetPixel(
    SR_Canvas *canvas,
    unsigned short x,
    unsigned short y)
{
    const SR_RGBAPixel empty = SR_CreateRGBA(0, 0, 0, 0);
    if (!canvas->pixels) return empty;
    if (SR_CanvasCheckOutOfBounds(canvas, x, y)) return empty;

    return canvas->pixels[SR_CanvasCalcPosition(canvas, x, y)];
}

void SR_DestroyCanvas(SR_Canvas *canvas)
    { if (canvas->pixels) { free(canvas->pixels); canvas->pixels = NULL; } }

bool SR_CanvasIsValid(SR_Canvas *canvas)
    { return BOOLIFY(canvas->pixels); }

SR_Canvas SR_CopyCanvas(
    SR_Canvas *canvas,
    unsigned short copy_start_x,
    unsigned short copy_start_y,
    unsigned short new_width,
    unsigned short new_height)
{
    SR_Canvas new = SR_NewCanvas(new_width, new_height);

    unsigned short x, y;
    for (x = 0; x < new_width; x++)
        for (y = 0; y < new_height; y++)
            SR_CanvasSetPixel(&new, x, y, SR_CanvasGetPixel(
                canvas, x + copy_start_x, y + copy_start_y));

    return new;
}

void SR_MergeCanvasIntoCanvas(
    SR_Canvas *dest_canvas,
    SR_Canvas *src_canvas,
    unsigned short paste_start_x,
    unsigned short paste_start_y,
    uint8_t alpha_modifier,
    char mode)
{
    // TODO: Add support for rotation

    unsigned short x, y;
    for (x = 0; x < src_canvas->width; x++)
    {
        for (y = 0; y < src_canvas->height; y++)
        {
            SR_CanvasSetPixel(
                dest_canvas,
                x + paste_start_x,
                y + paste_start_y,
                SR_RGBABlender(
                    SR_CanvasGetPixel(
                        dest_canvas,
                        x + paste_start_x,
                        y + paste_start_y
                    ),
                    SR_CanvasGetPixel(src_canvas, x, y),
                    alpha_modifier,
                    mode
                )
            );
        }
    }
}