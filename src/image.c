#include "glbl.h"
#include "image.h"
#include "canvas.h"
#include "colours.h"

uint8_t * LD_Blob_STBI(RadixMemoryBlob image, int *x, int *y, int *n)
{
    return stbi_load_from_memory(
        RadixAbstract_GetBlobPointer(image),
        RadixAbstract_GetBlobLength (image),
        x, y, n, 4
    );
}

SR_Canvas LD_STBICanv(uint8_t *image, int *x, int *y)
{
    SR_Canvas temp;
    temp.pixels = image;

    if (!temp.pixels)
    {
        temp = SR_NewCanvas(2, 2);
        if (!temp.pixels) {
            return temp;
        }

        SR_CanvasSetPixel(&temp, 0, 0, SR_CreateRGBA(255, 0  , 255, 255));
        SR_CanvasSetPixel(&temp, 0, 1, SR_CreateRGBA(0  , 0  , 0  , 255));
        SR_CanvasSetPixel(&temp, 1, 0, SR_CreateRGBA(0  , 0  , 0  , 255));
        SR_CanvasSetPixel(&temp, 1, 1, SR_CreateRGBA(255, 0  , 255, 255));
    } else {
        temp.width = *x;
        temp.height = *y;
        temp.ratio = (float)temp.width / temp.height;
    }

    return temp;
}

SR_Canvas SR_ImageMemToCanvas(RadixMemoryBlob image)
{
    int x, y, n;
    return LD_STBICanv(LD_Blob_STBI(image, &x, &y, &n), &x, &y);
}

SR_Canvas SR_ImageFileToCanvas(char *filename)
{
    int x, y, n;
    return LD_STBICanv(stbi_load(filename, &x, &y, &n, 4), &x, &y);
}