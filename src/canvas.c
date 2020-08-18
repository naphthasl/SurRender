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
    if (!canvas->pixels) return;

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

void SR_DestroyCanvas(SR_Canvas *canvas)
    { if (canvas->pixels) { free(canvas->pixels); canvas->pixels = NULL; } }

bool SR_CanvasIsValid(SR_Canvas *canvas)
    { return BOOLIFY(canvas->pixels); }

SR_Canvas SR_CopyCanvas(
    register SR_Canvas *canvas,
    register unsigned short copy_start_x,
    register unsigned short copy_start_y,
    unsigned short new_width,
    unsigned short new_height)
{
    SR_Canvas new = SR_NewCanvas(new_width, new_height);

    if (!new.pixels) return new;

    if (
        copy_start_x == 0 &&
        copy_start_y == 0 &&
        new.width    == canvas->width &&
        new.height   == canvas->height)
    {
        memcpy(new.pixels, canvas->pixels,
            (unsigned int)new.width *
            (unsigned int)new.height *
            sizeof(SR_RGBAPixel)
        );
    } else {
        register unsigned short x, y;
        for (x = 0; x < canvas->width; x++)
            for (y = 0; y < canvas->height; y++)
                SR_CanvasSetPixel(&new, x, y, SR_CanvasGetPixel(
                    canvas, x + copy_start_x, y + copy_start_y));
    }

    return new;
}

void SR_MergeCanvasIntoCanvas(
    register SR_Canvas *dest_canvas,
    register SR_Canvas *src_canvas,
    register unsigned short paste_start_x,
    register unsigned short paste_start_y,
    register uint8_t alpha_modifier,
    register char mode)
{
    register unsigned short x, y;
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

// Private
#define getByte(value, n) (value >> (n * 8) & 0xFF)
float lerp(float s, float e, float t) { return s + (e - s) * t; }
float blerp(float c00, float c10, float c01, float c11, float tx, float ty)
    { return lerp(lerp(c00, c10, tx), lerp(c01, c11, tx), ty); }

// Private
SR_Canvas SR_BilinearCanvasScale(
    register SR_Canvas *src,
    register unsigned short newWidth,
    register unsigned short newHeight)
{
    SR_Canvas dest = SR_NewCanvas(newWidth, newHeight);

    if (!dest.pixels) { return dest; }

    register unsigned int x, y;
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
	float skew = (float)skew_amount / (float)ycenter;
	skew_amount = abs(skew_amount);
	SR_Canvas final = SR_NewCanvas(w, h);
	SR_ZeroFill(&final);
	for (unsigned short y = 0; y < h; y++) {
		int xshift = (y - ycenter) * skew;
		for (unsigned short x = 0; x < w; x++) {
			SR_RGBAPixel pixel = SR_CanvasGetPixel(src, x, y);
			SR_CanvasSetPixel(&final, x + xshift, y, pixel);
		}
	}
	return final;
}

SR_Canvas SR_CanvasYShear(
		SR_Canvas *src,
		int skew_amount)
{
	unsigned short w = src->width;
	unsigned short h = src->height;
	unsigned short xcenter = w >> 1;
	float skew = (float)skew_amount / (float)xcenter;
	skew_amount = abs(skew_amount);
	SR_Canvas final = SR_NewCanvas(w, h);
	SR_ZeroFill(&final);
	for (unsigned short x = 0; x < w; x++) {
		int yshift = (x - xcenter) * skew;
		for (unsigned short y = 0; y < h; y++) {
			SR_RGBAPixel pixel = SR_CanvasGetPixel(src, x, y);
			SR_CanvasSetPixel(&final, x, y + yshift, pixel);
		}
	}
	return final;
}

SR_RotatedCanvas SR_CanvasRotate(
	SR_Canvas *src,
	float angle,
	bool safety_padding)
{
	//magic numbers warning
	//modulo by 2pi
	angle = fmod(angle, 6.28318530718);
	SR_Canvas temp;
	unsigned short w = src->width;
	unsigned short h = src->height;
	SR_RotatedCanvas final;
	
	if (safety_padding) {
		w <<= 1;
		h <<= 1;
		temp = SR_NewCanvas(w, h);
		SR_ZeroFill(&temp);
		SR_MergeCanvasIntoCanvas(
			&temp, src, w >> 2, h >> 2,
			255, SR_BLEND_REPLACE);
		final.offset_x = -(int)(w >> 2);
		final.offset_y = -(int)(h >> 2);
	} else {
		temp = SR_CopyCanvas(src, 0, 0, w, h);
		final.offset_x = 0;
		final.offset_y = 0;
	}
	
	final.canvas = SR_NewCanvas(w, h);
	float the_sin = sin(angle);
	float the_cos = cos(angle);
	int half_w = w >> 1;
	int half_h = h >> 1;
	
	for (int x = -half_w; x < half_w; x++) {
		for (int y = -half_h; y < half_h; y++) {
			SR_RGBAPixel pixel = SR_CanvasGetPixel(
				&temp,
				x + half_w, 
				y + half_h);
			SR_CanvasSetPixel(
				&(final.canvas),
				x * the_cos + y * the_sin + half_w,
				y * the_cos - x * the_sin + half_h,
				pixel);
		}
	}
	SR_DestroyCanvas(&temp);
	return final;
}
