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
	int err = dx + dy, e2; /* error value e_xy */

	for (;;) {  /* loop */
		SR_CanvasSetPixel(canvas, x0, y0, colour);
		if (x0 == x1 && y0 == y1) break;
		e2 = err << 1;
		if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
		if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
	}
}
