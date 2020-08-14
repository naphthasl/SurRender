#ifndef SURCV_HEADER_FILE
#define SURCV_HEADER_FILE
    #include "glbl.h"
    #include "colours.h"

    typedef struct SR_Canvas {
        unsigned short width;
        unsigned short height;
        float ratio;
        SR_RGBAPixel *pixels;
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
        unsigned int x,
        unsigned int y);

    // Check if a pixel is out of bounds
    bool SR_CanvasCheckOutOfBounds(
        SR_Canvas *canvas,
        unsigned short x,
        unsigned short y);

    // Set the value of a pixel in the canvas
    void SR_CanvasSetPixel(
        SR_Canvas *canvas,
        unsigned short x,
        unsigned short y,
        SR_RGBAPixel pixel);

    // Get the value of a pixel in the canvas
    SR_RGBAPixel SR_CanvasGetPixel(
        SR_Canvas *canvas,
        unsigned short x,
        unsigned short y);

    // Destroy the in-memory representation of the canvas
    // (Must create a new canvas or resize the current one in order to access)
    void SR_DestroyCanvas(SR_Canvas *canvas);

    // Check if the canvas has been successfully allocated
    bool SR_CanvasIsValid(SR_Canvas *canvas);

    // Malloc a new canvas of given size and start copying every pixel from the
    // specified old canvas to the new one, starting at the given position.
    // This also allows you to create cropped versions of a canvas! :)
    SR_Canvas SR_CopyCanvas(
        SR_Canvas *canvas,
        unsigned short copy_start_x,
        unsigned short copy_start_y,
        unsigned short new_width,
        unsigned short new_height);
#endif
