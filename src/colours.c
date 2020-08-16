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

uint32_t SR_RGBAtoWhole(SR_RGBAPixel pix)
    { return *(uint32_t *) &pix; }

SR_RGBAPixel SR_WholetoRGBA(uint32_t pix)
    { return *(SR_RGBAPixel *) &pix; }

SR_RGBAPixel __inline__ SR_RGBABlender(
    SR_RGBAPixel pixel_base,
    SR_RGBAPixel pixel_top,
    uint8_t alpha_modifier,
    char mode)
{
    // TODO: Make this faster and cleaner. Much faster and cleaner!

    uint32_t final = 0;

    if (mode == SR_BLEND_ADDITIVE || mode == SR_BLEND_XOR)
    {
        float alpha_mul = (
            ((float)pixel_top.alpha / 255.0) *
            ((float)alpha_modifier / 255.0)
        );
        float alpha_mul_neg = 1 - alpha_mul;

        pixel_top.rgb.red   *= alpha_mul;
        pixel_top.rgb.blue  *= alpha_mul;
        pixel_top.rgb.green *= alpha_mul;

        pixel_base.rgb.red   *= alpha_mul_neg;
        pixel_base.rgb.blue  *= alpha_mul_neg;
        pixel_base.rgb.green *= alpha_mul_neg;
    }

    uint32_t pixel_base_whole = SR_RGBAtoWhole(pixel_base);
    uint32_t pixel_top_whole  = SR_RGBAtoWhole(pixel_top );

    switch (mode)
    {
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
        case SR_BLEND_OVERLAY:
            if (((uint16_t)alpha_modifier * pixel_top.alpha) > 16129)
                final = ((pixel_top_whole & 0x00FFFFFF) |
                    (pixel_base_whole & 0xFF000000));
            else final = pixel_base_whole;

            break;
        case SR_BLEND_INVERT_DROP:
            final  = (pixel_base_whole & 0x00FFFFFF);
            final |= (255 - ((pixel_top_whole & 0xFF000000) >> 24)) << 24;

            break;
        case SR_BLEND_DROP:
            final = ((pixel_base_whole & 0x00FFFFFF) |
                (pixel_top_whole & 0xFF000000));

            break;
        default:
            fprintf(stderr, "Invalid blending mode!\n");
            exit(EXIT_FAILURE);

            break;
    }

    return SR_WholetoRGBA(final);
}