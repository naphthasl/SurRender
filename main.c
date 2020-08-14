#include <SDL2/SDL.h>
#include "src/surrender.h"

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
    canvy = SR_NewCanvas(256, 256);

    if (!SR_CanvasIsValid(&canvy)) {
        status = 3;
        goto sdl_destroywin;
    }

    if (!(win = SDL_CreateWindow(
        "SurRender demo by rodger",
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

    // For test
    unsigned short x, y, z;

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

    for (x = 0; x <= canvy.width - 1; x++) {
        for (y = 0; y <= canvy.height - 1; y++) {
            SR_CanvasSetPixel(&canvy, x, y, SR_CreateRGBA(
                x,
                y,
                z,
                255
            ));
        }
    }
    z++;

    SR_DrawLine(
        &canvy,
        SR_RGBABlender(
            SR_CreateRGBA(128, 0, 0, 255),
            SR_CreateRGBA(128, 255, 0, 255),
            255,
            SR_BLEND_ADDITIVE
        ), 
        0, 0, rand() % (canvy.width), rand() % (canvy.height));

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

    SDL_Delay(40);
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
