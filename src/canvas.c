#include "glbl.h"
#include "canvas.h"
#include "colours.h"

bool SR_ResizeCanvas(
    SR_Canvas *canvas,
    unsigned short width,
    unsigned short height)
{
    // It is impossible to create a 0-width/0-height canvas.
    if (!width || !height) return false;

    canvas->width = width;
    canvas->height = height;
    
    // Not strictly neccessary, but rodger put it here anyway, so whatever.
    canvas->ratio = (float)width / height;

    /* FYI: We can actually use realloc here if we assume that canvas->pixels
     * contains either a valid allocation or none at all (represented by NULL)
     * That way, we can simplify the whole process, as realloc works just like
     * malloc does when you feed it a null pointer. Magic!
     */
    canvas->pixels = realloc(
        canvas->pixels,
        (unsigned int)(
            (unsigned int)width *
            (unsigned int)height *
            sizeof(SR_RGBAPixel)));

    // Return the allocation state.
    return BOOLIFY(canvas->pixels);
}

void SR_ZeroFill(SR_Canvas *canvas)
{
    if (!canvas->pixels) return;

    // Fill the canvas with zeros, resulting in RGBA(0, 0, 0, 0).
    // To fill with something like RGBA(0, 0, 0, 255), see shapes.h.
    memset(canvas->pixels, 0, SR_CanvasCalcSize(canvas));
}

SR_Canvas SR_NewCanvas(unsigned short width, unsigned short height)
{
    SR_Canvas temp;
    temp.pixels = NULL; // Just for realloc's sake.

    // As long as we set pixels to NULL, ResizeCanvas can be used here too.
    SR_ResizeCanvas(&temp, width, height);

    return temp;
}

// Just some abstraction functions - may be macro'd later.
unsigned short SR_CanvasGetWidth(SR_Canvas *canvas)
    { return canvas->width; }

unsigned short SR_CanvasGetHeight(SR_Canvas *canvas)
    { return canvas->height; }

// SR_DestroyCanvas is super important for any mallocated canvases. Use it.
void SR_DestroyCanvas(SR_Canvas *canvas)
    { if (canvas->pixels) { free(canvas->pixels); canvas->pixels = NULL; } }

// Check if a canvas has been destroyed - i.e it's a null pointer
bool SR_CanvasIsValid(SR_Canvas *canvas)
    { return BOOLIFY(canvas->pixels); }

