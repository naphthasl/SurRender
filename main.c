#include <SDL2/SDL.h>
#include "src/surrender.h"
#include <complex.h>

#if defined(__i386__)

static __inline__ unsigned long long rdtsc(void)
{
    unsigned long long int x;
    __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
    return x;
}

#elif defined(__x86_64__)

static __inline__ unsigned long long rdtsc(void)
{
    unsigned hi, lo;
    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
    return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}

#endif

int main(void)
{
    SR_Canvas canvy;
    SDL_Window *win;
    SDL_Event ev;
    int status;

    float aspect_ratio;

    /* needed for blitting */   
    SDL_Surface *wsurf, *canvysurf;
    SDL_Rect destrect;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        return 1;

    /* edit: this is the current way of testing whether
       canvas allocation has failed or not for now */
    canvy = SR_NewCanvas(1366, 768);
    SR_DrawRect(&canvy, SR_CreateRGBA(0, 0, 0, 255), 0, 0, 1366, 768);

    unsigned int times = 65535;
    unsigned long long i = rdtsc();
    while (--times)
        SR_CanvasSetPixel(&canvy, times, times, SR_CreateRGBA(0, 0, 0, 255));
    printf("Set Pixel Ticks: %llu\n", rdtsc() - i);

    SR_Canvas imagetest = SR_ImageFileToCanvas("./PUCK.BMP");
    SR_OffsetCanvas rotcanvas;
    float speeen = 0.0;
    
    //look i'm not sure the 90 deg rots are hecking properly
    //yes i could theoretically just rotate the same tempbox
    //canvas three times but testing so SKDJKALDFJB Goa hgasoiugha
    SR_Canvas boxes = SR_NewCanvas(128, 128);
    SR_ZeroFill(&boxes);
    char filename[] = "TEST_SM.BMP";
    SR_Canvas tempbox0 = SR_ImageFileToCanvas(filename);
    SR_Canvas tempbox1 = SR_ImageFileToCanvas(filename);
    tempbox1 = SR_CanvasRotate(&tempbox1, 90, 0, 0).canvas;
    SR_Canvas tempbox2 = SR_ImageFileToCanvas(filename);
    tempbox2 = SR_CanvasRotate(&tempbox2, 180, 0, 0).canvas;
    SR_Canvas tempbox3 = SR_ImageFileToCanvas(filename);
    tempbox3 = SR_CanvasRotate(&tempbox3, 270, 0, 0).canvas;
    SR_MergeCanvasIntoCanvas(&boxes, &tempbox0,  2,  2, 255, SR_BLEND_REPLACE);
    SR_MergeCanvasIntoCanvas(&boxes, &tempbox1, 42,  2, 255, SR_BLEND_REPLACE);
    SR_MergeCanvasIntoCanvas(&boxes, &tempbox2, 82,  2, 255, SR_BLEND_REPLACE);
    SR_MergeCanvasIntoCanvas(&boxes, &tempbox3,  2, 42, 255, SR_BLEND_REPLACE);
    SR_DestroyCanvas(&tempbox0);
    SR_DestroyCanvas(&tempbox1);
    SR_DestroyCanvas(&tempbox2);
    SR_DestroyCanvas(&tempbox3);
    
    SR_Canvas pokesquish = SR_ImageFileToCanvas("./GOODRA.BMP");
    SR_OffsetCanvas squish;
    
    // free(imagetest.pixels); imagetest.pixels = NULL;

    if (!SR_CanvasIsValid(&canvy)) {
        status = 3;
        goto sdl_destroywin;
    }

    if (!(win = SDL_CreateWindow(
        "SurRender your secrets to zoidberg",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        canvy.width,
        canvy.height,
        SDL_WINDOW_RESIZABLE))) {
        status = 2;
        goto sdl_quit;
    }

    if (!(canvysurf = SDL_CreateRGBSurfaceFrom(
        canvy.pixels,
        canvy.width,
        canvy.height,
        32,
        canvy.width * sizeof(SR_RGBAPixel),
        0x000000FF,
        0x0000FF00,
        0x00FF0000,
        0xFF000000))) {
        status = 4;
        goto sr_destroycanvas;
    }

    /* this rectangle will be recomputed
       every time the window is resized */
    destrect.x = 0, destrect.y = 0,
    destrect.w = canvy.width, destrect.h = canvy.height;

    if (!(wsurf = SDL_GetWindowSurface(win))) {
        status = 5;
        goto sdl_freesurf;
    }

event_loop:
    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) {
            status = 0;
            goto sdl_freesurf;
        }

        if (ev.type == SDL_WINDOWEVENT)
        if (ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
            if (!(wsurf = SDL_GetWindowSurface(win))) {
                status = 5;
                goto sdl_freesurf;
            }

            if ((float)ev.window.data1 / ev.window.data2 >= canvy.ratio)
                aspect_ratio = (float)ev.window.data2 / canvy.height;
            else
                aspect_ratio = (float)ev.window.data1 / canvy.width;

            destrect.w = canvy.width  * aspect_ratio,
            destrect.h = canvy.height * aspect_ratio;

            destrect.x = (ev.window.data1 - destrect.w) * 0.5f,
            destrect.y = (ev.window.data2 - destrect.h) * 0.5f;
        }
    }

    static float minX = -2.0;
    static float maxX = 1.0;
    static float pos = 0;
    float yScale = (((float)maxX-minX) *
        ((float)canvy.height / canvy.width) * canvy.ratio);

    uint8_t shades[] = {
        0, 15, 30, 45, 60, 75, 90, 105, 120, 135, 150, 165, 180, 195, 210,
        225, 240, 255};

    for (unsigned short x = 0; x < canvy.width; x++) {
        for (unsigned short y = 0; y < canvy.height; y++) {
            float complex c = (
                ((float)minX + x * (float)(maxX - minX) / (float)canvy.width) +
                (y * yScale / canvy.height - yScale / 2.0) * I);

            c -= pos * 2; c /= (pos + 1);
            float complex z = c;

            size_t zC;
            for (zC = 0; zC < sizeof(shades); zC++) {
                if (abs(z) > 2) break;

                z = z * z + c;
            }

            SR_CanvasSetPixel(&canvy, x, y, SR_CreateRGBA(
                shades[zC],
                shades[zC],
                shades[zC],
                255));
        }
    }
    pos += 1;
    SR_DrawCircOutline(&canvy, SR_CreateRGBA(255, 0, 0, 255), pos, pos, 69);
    /*
    SR_DrawLine(
        &canvy,
        SR_RGBABlender(
            SR_CreateRGBA(rand(), rand(), rand(), 255),
            SR_CreateRGBA(rand(), rand(), rand(), 255),
            255,
            SR_BLEND_OVERLAY
        ), 
        rand() % (canvy.width),
        rand() % (canvy.height),
        rand() % (canvy.width),
        rand() % (canvy.height)
    );

    SR_MergeCanvasIntoCanvas(
        &canvy, &imagetest,
        rand() % (canvy.width), rand() % (canvy.height),
        255, SR_BLEND_ADDITIVE);

    SR_DrawTri(
        &canvy,
        SR_CreateRGBA(rand(), rand(), rand(), 255),
        rand() % (canvy.width),
        rand() % (canvy.height),
        rand() % (canvy.width),
        rand() % (canvy.height),
        rand() % (canvy.width),
        rand() % (canvy.height)
    );
    
    // temp
    //SR_DrawRect(&canvy, SR_CreateRGBA(0, 0, 0, 255), 0, 0, 1366, 768);

    speeen += 1;
    rotcanvas = SR_CanvasRotate(&imagetest, speeen, 1, 1);
    SR_MergeCanvasIntoCanvas(
        &canvy, &(rotcanvas.canvas),
        128 + rotcanvas.offset_x, 128 + rotcanvas.offset_y,
        //0, 0,
        255, SR_BLEND_ADDITIVE);
    SR_DestroyCanvas(&(rotcanvas.canvas));
    
    squish = SR_CanvasShear(
        &pokesquish,
        sin(speeen * 0.017453292519943295 * 5) * 16,
        0);
    SR_MergeCanvasIntoCanvas(&canvy, &(squish.canvas),
        420 + squish.offset_x, 420, 255, SR_BLEND_ADDITIVE);
    SR_DestroyCanvas(&(squish.canvas));
    
    SR_MergeCanvasIntoCanvas(&canvy, &boxes, 0, 0, 255, SR_BLEND_ADDITIVE);
    */

    /* update the canvas here, the rest is
       actually blitting it to the window */
    
    /* refresh the window */
    if (SDL_FillRect(wsurf, NULL, SDL_MapRGB(wsurf->format, 0, 0, 0)) < 0) {
        status = 6;
        goto sdl_freesurf;
    }

    if (SDL_BlitScaled(canvysurf, NULL, wsurf, &destrect) < 0) {
        status = 7;
        goto sdl_freesurf;
    }

    if (SDL_UpdateWindowSurface(win) < 0) {
        status = 8;
        goto sdl_freesurf;
    }

    //SDL_Delay(0);
    goto event_loop;

sdl_freesurf:
    SDL_FreeSurface(canvysurf);
sr_destroycanvas:
    SR_DestroyCanvas(&canvy);
sdl_destroywin:
    SDL_DestroyWindow(win);
sdl_quit:
    SDL_Quit();
retstatus:
    return status;
}
