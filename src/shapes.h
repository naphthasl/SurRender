#ifndef SURSH_HEADER_FILE
#define SURSH_HEADER_FILE
    #include "glbl.h"
    #include "canvas.h"
    #include "colours.h"
    
    //line drawing function
    void SR_DrawLine(
        SR_Canvas *canvas,
        SR_RGBAPixel colour,
        int x0,
        int y0,
        int x1,
        int y1);
    
    //triangle drawing function
    void SR_DrawTriOutline(
        SR_Canvas *canvas,
        SR_RGBAPixel colour,
        int x0,
        int y0,
        int x1,
        int y1,
        int x2,
        int y2);

    void SR_DrawTri(
        SR_Canvas *canvas,
        SR_RGBAPixel colour,
        int x0,
        int y0, 
        int x1, 
        int y1,
        int x2,
        int y2);
    
    //get pain wrecked
    void SR_DrawRectOutline(
        SR_Canvas *canvas,
        SR_RGBAPixel colour,
        unsigned short x,
        unsigned short y,
        unsigned short w,
        unsigned short h);
    
    void SR_DrawRect(
        SR_Canvas *canvas,
        SR_RGBAPixel colour,
        unsigned short x,
        unsigned short y,
        unsigned short w,
        unsigned short h);
    
    //round
    void SR_DrawCircOutline(
        SR_Canvas *canvas,
        SR_RGBAPixel colour,
        int x,
        int y,
        int r);
    
    void SR_DrawCirc(
        SR_Canvas *canvas,
        SR_RGBAPixel colour,
        unsigned short x,
        unsigned short y,
        unsigned short r);
#endif
