#include "src/surrender.h"

int main(int argc, char *argv[])
{
    SR_Canvas canvas = SR_NewCanvas(640, 480);
    printf("%u\n", SR_CanvasGetPixel(&canvas, 0, 0).red);
}