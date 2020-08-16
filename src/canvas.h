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

    enum SR_ScaleModes {
        SR_SCALE_BILINEAR
    };

    // Make a canvas larger or smaller. Preserves the contents, but not
    // accurately. May ruin the current contents of the canvas.
    bool SR_ResizeCanvas(
        SR_Canvas *canvas,
        unsigned short width,
        unsigned short height);

    // A canvas may contain garbage data when initially created. This will
    // zero fill it for you, if needed.
    void SR_ZeroFill(SR_Canvas *canvas);

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
    // Note: This does not destroy the old canvas. If you don't need it anymore
    // don't forget to destroy it, or it will remain allocated.
    SR_Canvas SR_CopyCanvas(
        SR_Canvas *canvas,
        unsigned short copy_start_x,
        unsigned short copy_start_y,
        unsigned short new_width,
        unsigned short new_height);

    // Allows you to blend/merge a source canvas on to a destination canvas.
    // Can be pasted at a given offset (paste_start_x and paste_start_y)
    // Uses alpha modifier and mode values just like SR_RGBABlender. Usually
    // you'll just want a modifier of 255 and mode SR_BLEND_ADDITIVE.
    void SR_MergeCanvasIntoCanvas(
        SR_Canvas *dest_canvas,
        SR_Canvas *src_canvas,
        unsigned short paste_start_x,
        unsigned short paste_start_y,
        uint8_t alpha_modifier,
        char mode);

    // Return a scaled up version of a canvas using SR_ScaleModes for the mode.
    // Will malloc a new canvas! You will need to destroy the old one if
    // needed.
    SR_Canvas SR_CanvasScale(
        SR_Canvas *src,
        unsigned short newWidth,
        unsigned short newHeight,
        char mode);
    
    // Returns a canvas with the input canvas's content skewed horizontally
    // Will malloc a new canvas!
    SR_Canvas SR_CanvasXShear(
		SR_Canvas *src,
		int skew_amount);
	
	// Returns a canvas with the input canvas's content skewed vertically
    // Will malloc a new canvas!
    SR_Canvas SR_CanvasYShear(
		SR_Canvas *src,
		int skew_amount);
	
	// Returns a canvas that is hecking rotated, hopefully
	// Expects an angle in radians
	// Will malloc a new canvas!
	SR_Canvas SR_CanvasRotate(
		SR_Canvas *src,
		double angle);
#endif
