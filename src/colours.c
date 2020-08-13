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