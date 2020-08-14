#include "glbl.h"
#include "canvas.h"
#include "colours.h"
#include "shapes.h"

//copy pasted from:
//https://gist.github.com/bert/1085538#file-plot_line-c-L9
void SR_DrawLine(
	SR_Canvas *canvas,
	SR_RGBAPixel colour,
	int x0,
	int y0, 
	int x1, 
	int y1)
{
	int dx =  abs (x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = -abs (y1 - y0), sy = y0 < y1 ? 1 : -1; 
	int err = dx + dy, e2;

	for (;;) {
		SR_CanvasSetPixel(canvas, x0, y0, colour);
		if (x0 == x1 && y0 == y1) break;
		e2 = err << 1;
		if (e2 >= dy)
			err += dy;
			x0 += sx;
		if (e2 <= dx)
			err += dx;
			y0 += sy;
	}
}

void SR_DrawTri(
	SR_Canvas *canvas,
	SR_RGBAPixel color,
	int x0,
	int y0,
	int x1,
	int y1,
	int x2,
	int y2)
{
	SR_DrawLine(canvas, color, x0, y0, x1, y1);
	SR_DrawLine(canvas, color, x1, y1, x2, y2);
	SR_DrawLine(canvas, color, x2, y2, x0, y0);
}
