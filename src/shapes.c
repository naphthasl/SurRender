#include "glbl.h"
#include "canvas.h"
#include "colours.h"
#include "shapes.h"

/* rodger: rewritten, now based off Computer Graphics, C Version (2nd Ed.) */
void SR_DrawLine(
    SR_Canvas *canvas,
    SR_RGBAPixel colour,
    int x0,
    int y0, 
    int x1, 
    int y1)
{
    if (y0 <= y1)
	{
		int t;
		t = y0, y0 = y1, y1 = t;
		t = x0, x0 = x1, x1 = t;
	}

    int dx = abs(x0 - x1), dy = abs(y0 - y1);
    int p = (dy * 2) - dx;
    int two_dy = 2 * dy, two_dydx = 2 * (dy - dx);
    int x, y, x_end;

    if (x0 > x1)  x = x1, y = y1, x_end = x0;
    else          x = x0, y = y0, x_end = x1;

    SR_CanvasSetPixel(canvas, x, y, colour);

    while (x++ < x_end) {
        if (p < 0) p += two_dy;
        else       p += two_dydx, y++;

        SR_CanvasSetPixel(canvas, x, y, colour);
    }
}

void SR_DrawTri(
    SR_Canvas *canvas,
    SR_RGBAPixel colour,
    int x0,
    int y0,
    int x1,
    int y1,
    int x2,
    int y2)
{
    SR_DrawLine(canvas, colour, x0, y0, x1, y1);
    SR_DrawLine(canvas, colour, x1, y1, x2, y2);
    SR_DrawLine(canvas, colour, x2, y2, x0, y0);
}
