#ifndef SURCL_HEADER_FILE
#define SURCL_HEADER_FILE
    #include "glbl.h"

    typedef struct SR_RGBPixel {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
    } SR_RGBPixel;

    typedef struct SR_RGBAPixel {
        SR_RGBPixel rgb;
        uint8_t alpha;
    } SR_RGBAPixel;

    enum SR_BlendingModes {
        // XOR all RGB values
        SR_BLEND_XOR,
        // Add RGB values together with clamping
        SR_BLEND_ADDITIVE,
        // Rounded overlay approach (fastest)
        SR_BLEND_OVERLAY,
        // Replace base alpha with inverted top alpha
        SR_BLEND_INVERT_DROP,
        // Replace base alpha with top alpha
        SR_BLEND_DROP,
        // Replace entire top pixel with bottom pixel
        SR_BLEND_REPLACE,
        // Directly XOR the RGB channels without mutating the alpha
        SR_BLEND_DIRECT_XOR,
        // Directly XOR EVERYTHING (RGBA) without mutating the alpha
        SR_BLEND_DIRECT_XOR_ALL
    };

    // Construct an RGB colour value.
    __inline__ SR_RGBPixel SR_CreateRGB(
        register uint8_t red,
        register uint8_t green,
        register uint8_t blue)
    {
        SR_RGBPixel temp = {red, green, blue}; return temp;
    }

    // Create an RGBA colour value.
    __inline__ SR_RGBAPixel SR_CreateRGBA(
        register uint8_t red,
        register uint8_t green,
        register uint8_t blue,
        register uint8_t alpha)
    {
        SR_RGBAPixel temp = {{red, green, blue}, alpha};
        return temp;
    }

    // Conversion
    #define SR_RGBAtoRGB(pixel) (pixel.rgb)

    __inline__ SR_RGBAPixel SR_RGBtoRGBA(SR_RGBPixel pixel, uint8_t alpha)
    {
        SR_RGBAPixel temp;
        temp.rgb = pixel;
        temp.alpha = alpha;
        return temp;
    }

    __inline__ uint32_t SR_RGBAtoWhole(SR_RGBAPixel pix)
        { return *(uint32_t *) &pix; }

    __inline__ SR_RGBAPixel SR_WholetoRGBA(uint32_t pix)
        { return *(SR_RGBAPixel *) &pix; }

    // Blend RGBA values
    // Use mode provided by SR_BlendingModes
    // Usually, you'll want to set alpha_modifier to 255.
    SR_RGBAPixel SR_RGBABlender(
        SR_RGBAPixel pixel_base,
        SR_RGBAPixel pixel_top,
        register uint8_t alpha_modifier,
        register char mode);
#endif