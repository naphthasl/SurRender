#ifndef SURSH_HEADER_FILE
#define SURSH_HEADER_FILE
	#include "glbl.h"
	#include "canvas.h"
    #include "colours.h"
    
	//line drawing function
    void SR_DrawLine(
		SR_Canvas *canvas,
		SR_RGBAPixel color,
		int x0,
		int y0,
		int x1,
		int y1);
	
	//triangle drawing function
    void SR_DrawTri(
		SR_Canvas *canvas,
		SR_RGBAPixel color,
		int x0,
		int y0,
		int x1,
		int y1,
		int x2,
		int y2);
#endif
