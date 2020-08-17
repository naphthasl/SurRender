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

void SR_CanvasRotFixed(SR_Canvas *src, char quarter_turn) {
    quarter_turn %= 4; // Only four possibilities

    if (!quarter_turn) return; // 0 degree rotation lol

	register unsigned short w = src->width;
	register unsigned short h = src->height;
    register unsigned short x, y;
    for (x = 0; x < (w >> 1); x++)
    {
        for (y = 0; y < (h >> 1); y++)
        {
            SR_RGBAPixel pixel0 = SR_CanvasGetPixel(src,     x,     y);
            SR_RGBAPixel pixel1 = SR_CanvasGetPixel(src,     y, w - x);
            SR_RGBAPixel pixel2 = SR_CanvasGetPixel(src, w - x, h - y);
            SR_RGBAPixel pixel3 = SR_CanvasGetPixel(src, h - y,     x);
            switch (quarter_turn)
            {
                case 1:
                    SR_CanvasSetPixel(src,     y, w - x, pixel0);
                    SR_CanvasSetPixel(src, w - x, h - y, pixel1);
                    SR_CanvasSetPixel(src, h - y,     x, pixel2);
                    SR_CanvasSetPixel(src,     x,     y, pixel3);
                    break;
                case 2:
                    SR_CanvasSetPixel(src, w - x, h - y, pixel0);
                    SR_CanvasSetPixel(src, h - y,     x, pixel1);
                    SR_CanvasSetPixel(src,     x,     y, pixel2);
                    SR_CanvasSetPixel(src,     y, w - x, pixel3);
                    break;
                case 3:
                    SR_CanvasSetPixel(src, h - y,     x, pixel0);
                    SR_CanvasSetPixel(src,     x,     y, pixel1);
                    SR_CanvasSetPixel(src,     y, w - x, pixel2);
                    SR_CanvasSetPixel(src, w - x, h - y, pixel3);
                    break;
            }
        }
	}
}

SR_RotatedCanvas SR_CanvasRotate(
	SR_Canvas *src,
	float angle,
	bool safety_padding)
{
	//magic numbers warning
	//modulo by 2pi
	//multiply by 2/pi, same as dividing by pi/2
	char quarter_turns = floor(fmod(angle, 6.28318530718) * 0.636619772368);
	SR_Canvas temp0;
	unsigned short w = src->width;
	unsigned short h = src->height;
	SR_RotatedCanvas final;
	if (safety_padding) {
		w <<= 1;
		h <<= 1;
		temp0 = SR_NewCanvas(w, h);
		SR_ZeroFill(&temp0);
		SR_MergeCanvasIntoCanvas(
			&temp0, src, w >> 2, h >> 2,
			255, SR_BLEND_REPLACE);
		final.offset_x = -(int)(w >> 2);
		final.offset_y = -(int)(h >> 2);
	} else {
		temp0 = SR_CopyCanvas(src, 0, 0, w, h);
		final.offset_x = 0;
		final.offset_y = 0;
	}
	SR_CanvasRotFixed(&temp0, quarter_turns);
	
	//modulo by pi/2
	angle = fmod(angle, 1.57079632679);
	unsigned short sneeze = MAX(w, h) >> 1;
	float xshear = -tan(angle / 2) * sneeze;
	float yshear = sin(angle) * sneeze;
	SR_Canvas temp1 = SR_CanvasYShear(&temp0, xshear);
	SR_DestroyCanvas(&temp0);
	SR_Canvas temp2 = SR_CanvasXShear(&temp1, yshear);
	SR_DestroyCanvas(&temp1);
	final.canvas = SR_CanvasYShear(&temp2, xshear);
	SR_DestroyCanvas(&temp2);
	return final;
}

//Private
/*
void SR_CanvasXShearPadded(SR_Canvas *src, int skew_amount) {
	unsigned short xstart = src->width >> 2;
	unsigned short xend = xstart + src->width >> 1;
	unsigned short copy_length = src->width << 1;
	unsigned short ycenter = src->height >> 1;
	unsigned short ystart = src->height >> 2;
	unsigned short yend = ystart + ycenter;
	float skew = (float)skew_amount / (float)ystart;
	for (unsigned short y = ystart; y < yend; y++) {
		int xshift = (y - ycenter - ystart) * skew;
		unsigned long long memory_offset = src->pixels + y * src->width;
		memcpy(memory_offset + xshift, memory_offset, copy_length);
	}
}
*/
