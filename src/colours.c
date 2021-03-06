#include "glbl.h"
#include "colours.h"

SR_RGBAPixel SR_RGBABlender(
    SR_RGBAPixel pixel_base,
    SR_RGBAPixel pixel_top,
    register uint8_t alpha_modifier,
    register char mode)
{
    // TODO: Make this faster and cleaner. Much faster and cleaner!

    register uint32_t final, pixel_base_whole, pixel_top_whole = 0;
    register float alpha_mul, alpha_mul_neg;

    if (mode != SR_BLEND_ADDITIVE && mode != SR_BLEND_XOR) goto srbl_nomul;

    alpha_mul = (
        ((float)pixel_top.alpha / 255.0) *
        ((float)alpha_modifier / 255.0)
    );
    alpha_mul_neg = 1 - alpha_mul;

    pixel_top.rgb.red   *= alpha_mul;
    pixel_top.rgb.blue  *= alpha_mul;
    pixel_top.rgb.green *= alpha_mul;

    pixel_base.rgb.red   *= alpha_mul_neg;
    pixel_base.rgb.blue  *= alpha_mul_neg;
    pixel_base.rgb.green *= alpha_mul_neg;

srbl_nomul:
    pixel_base_whole = SR_RGBAtoWhole(pixel_base);
    pixel_top_whole  = SR_RGBAtoWhole(pixel_top );

    switch (mode) {
    case SR_BLEND_DIRECT_XOR: // Mul skipped by goto srbl_nomul
    case SR_BLEND_XOR: // Mul version
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
    case SR_BLEND_REPLACE:
        final = pixel_top_whole;

        break;
    case SR_BLEND_DIRECT_XOR_ALL:
        final = pixel_base_whole ^ pixel_top_whole;
        break;
    default:
        fprintf(stderr, "Invalid blending mode!\n");
        exit(EXIT_FAILURE);

        break;
    }

    return SR_WholetoRGBA(final);
}