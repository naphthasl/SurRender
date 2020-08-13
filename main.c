#include <SDL2/SDL.h>
#include "src/surrender.h"

int main(void)
{
    SR_Canvas canvy;
    SDL_Window *win;
    SDL_Event ev;
    int status;

    /* needed for blitting */
    SDL_Surface *wsurf, *canvysurf;
    SDL_Rect destrect;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        return 1;

    if (!(win = SDL_CreateWindow(
        "SurRender demo by rodger",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        640,
        480,
        0))) {
        status = 2;
        goto sdl_quit;
    }

    /* and SR_NewCanvas doesn't have any return code
       whatsoever because it could never fail to allocate
       the canvas itself. it could not possibly happen. */
    canvy = SR_NewCanvas(640, 480);

    if (!(canvysurf = SDL_CreateRGBSurfaceFrom(
        canvy.pixels,
        canvy.width,
        canvy.height,
        24,
        canvy.width * sizeof(SR_RGBPixel),
        0x000000FF,
        0x0000FF00,
        0x00FF0000,
        0x00000000))) {
        status = 4;
        goto sr_destroycanvas;
    }

    /* recompute these after resizing the window
       if you decide to add support for that */
    destrect.x =   0, destrect.y =   0,
    destrect.w = 640, destrect.h = 480;

event_loop:
    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) {
            status = 0;
            goto sdl_freesurf;
        }
    }

    /* update the canvas here, the rest is
       actually blitting it to the window */

    /* refresh the window */
    wsurf = SDL_GetWindowSurface(win);

    if (!wsurf) {
        status = 5;
        goto sdl_freesurf;
    }

    if (SDL_BlitScaled(canvysurf, NULL, wsurf, &destrect) < 0) {
        status = 6;
        goto sdl_freesurf;
    }

    if (SDL_UpdateWindowSurface(win) < 0) {
        status = 7;
        goto sdl_freesurf;
    }

    SDL_Delay(12);
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
