#include "glbl.h"
#include "canvas.h"
#include "colours.h"

bool SR_ResizeCanvas(
    SR_Canvas *canvas,
    unsigned short width,
    unsigned short height)
{
    if (!width || !height) return false;

    canvas->width = width;
    canvas->height = height;
    
    canvas->ratio = (float)width / height;

    unsigned int size = (
        (unsigned int)width *
        (unsigned int)height *
        sizeof(SR_RGBAPixel));

    canvas->pixels = realloc(canvas->pixels, size);

    return BOOLIFY(canvas->pixels);
}

void SR_ZeroFill(SR_Canvas *canvas)
{
    unsigned int size = (
        (unsigned int)canvas->width *
        (unsigned int)canvas->height *
        sizeof(SR_RGBAPixel));

    memset(canvas->pixels, 0, size);
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
    x %= canvas->width; y %= canvas->height;
    canvas->pixels[SR_CanvasCalcPosition(canvas, x, y)] = pixel;
}

SR_RGBAPixel SR_CanvasGetPixel(
    SR_Canvas *canvas,
    unsigned short x,
    unsigned short y)
{
    if (!canvas->pixels) { return SR_CreateRGBA(255, 0, 0, 255); }

    x %= canvas->width; y %= canvas->height;

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

    if (new.pixels)
    {
        unsigned short x, y;
        for (x = 0; x < new_width; x++)
            for (y = 0; y < new_height; y++)
                SR_CanvasSetPixel(&new, x, y, SR_CanvasGetPixel(
                    canvas, x + copy_start_x, y + copy_start_y));
    }

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
    for(x = 0, y = 0; y < src_canvas->height; x++)
    {
        if(x > src_canvas->width) { x = 0; y++; }

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

// Private
#define getByte(value, n) (value >> (n * 8) & 0xFF)
float lerp(float s, float e, float t) { return s + (e - s) * t; }
float blerp(float c00, float c10, float c01, float c11, float tx, float ty)
    { return lerp(lerp(c00, c10, tx), lerp(c01, c11, tx), ty); }

// Private
SR_Canvas SR_BilinearCanvasScale(
    SR_Canvas *src,
    unsigned short newWidth,
    unsigned short newHeight)
{
    SR_Canvas dest = SR_NewCanvas(newWidth, newHeight);

    if (!dest.pixels) { return dest; }

    unsigned int x, y;
    for(x = 0, y = 0; y < newHeight; x++)
    {
        if(x > newWidth) { x = 0; y++; }

        float gx = x / (float)(newWidth ) * (SR_CanvasGetWidth (src) - 1);
        float gy = y / (float)(newHeight) * (SR_CanvasGetHeight(src) - 1);
        int gxi = (int)gx;
        int gyi = (int)gy;

        uint32_t c00 = SR_RGBAtoWhole(
            SR_CanvasGetPixel(src, gxi    , gyi    ));
        uint32_t c10 = SR_RGBAtoWhole(
            SR_CanvasGetPixel(src, gxi + 1, gyi    ));
        uint32_t c01 = SR_RGBAtoWhole(
            SR_CanvasGetPixel(src, gxi    , gyi + 1));
        uint32_t c11 = SR_RGBAtoWhole(
            SR_CanvasGetPixel(src, gxi + 1, gyi + 1));

        uint32_t result = 0;
        uint8_t i; for(i = 0; i < 4; i++)
        {
            result |= (uint8_t)blerp(
                getByte(c00, i),
                getByte(c10, i),
                getByte(c01, i),
                getByte(c11, i),
                gx - gxi, gy - gyi
            ) << (8 * i);
        }

        SR_CanvasSetPixel(&dest, x, y, SR_WholetoRGBA(result));
    }

    return dest;
}

SR_Canvas SR_CanvasScale(
    SR_Canvas *src,
    unsigned short newWidth,
    unsigned short newHeight,
    char mode)
{
    SR_Canvas final;
    switch (mode)
    {
        case SR_SCALE_BILINEAR:
            final = SR_BilinearCanvasScale(src, newWidth, newHeight);

            break;
        default:
            fprintf(stderr, "Invalid scaling mode!\n");
            exit(EXIT_FAILURE);

            break;
    }
    return final;
}

SR_Canvas SR_CanvasXShear(
		SR_Canvas *src,
		int skew_amount)
{
	unsigned short w = src->width;
	unsigned short h = src->height;
	unsigned short ycenter = h >> 1;
	double skew = (double)skew_amount / (double)ycenter;
	skew_amount = abs(skew_amount);
	SR_Canvas final = SR_NewCanvas(w + skew_amount << 1, h);
	for (unsigned short y = 0; y < h; y++) {
		int xshift = (y - ycenter) * skew;
		for (unsigned short x = 0; x < w; x++) {
			SR_RGBAPixel pixel = SR_CanvasGetPixel(src, x, y);
			SR_CanvasSetPixel(&final, x + skew_amount + xshift, y, pixel);
		}
	}
	return final;
}
