#include "glbl.h"
#include "canvas.h"
#include "colours.h"
#include "shapes.h"

//copy pasted from:
//https://gist.github.com/bert/1085538#file-plot_line-c-L9
void SR_DrawLine(
    SR_Canvas *canvas,
    SR_RGBAPixel colour,
    int x0, int y0, 
    int x1, int y1)
{
    int dx, dy, err, sx, sy, e2;
    dx =  abs (x1 - x0), sx = x0 < x1 ? 1 : -1;
    dy = -abs (y1 - y0), sy = y0 < y1 ? 1 : -1; 
    err = dx + dy, e2;

    for (;;) {
        SR_CanvasSetPixel(canvas, x0, y0, colour);

        if (x0 == x1 && y0 == y1) break;

        e2 = err << 1;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void SR_DrawTriOutline(
    SR_Canvas *canvas,
    SR_RGBAPixel colour,
    int x0, int y0,
    int x1, int y1,
    int x2, int y2)
{
    SR_DrawLine(canvas, colour, x0, y0, x1, y1);
    SR_DrawLine(canvas, colour, x1, y1, x2, y2);
    SR_DrawLine(canvas, colour, x2, y2, x0, y0);
}

void SR_DrawTri(
    SR_Canvas *canvas,
    SR_RGBAPixel colour,
    int x0, int y0,
    int x1, int y1,
    int x2, int y2)
{
	unsigned short  min_x, min_y, max_x, max_y;
    min_x = MIN(x0, MIN(x1, x2));
	min_y = MIN(y0, MIN(y1, y2));
	max_x = MAX(x0, MAX(x1, x2));
	max_y = MAX(y0, MAX(y1, y2));
    
    int v0x, v0y, v1x, v1y;
    v0x = x1 - x0;
    v0y = y1 - y0;
    v1x = x2 - x0;
    v1y = y2 - y0;
    
    float vcross = v0x * v1y - v0y * v1x;
    
    for (unsigned short x = min_x; x <= max_x; x++) {
        for (unsigned short y = min_y; y <= max_y; y++) {
            int vqx = x - x0;
            int vqy = y - y0;
            
            float s = (float)(vqx * v1y - vqy * v1x);
            s /= vcross;
            float t = (float)(v0x * vqy - v0y * vqx);
            t /= vcross;
            
            if ((s >= 0) && (t >= 0) && (s + t <= 1)) {
				SR_CanvasSetPixel(canvas, x, y, colour);
			}
        }
    }
}

void SR_DrawRectOutline(
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
    
    for (unsigned short xi = x; xi < x1; xi++) {
        SR_CanvasSetPixel(canvas, xi, y, colour);
        SR_CanvasSetPixel(canvas, xi, h, colour);
    }
    
    for (unsigned short yi = y + 1; yi < y1; yi++) {
        SR_CanvasSetPixel(canvas, x, yi, colour);
        SR_CanvasSetPixel(canvas, w, yi, colour);
    }
}

void SR_DrawRect(
    SR_Canvas *canvas,
    SR_RGBAPixel colour,
    unsigned short x,
    unsigned short y,
    unsigned short w,
    unsigned short h)
{
    unsigned short x1 = MIN(canvas->width - 1, w + x);
    unsigned short y1 = MIN(canvas->height - 1, h + y);
    
    for (unsigned short yi = y; yi < y1; yi++) {
        for (unsigned short xi = x; xi < x1; xi++) {
            SR_CanvasSetPixel(canvas, xi, yi, colour);
        }
    }
}
