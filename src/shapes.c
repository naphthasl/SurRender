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
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
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

void SR_DrawRect(
	SR_Canvas *canvas,
	SR_RGBAPixel colour,
	unsigned short x,
	unsigned short y,
	unsigned short w,
	unsigned short h)
{
	w += x;
	h += y;
	unsigned short x1 = MIN(canvas->width - 1, w);
	unsigned short y1 = MIN(canvas->height - 1, h);
	
	for (unsigned short xi = x; xi <= x1; xi++) {
		SR_CanvasSetPixel(canvas, xi, y, colour);
		SR_CanvasSetPixel(canvas, xi, h, colour);
	}
	
	for (unsigned short yi = y + 1; yi < y1; yi++) {
		SR_CanvasSetPixel(canvas, x, yi, colour);
		SR_CanvasSetPixel(canvas, w, yi, colour);
	}
}

void SR_DrawRectFill(
	SR_Canvas *canvas,
	SR_RGBAPixel colour,
	unsigned short x,
	unsigned short y,
	unsigned short w,
	unsigned short h)
{
	unsigned short x1 = MIN(canvas->width - 1, w + x);
	unsigned short y1 = MIN(canvas->height - 1, h + y);
	
	for (unsigned short yi = y; yi <= y1; yi++) {
		for (unsigned short xi = x; xi <= x1; xi++) {
			SR_CanvasSetPixel(canvas, xi, yi, colour);
		}
	}
}
