#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <stdio.h>
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"

#define FRAMES_PER_SECOND 40

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

SDL_Surface *background=NULL;
SDL_Surface *dot=NULL;
SDL_Surface *screen=NULL;
SDL_Surface *bullet=NULL;
SDL_Event event;
TTF_Font *font=NULL;
SDL_Color textcolor = {255, 255, 255};

SDL_Surface *Load_Image (char *filename)
{
    SDL_Surface *Loaded_Image=NULL;
    SDL_Surface *Optimized_Image=NULL;
    Loaded_Image = IMG_Load (filename);
    if (Loaded_Image != NULL)
    {
        Optimized_Image = SDL_DisplayFormat (Loaded_Image);
        SDL_FreeSurface (Loaded_Image);
        if (Optimized_Image != NULL)
        {
            Uint32 colorkey = SDL_MapRGB (Optimized_Image->format, 0xFF, 0xFF, 0xFF);
            SDL_SetColorKey (Optimized_Image, SDL_SRCCOLORKEY, colorkey);
        }
    }
    return Optimized_Image;
}

void apply_surface (int x, int y, SDL_Surface *source, SDL_Surface *destination)
{
    SDL_Rect offset;
    offset.x = x;
    offset.y = y;
    SDL_BlitSurface (source, NULL, destination, &offset);
}

int init ()
{
    if (SDL_Init (SDL_INIT_EVERYTHING) == -1)
        return 0;
    screen = SDL_SetVideoMode (SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);
    if (screen == NULL)
        return 0;
    if (TTF_Init() == -1)
        return 0;
    SDL_WM_SetCaption (">_<", "data/img/icon.bmp");
    SDL_WM_SetIcon (Load_Image ("data/img/icon.bmp"), NULL);
    return 1;
}

int load_files ()
{
    background = Load_Image ("data/img/background.png");
    dot = Load_Image ("data/img/dot.png");
    bullet = Load_Image ("data/img/bullet.png");
    font = TTF_OpenFont ("data/acquestscript.ttf", 36);
    if (background == NULL)
        return 0;
    return 1;
}

void clean_up ()
{
    SDL_FreeSurface (background);
    SDL_FreeSurface (dot);
    SDL_FreeSurface (bullet);
    TTF_CloseFont (font);
    TTF_Quit();
    SDL_Quit();
}

int main( int argc, char* args[] )
{
    int cap = 1;
    Uint32 start=SDL_GetTicks();
    int dx = SCREEN_WIDTH/2; /*Координата x точки*/
    int dy = SCREEN_HEIGHT/2; /*Координата y точки*/
    int bx,by; /*Координаты пули*/
    int mx,my; /*Координаты курсора*/
    int xVel, yVel;
    int kVel, b;
    int quit = 0;
    Uint8 *keystates = SDL_GetKeyState(NULL);
    if (init()!=1)
        return 1;
    if (load_files()!=1)
        return 1;
    while (quit == 0)
    {
        start = SDL_GetTicks();
        while (SDL_PollEvent (&event))
        {
            if (event.type == SDL_QUIT)
                quit = 1;
            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    quit = 1;
            }
            if (event.type == SDL_MOUSEMOTION)
            {
                mx = event.motion.x;
                my = event.motion.y;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    bx = dx + (dot->w)/2;
                    by = dy + (dot->h)/2;;
                    kVel=sqrt(mx*mx + my*my)/10;
                    xVel = (mx-bx)/kVel;
                    yVel = (my-by)/kVel;
                }
            }
        }
        if ((keystates[SDLK_UP]) || (keystates[SDLK_w]))
            dy -= 5;
        if ((keystates[SDLK_DOWN]) || (keystates[SDLK_s]))
            dy += 5;
        if ((keystates[SDLK_LEFT]) || (keystates[SDLK_a]))
            dx -= 5;
        if ((keystates[SDLK_RIGHT]) || (keystates[SDLK_d]))
            dx += 5;
        if ( dx <= 0 )
        {
            dx = 0;
        }
        if ( dx >= SCREEN_WIDTH - dot->w)
        {
            dx = SCREEN_WIDTH - dot->w;
        }
        if ( dy <= 0 )
        {
            dy = 0;
        }
        if ( dy >= SCREEN_HEIGHT - dot->h)
        {
            dy = SCREEN_HEIGHT - dot->h;
        }
        //If we want to cap the frame rate
        if( (cap == 1) && ((SDL_GetTicks()-start) < 1000 / FRAMES_PER_SECOND ))
        {
            //Sleep the remaining frame time
            SDL_Delay( ( 1000 / FRAMES_PER_SECOND ) - (SDL_GetTicks()-start) );
        }
        bx+=xVel;
        by+=yVel;
        apply_surface (0, 0, background, screen);
        apply_surface (dx, dy, dot, screen);
        apply_surface (bx, by, bullet, screen);
        SDL_Flip (screen);
    }
    clean_up();
    return 0;
}
