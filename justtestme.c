#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <stdio.h>
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"
#include <math.h>

#define FRAMES_PER_SECOND 40

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

SDL_Rect clip;
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

void apply_surface (int x, int y, SDL_Surface *source, SDL_Surface *destination, SDL_Rect *clip)
{
    SDL_Rect offset;
    offset.x = x;
    offset.y = y;
    SDL_BlitSurface (source, clip, destination, &offset);
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
void set_clip()
{
    clip.x=0;
    clip.y=0;
    clip.h=8;
    clip.w=8;
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
    int dx = SCREEN_WIDTH/2; /*Координата x точки*/
    int dy = SCREEN_HEIGHT/2; /*Координата y точки*/
    float bx,by; /*Координаты пули*/
    float mx,my; /*Координаты курсора*/
    float xVel, yVel;
    float kVel, b;
    int k=0;
    float sin,cos;
    set_clip();
    int quit = 0;
    Uint8 *keystates = SDL_GetKeyState(NULL);
    if (init()!=1)
        return 1;
    if (load_files()!=1)
        return 1;
    while (quit == 0)
    {
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
                    if (k==0)
                    {
                        bx = dx + (dot->w)/2;
                        by = dy + (dot->h)/2;;
                        b=mx*mx + my*my;
                        kVel=sqrt (b);
                        sin=(my-by)/kVel;
                        cos=(mx-bx)/kVel;
                        yVel=sin*3;
                        xVel=cos*3;
                        k=1;
                    }
                }
            }
        }
        if ((keystates[SDLK_UP]) || (keystates[SDLK_w]))
            dy -= 3;
        if ((keystates[SDLK_DOWN]) || (keystates[SDLK_s]))
            dy += 3;
        if ((keystates[SDLK_LEFT]) || (keystates[SDLK_a]))
            dx -= 3;
        if ((keystates[SDLK_RIGHT]) || (keystates[SDLK_d]))
            dx += 3;
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
        bx+=xVel;
        by+=yVel;
        if ((bx>=SCREEN_WIDTH) || (bx <= 0 - bullet->w) || (by >= SCREEN_HEIGHT) || (by <= 0 - bullet->h))
            k=0;
        apply_surface (0, 0, background, screen, NULL);
        apply_surface (dx, dy, dot, screen, NULL);
        apply_surface (bx, by, bullet, screen, NULL);
        SDL_Flip (screen);
    }
    clean_up();
    return 0;
}
