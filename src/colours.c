#include "glbl.h"
#include "colours.h"

SR_RGBPixel SR_CreateRGB(uint8_t red, uint8_t green, uint8_t blue)
    { SR_RGBPixel temp = {red, green, blue}; return temp; }

SR_RGBAPixel SR_CreateRGBA(
    uint8_t red,
    uint8_t green,
    uint8_t blue,
    uint8_t alpha)
{
    SR_RGBAPixel temp = {{red, green, blue}, alpha};
    return temp;
}

SR_RGBPixel SR_RGBAtoRGB(SR_RGBAPixel pixel)
    { return pixel.rgb; }

SR_RGBAPixel SR_RGBtoRGBA(SR_RGBPixel pixel, uint8_t alpha)
{
    SR_RGBAPixel temp;
    temp.rgb = pixel;
    temp.alpha = alpha;
    return temp;
}

SR_RGBAPixel SR_RGBABlender(
    SR_RGBAPixel pixel_base,
    SR_RGBAPixel pixel_top,
    uint8_t alpha_modifier,
    char mode)
{
    uint32_t final = 0;

    if (pixel_top.alpha)
    {
        pixel_top.rgb.red   *= ((float)pixel_top.alpha / 255.0);
        pixel_top.rgb.blue  *= ((float)pixel_top.alpha / 255.0);
        pixel_top.rgb.green *= ((float)pixel_top.alpha / 255.0);
    }

    if (alpha_modifier)
    {
        pixel_top.rgb.red   *= ((float)alpha_modifier / 255.0);
        pixel_top.rgb.blue  *= ((float)alpha_modifier / 255.0);
        pixel_top.rgb.green *= ((float)alpha_modifier / 255.0);
    }

    uint32_t pixel_base_whole = *(uint32_t *) &pixel_base;
    uint32_t pixel_top_whole  = *(uint32_t *) &pixel_top ;

    switch (mode) {
        case SR_BLEND_XOR:
            final = pixel_base_whole ^ (pixel_top_whole & 0x00FFFFFF);

            break;
        case SR_BLEND_ADDITIVE:
            final = (
                (MIN(((pixel_base_whole & 0x000000FF)      ) +
                ((pixel_top_whole & 0x000000FF)      ), 255)      ) |
                (MIN(((pixel_base_whole & 0x0000FF00) >> 8 ) +
                ((pixel_top_whole & 0x0000FF00) >> 8 ), 255) << 8 ) |
                (MIN(((pixel_base_whole & 0x00FF0000) >> 16) +
                ((pixel_top_whole & 0x00FF0000) >> 16), 255) << 16) |
                (pixel_base_whole & 0xFF000000)
            );

            break;
    }

    return *(SR_RGBAPixel *) &final;
}