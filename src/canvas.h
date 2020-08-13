#ifndef SURCV_HEADER_FILE
#define SURCV_HEADER_FILE
    #include "glbl.h"
    #include "colours.h"

    typedef struct SR_Canvas {
        unsigned short width;
        unsigned short height;
	    SR_RGBPixel *pixels;
	} SR_Canvas;

    // Make a canvas larger or smaller. Preserves the contents, but not
    // accurately. May ruin the current contents of the canvas.
    void SR_ResizeCanvas(
        SR_Canvas *canvas,
        unsigned short width,
        unsigned short height);

    // Create a new canvas of the given size
    SR_Canvas SR_NewCanvas(unsigned short width, unsigned short height);

    // Get the width of a canvas
    unsigned short SR_CanvasGetWidth(SR_Canvas *canvas);

    // Get the height of a canvas
    unsigned short SR_CanvasGetHeight(SR_Canvas *canvas);

    // Calculate the "real" position of a pixel in the canvas - not really
    // recommended to use this yourself.
    unsigned int SR_CanvasCalcPosition(
        SR_Canvas *canvas,
        unsigned short x,
        unsigned short y);

    // Set the value of a pixel in the canvas
    void SR_CanvasSetPixel(
        SR_Canvas *canvas,
        unsigned short x,
        unsigned short y,
        SR_RGBPixel pixel);

    // Get the value of a pixel in the canvas
    SR_RGBPixel SR_CanvasGetPixel(
        SR_Canvas *canvas,
        unsigned short x,
        unsigned short y);

    // Destroy the in-memory representation of the canvas
    // (Must create a new canvas or resize the current one in order to access)
    void SR_DestroyCanvas(SR_Canvas *canvas);
#endif