SR_Canvas SR_CopyCanvas(
    register SR_Canvas *canvas,
    register unsigned short copy_start_x,
    register unsigned short copy_start_y,
    unsigned short new_width,
    unsigned short new_height)
{
    // Create the destination canvas
    SR_Canvas new = SR_NewCanvas(new_width, new_height);

    // If it isn't valid, just return the metadata and pray it doesn't get used
    if (!new.pixels) goto srcc_finish;

    if (copy_start_x == 0 &&
        copy_start_y == 0 &&
        new.width    == canvas->width &&
        new.height   == canvas->height) {
        // Super fast memcpy when possible, hopefully.
        memcpy(new.pixels, canvas->pixels, SR_CanvasCalcSize(&new));

        goto srcc_finish; // Just jump to finish here, saves ident level
    }

    // Slower copying, but not much slower - used for cropping/panning
    register unsigned short x, y;
    for (x = 0; x < new.width; x++)
        for (y = 0; y < new.height; y++)
            SR_CanvasSetPixel(&new, x, y, SR_CanvasGetPixel(
                canvas, x + copy_start_x, y + copy_start_y));

srcc_finish:
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
    for (x = 0; x < src_canvas->width; x++) {
        for (y = 0; y < src_canvas->height; y++) {
            // Uses the function for blending individual RGBA values.
            SR_CanvasSetPixel(
                dest_canvas,
                x + paste_start_x,
                y + paste_start_y,
                SR_RGBABlender(
                    SR_CanvasGetPixel(
                        dest_canvas,
                        x + paste_start_x,
                        y + paste_start_y),
                    SR_CanvasGetPixel(src_canvas, x, y),
                    alpha_modifier,
                    mode));
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
    for(x = 0, y = 0; y < newHeight; x++) {
        if(x > newWidth) { x = 0; y++; }

        float gx = x / (float)(newWidth ) * (SR_CanvasGetWidth (src) - 1);
        float gy = y / (float)(newHeight) * (SR_CanvasGetHeight(src) - 1);
        int gxi = (int)gx;
        int gyi = (int)gy;

        // TODO: Clean this up, preferably stop using SR_RGBAtoWhole, it's slow

        uint32_t c00 = SR_RGBAtoWhole(
            SR_CanvasGetPixel(src, gxi    , gyi    ));
        uint32_t c10 = SR_RGBAtoWhole(
            SR_CanvasGetPixel(src, gxi + 1, gyi    ));
        uint32_t c01 = SR_RGBAtoWhole(
            SR_CanvasGetPixel(src, gxi    , gyi + 1));
        uint32_t c11 = SR_RGBAtoWhole(
            SR_CanvasGetPixel(src, gxi + 1, gyi + 1));

        uint32_t result = 0;
        for (uint8_t i = 0; i < 4; i++) {
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
    // TODO: Add nearest neighbour scaling - this is super, super important

    SR_Canvas final;
    switch (mode) {
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

unsigned short * SR_NZBoundingBox(SR_Canvas *src)
{
    // TODO: Test this for bugs
    // TODO: Find some way to clean up the repetition here

    // Static declaration prevents a dangling pointer
    static unsigned short bbox[4] = {0, 0, 0, 0};
    register unsigned short xC, yC, firstX, firstY, lastX, lastY, x, y;

    for (y = 0; y < src->height; y++)
        for (x = 0; x < src->width; x++)
            if (SR_CanvasPixelCNZ(src, x, y))
                { firstX = x, firstY = y; goto srnzbbx_first_pixel_done; }
    
    goto srnzbbx_empty; // No data found in image - commit die
srnzbbx_first_pixel_done: // Exit loop
    for (y = src->height - 1; y > 0; y--)
        for (x = src->width - 1; x > 0; x--)
            if (SR_CanvasPixelCNZ(src, x, y))
                { lastX = x, lastY = y; goto srnzbbx_last_pixel_done; }

    goto srnzbbx_empty;
srnzbbx_last_pixel_done:
    for (xC = 0; xC <= firstX; xC++)
        for (yC = firstY; yC <= lastY; yC++)
            if (SR_CanvasPixelCNZ(src, xC, yC)) {
                bbox[0] = xC; bbox[1] = firstY;
                goto srnzbbx_found_first;
            }

    goto srnzbbx_empty;
srnzbbx_found_first:
    for (xC = src->width - 1; xC > lastX; xC--)
        for (yC = lastY; yC >= firstY; yC--)
            if (SR_CanvasPixelCNZ(src, xC, yC)) {
                bbox[2] = xC; bbox[3] = lastY;
                goto srnzbbx_bounded;
            }

    goto srnzbbx_no_end_in_sight; // No last point found - is this possible?
srnzbbx_no_end_in_sight:
    bbox[2] = src->width - 1; bbox[3] = src->height - 1;
srnzbbx_bounded:
    return bbox; // Return the box (er, I mean RETURN THE SLAB)
srnzbbx_empty:
    /* We can return a null pointer if we believe the canvas is empty, making
     * it easier to check the state of a canvas.
     */
    return NULL;
}

SR_OffsetCanvas SR_CanvasShear(
        SR_Canvas *src,
        int skew_amount,
        bool mode)
{
    unsigned short w, h, mcenter;
    float skew;

    w = src->width;
    h = src->height;
    mcenter = mode ? w >> 1 : h >> 1;
    skew = (float)skew_amount / (float)mcenter;
    skew_amount = abs(skew_amount);

    SR_OffsetCanvas final;

    if (mode) final.canvas = SR_NewCanvas(w, h + (skew_amount << 1));
    else final.canvas = SR_NewCanvas(w + (skew_amount << 1), h);
    SR_ZeroFill(&(final.canvas));

    final.offset_x = mode ? 0 : -skew_amount;
    final.offset_y = mode ? -skew_amount : 0;

    int mshift;

    // TODO: Find some way to clean up the repetition here

    if (mode) {
        for (unsigned short x = 0; x < w; x++) {
            mshift = skew_amount + (x - mcenter) * skew;
            for (unsigned short y = 0; y < h; y++) {
                SR_RGBAPixel pixel = SR_CanvasGetPixel(src, x, y);
                SR_CanvasSetPixel(&(final.canvas), x, y + mshift, pixel);
            }
        }
	} else {
        for (unsigned short y = 0; y < h; y++) {
            mshift = skew_amount + (y - mcenter) * skew;
            for (unsigned short x = 0; x < w; x++) {
                SR_RGBAPixel pixel = SR_CanvasGetPixel(src, x, y);
                SR_CanvasSetPixel(&(final.canvas), x + mshift, y, pixel);
            }
        }
    }

    return final;
}

SR_OffsetCanvas SR_CanvasRotate(
    SR_Canvas *src,
    float degrees,
    bool safety_padding,
    bool autocrop)
{
    // Declare everything here
    SR_Canvas temp;
    register unsigned short w, h, boundary, xC, yC, nx, ny;
    int x, y, nxM, nyM, half_w, half_h;
    float the_sin, the_cos;
    SR_RGBAPixel pixel, pixbuf;
    SR_OffsetCanvas final;

    // There's no point in considering unique values above 359. 360 -> 0
    degrees = fmod(degrees, 360);

    // For simplicity's sake
    w = src->width;
    h = src->height;

    final.offset_x = 0;
    final.offset_y = 0;

    if (safety_padding) {
        // Create additional padding in case rotated data goes off-canvas
        boundary = MAX(w, h) << 1; // Double the largest side length
        final.canvas = SR_NewCanvas(boundary, boundary);
        final.offset_x = -(int)(boundary >> 2);
        final.offset_y = -(int)(boundary >> 2);
    } else {
        final.canvas = SR_NewCanvas(w, h);
    }
    // Prevent garbage data seeping in
    SR_ZeroFill(&final.canvas);

    // Rotation not 0, 90, 180 or 270 degrees? Use inaccurate method instead
    if (fmod(degrees, 90) != .0) goto srcvrot_mismatch;

    // Trying to rotate 0 degrees? Just copy the canvas, I guess.
    if (!((unsigned short)degrees % 360)) {
        SR_MergeCanvasIntoCanvas(
            &final.canvas,
            src,
            -final.offset_x, // Still need to use the offset incase padding
            -final.offset_y,
            255,
            SR_BLEND_REPLACE); // Fastest and safest, also no background alpha

        goto srcvrot_finished; // Jump to the finishing/cleanup line
    }

    /* TODO: Mismatching width and height causes accurate rotation bug.
     * We should probably fix this, but for now it's fine to use inaccurate
     * rotation.
     */
    if (w != h) goto srcvrot_mismatch;

    // This is the accurate rotation system, but it only works on degrees
    // where x % 90 == 0
    for (xC = 0; xC < w; xC++) {
        for (yC = 0; yC < h; yC++) {
            pixbuf = SR_CanvasGetPixel(src, xC, yC);
            switch (((unsigned short)degrees) % 360) {
            case 90:
                nx = (h - 1) - yC;
                ny = xC;
                break;
            case 180:
                nx = (w - 1) - xC;
                ny = (h - 1) - yC;
                break;
            case 270:
                nx = yC;
                ny = (w - 1) - xC;
                break;
            }

            SR_CanvasSetPixel(
                &final.canvas,
                nx - final.offset_x, // Correct for offset
                ny - final.offset_y,
                pixbuf);
        }
    }

    goto srcvrot_finished;

srcvrot_mismatch:
    // Convert to radians and then modulo by 2*pi
    degrees = fmod(degrees * 0.017453292519943295, 6.28318530718);

    the_sin = -sin(degrees);
    the_cos = cos(degrees);
    half_w = w >> 1;
    half_h = h >> 1;

    for (x = -half_w; x < half_w; x++) {
        for (y = -half_h; y < half_h; y++) {
            nxM = (x * the_cos + y * the_sin + half_w) - final.offset_x;
            nyM = (y * the_cos - x * the_sin + half_h) - final.offset_y;
            pixel = SR_CanvasGetPixel(src, x + half_w, y + half_h);

            // Set target AND a single nearby pixel to de-alias
            SR_CanvasSetPixel(&final.canvas, nxM    , nyM    , pixel);
            SR_CanvasSetPixel(&final.canvas, nxM - 1, nyM    , pixel);
        }
    }

srcvrot_finished:
    if (safety_padding && autocrop) {
        // If autocropping is enabled, auto-crop padded images. This is slow,
        // but saves memory.
        unsigned short * bbox = SR_NZBoundingBox(&final.canvas);
        if (bbox) {
            temp = SR_CopyCanvas(
                &final.canvas,
                bbox[0],
                bbox[1],
                (bbox[2] - bbox[0]) + 1,
                (bbox[3] - bbox[1]) + 1);

            SR_DestroyCanvas(&final.canvas);
            final.canvas = temp;

            final.offset_x += bbox[0];
            final.offset_y += bbox[1];
        }
    }

    return final;
}

void SR_InplaceFlip(SR_Canvas *src, bool vertical)
{
    // Flipping canvases honestly doesn't need a new canvas to be allocated,
    // so we can do it in-place just fine for extra speed and less memory
    // usage.
    register unsigned short x, y, wmax, hmax, xdest, ydest;
    SR_RGBAPixel temp, pixel;

    wmax = vertical ? src->width : src->width >> 1;
    hmax = vertical ? src->height >> 1 : src->height;

    for (x = 0; x < wmax; x++) {
        for (y = 0; y < hmax; y++) {
            xdest = vertical ? x : (src->width  - 1) - x;
            ydest = vertical ? (src->height - 1) - y : y;

            temp  = SR_CanvasGetPixel(src, xdest, ydest);
            pixel = SR_CanvasGetPixel(src, x, y);
            SR_CanvasSetPixel(src, xdest, ydest, pixel);
            SR_CanvasSetPixel(src, x, y, temp);
        }
    }
}
