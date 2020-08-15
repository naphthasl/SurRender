#ifndef SURIM_HEADER_FILE
#define SURIM_HEADER_FILE
    #include "glbl.h"
    #include "canvas.h"
    #include "colours.h"

    #define STB_IMAGE_IMPLEMENTATION
    #define STBI_ONLY_BMP
    #include "stb_image.h"

    // Load a Radix memory blob as an image and convert it into a canvas
    SR_Canvas SR_ImageMemToCanvas(RadixMemoryBlob image);

    // Load an image file into a new canvas
    SR_Canvas SR_ImageFileToCanvas(char *filename);

    // WARNING: All created canvases must be freed as they are mallocated.
#endif