#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <stdio.h>
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define FRAMES_PER_SECOND 100

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;
int mx, my;
int p = 0;
int n = 1;

SDL_Surface *backmenu=NULL;
SDL_Surface *background=NULL;
SDL_Surface *dot=NULL;
SDL_Surface *temp=NULL;
SDL_Surface *screen=NULL;
SDL_Surface *bullet=NULL;
SDL_Surface *empty;
SDL_Rect clip_bul;
SDL_Rect clip_ene [4];
SDL_Rect clip_dot;
SDL_Surface *enemy1=NULL;
SDL_Surface *enemy2=NULL;
SDL_Surface *enemy3=NULL;
SDL_Surface *enemy4=NULL;
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

int check_collision(SDL_Surface *A, SDL_Surface *B, int Ax, int Ay, int Bx, int By)
{
    //The sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    //Calculate the sides of rect A
    leftA = Ax;
    rightA = Ax + A->w;
    topA = Ay;
    bottomA = Ay + A->h;

    //Calculate the sides of rect B
    leftB = Bx;
    rightB = Bx + B->w;
    topB = By;
    bottomB = By + B->h;

    if( bottomA <= topB )
    {
        return 0;
    }

    if( topA >= bottomB )
    {
        return 0;
    }

    if( rightA <= leftB )
    {
        return 0;
    }

    if( leftA >= rightB )
    {
        return 0;
    }

    //If none of the sides from A are outside B
    return 1;
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
int menu ()
{
    SDL_Surface *message = NULL;
    SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB( screen->format, 0xFF, 0xFF, 0xFF ) );
    apply_surface (0, 0, backmenu, screen, NULL);
    message = TTF_RenderText_Solid ( font, "... (START)", textcolor);
    apply_surface ((SCREEN_WIDTH - message->w)/2, 60, message, screen, NULL);
    SDL_FreeSurface (message);
        SDL_Flip (screen);
    if (event.type == SDL_MOUSEMOTION)
    {
        if (n == 1)
        {
            mx = event.motion.x;
            my = event.motion.y;
        }
    }
    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            if ((mx >= (SCREEN_WIDTH - message->w)/2) && (mx <= (SCREEN_WIDTH - message->w)/2 + message->w) && (my >= 60) && (my <= (60 + message->h)))
                return 1;
        }
    }
    return 0;
}

int load_files ()
{
    background = Load_Image ("data/img/background.png");
    dot = Load_Image ("data/img/dot.png");
    bullet = Load_Image ("data/img/bullet.png");
    enemy1 = Load_Image ("data/img/enemy1.png");
    enemy2 = Load_Image ("data/img/enemy2.png");
    enemy3 = Load_Image ("data/img/enemy3.png");
    enemy3 = Load_Image ("data/img/enemy4.png");
    empty = Load_Image ("data/img/empty.png");
    font = TTF_OpenFont ("data/acquestscript.ttf", 36);
    if (background == NULL)
        return 0;
    return 1;
}

void set_clips()
{
}
void clean_up ()
{
    SDL_FreeSurface (background);
    SDL_FreeSurface (dot);
    SDL_FreeSurface (bullet);
    SDL_FreeSurface (enemy1);
    SDL_FreeSurface (enemy2);
    SDL_FreeSurface (enemy3);
    SDL_FreeSurface (enemy4);
    SDL_FreeSurface (temp);
    SDL_FreeSurface (empty);
    TTF_CloseFont (font);
    TTF_Quit();
    SDL_Quit();
}

int main( int argc, char* args[] )
{
    Uint32 start=SDL_GetTicks();
    Uint32 bad_time = SDL_GetTicks();
    int dx = SCREEN_WIDTH/2; /*Координата x точки*/
    int dy = SCREEN_HEIGHT/2; /*Координата y точки*/
    float bx = -100,by = -100; /*Координаты пули*/
    float mx,my; /*Координаты курсора*/
    float xVel, yVel;
    float kVel, b;
    float ex,ey; /*Координаты вражины*/
    int k=0;
    float sin,cos;
    int quit = 0;
    Uint8 *keystates = SDL_GetKeyState(NULL);
    if (init()!=1)
        return 1;
    if (load_files()!=1)
        return 1;
    srand ( (unsigned)time ( NULL ) );
    switch (rand()%4)
    {
        case 2: temp = enemy1; break;
        case 1: temp = enemy2; break;
        case 0: temp = enemy3; break;
        case 3: temp = enemy4; break;
    }
    switch (rand()%4)
    {
        case 0:
        ex = rand()%SCREEN_WIDTH;
        ey = 0;
        break;
        case 2:
        ex = SCREEN_WIDTH;
        ey = rand()%SCREEN_HEIGHT;
        break;
        case 3:
        ex = 0;
        ey = rand()%SCREEN_HEIGHT;
        break;
        case 1:
        ex = SCREEN_WIDTH - 20;
        ey = rand()%SCREEN_HEIGHT;
        break;
    }
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
                    if (k==0)
                    {
                        bx = dx + (dot->w)/2;
                        by = dy + (dot->h)/2;;
                        b=(mx-bx)*(mx-bx) + (my-by)*(my-by);
                        kVel=sqrt (b);
                        sin=(my-by)/kVel;
                        cos=(mx-bx)/kVel;
                        yVel=sin*5;
                        xVel=cos*5;
                        k=1;
                    }
                }
            }
        }
        if (((SDL_GetTicks()-start) < 1000 / FRAMES_PER_SECOND ))
        {
            SDL_Delay( ( 1000 / FRAMES_PER_SECOND ) - (SDL_GetTicks()-start) );
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
        if (dx < ex)
            ex= ex - 2;
        if (dx > ex)
            ex= ex + 2;
        if (dy < ey)
            ey= ey - 2;
        if (dy > ey)
            ey= ey + 2;
        if (check_collision (bullet, temp, bx, by, ex, ey))
        {
           bx = -6000;
           by = -6000;
           ex = -700;
           ey = -700;
        }
        apply_surface (0, 0, background, screen, NULL);
        apply_surface (ex, ey, temp, screen, NULL);
        apply_surface (dx, dy, dot, screen, NULL);
        apply_surface (bx, by, bullet, screen, NULL);
        SDL_Flip (screen);
    }
    clean_up();
    return 0;
}
