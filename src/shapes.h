#ifndef SURSH_HEADER_FILE
#define SURSH_HEADER_FILE
	#include "glbl.h"
	#include "canvas.h"
    #include "colours.h"
    
	//base line function
    void SR_DrawLine(
		SR_Canvas *canvas,
		SR_RGBAPixel color,
		int x0,
		int y0,
		int x1,
		int y1);
#endif
