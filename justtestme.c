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
int z = 0;
int FOO_RIGHT = 0;
int FOO_LEFT = 1;
int FOO_UP = 2;
int FOO_DOWN = 3;
int FOO_WIDTH = 30;
int FOO_HEIGHT = 30;

SDL_Rect clipsRight[ 4 ];
SDL_Rect clipsLeft[ 4 ];
SDL_Rect clipsUp [ 4 ];
SDL_Rect clipsDown [ 4 ];
SDL_Rect clipEnemy [ 4 ];
SDL_Rect bullets [ 4 ];
SDL_Surface *message = NULL;
SDL_Surface *backmenu=NULL;
SDL_Surface *background=NULL;
SDL_Surface *dot=NULL;
SDL_Surface *temp=NULL;
SDL_Surface *screen=NULL;
SDL_Surface *bullet=NULL;
SDL_Surface *enemy1=NULL;
SDL_Surface *enemy2=NULL;
SDL_Surface *enemy3=NULL;
SDL_Surface *enemy4=NULL;
SDL_Event event;
TTF_Font *font=NULL;
SDL_Color textcolor = {0, 0, 0};
int status=0;
int statul=0;
int frame=0;

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

int check_collision(SDL_Surface *B, int Ax, int Ay, int Bx, int By, int C)
{
    //The sides of the rectangles
    float leftA, leftB;
    float rightA, rightB;
    float topA, topB;
    float bottomA, bottomB;

    //Calculate the sides of rect A
    leftA = Ax;
    rightA = Ax + C;
    topA = Ay;
    bottomA = Ay + C;

    //Calculate the sides of rect B
    leftB = Bx;
    rightB = Bx + B->w;
    topB = By;
    bottomB = By + B->h;

    if ( bottomA <= topB )
    {
        return 0;
    }

    if ( topA >= bottomB )
    {
        return 0;
    }

    if ( rightA <= leftB )
    {
        return 0;
    }

    if ( leftA >= rightB )
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


int load_files ()
{
    background = Load_Image ("data/img/background.png");
    dot = Load_Image ("data/img/dot.png");
    backmenu = Load_Image("data/img/backmenu.bmp");
    bullet = Load_Image ("data/img/bullet.png");
    enemy1 = Load_Image ("data/img/enemy1.png");
    enemy2 = Load_Image ("data/img/enemy2.png");
    enemy3 = Load_Image ("data/img/enemy3.png");
    enemy4 = Load_Image ("data/img/enemy4.png");
    font = TTF_OpenFont ("data/acquestscript.ttf", 36);
    if (background == NULL)
        return 0;
    return 1;
}

void clean_up ()
{
    SDL_FreeSurface (background);
    SDL_FreeSurface (dot);
    SDL_FreeSurface (backmenu);
    SDL_FreeSurface (bullet);
    SDL_FreeSurface (enemy1);
    SDL_FreeSurface (enemy2);
    SDL_FreeSurface (enemy3);
    SDL_FreeSurface (enemy4);
    TTF_CloseFont (font);
    TTF_Quit();
    SDL_Quit();
}

void set_clips()
{
    bullets[ 0 ].x = 0;
    bullets[ 0 ].y = 0;
    bullets[ 0 ].w = 8;
    bullets[ 0 ].h = 8;

    bullets[ 1 ].x = 8;
    bullets[ 1 ].y = 0;
    bullets[ 1 ].w = 8;
    bullets[ 1 ].h = 8;

    bullets[ 2 ].x = 0;
    bullets[ 2 ].y = 8;
    bullets[ 2 ].w = 8;
    bullets[ 2 ].h = 8;

    bullets[ 3 ].x = 8;
    bullets[ 3 ].y = 8;
    bullets[ 3 ].w = 8;
    bullets[ 3 ].h = 8;

    //Clip the sprites
    clipsRight[ 0 ].x = 0;
    clipsRight[ 0 ].y = 0;
    clipsRight[ 0 ].w = FOO_WIDTH;
    clipsRight[ 0 ].h = FOO_HEIGHT;

    clipsRight[ 1 ].x = FOO_WIDTH;
    clipsRight[ 1 ].y = 0;
    clipsRight[ 1 ].w = FOO_WIDTH;
    clipsRight[ 1 ].h = FOO_HEIGHT;

    clipsRight[ 2 ].x = FOO_WIDTH * 2;
    clipsRight[ 2 ].y = 0;
    clipsRight[ 2 ].w = FOO_WIDTH;
    clipsRight[ 2 ].h = FOO_HEIGHT;

    clipsRight[ 3 ].x = FOO_WIDTH * 3;
    clipsRight[ 3 ].y = 0;
    clipsRight[ 3 ].w = FOO_WIDTH;
    clipsRight[ 3 ].h = FOO_HEIGHT;

    clipsLeft[ 0 ].x = 0;
    clipsLeft[ 0 ].y = FOO_HEIGHT;
    clipsLeft[ 0 ].w = FOO_WIDTH;
    clipsLeft[ 0 ].h = FOO_HEIGHT;

    clipsLeft[ 1 ].x = FOO_WIDTH;
    clipsLeft[ 1 ].y = FOO_HEIGHT;
    clipsLeft[ 1 ].w = FOO_WIDTH;
    clipsLeft[ 1 ].h = FOO_HEIGHT;

    clipsLeft[ 2 ].x = FOO_WIDTH * 2;
    clipsLeft[ 2 ].y = FOO_HEIGHT;
    clipsLeft[ 2 ].w = FOO_WIDTH;
    clipsLeft[ 2 ].h = FOO_HEIGHT;

    clipsLeft[ 3 ].x = FOO_WIDTH * 3;
    clipsLeft[ 3 ].y = FOO_HEIGHT;
    clipsLeft[ 3 ].w = FOO_WIDTH;
    clipsLeft[ 3 ].h = FOO_HEIGHT;

    clipsUp[ 0 ].x = 0;
    clipsUp[ 0 ].y = FOO_HEIGHT * 2;
    clipsUp[ 0 ].w = FOO_WIDTH;
    clipsUp[ 0 ].h = FOO_HEIGHT;

    clipsUp[ 1 ].x = FOO_WIDTH;
    clipsUp[ 1 ].y = FOO_HEIGHT * 2;
    clipsUp[ 1 ].w = FOO_WIDTH;
    clipsUp[ 1 ].h = FOO_HEIGHT;

    clipsUp[ 2 ].x = FOO_WIDTH * 2;
    clipsUp[ 2 ].y = FOO_HEIGHT * 2;
    clipsUp[ 2 ].w = FOO_WIDTH;
    clipsUp[ 2 ].h = FOO_HEIGHT;

    clipsUp[ 3 ].x = FOO_WIDTH * 3;
    clipsUp[ 3 ].y = FOO_HEIGHT * 2;
    clipsUp[ 3 ].w = FOO_WIDTH;
    clipsUp[ 3 ].h = FOO_HEIGHT;

    clipsDown[ 0 ].x = 0;
    clipsDown[ 0 ].y = FOO_HEIGHT * 3;
    clipsDown[ 0 ].w = FOO_WIDTH;
    clipsDown[ 0 ].h = FOO_HEIGHT;

    clipsDown[ 1 ].x = FOO_WIDTH ;
    clipsDown[ 1 ].y = FOO_HEIGHT * 3;
    clipsDown[ 1 ].w = FOO_WIDTH;
    clipsDown[ 1 ].h = FOO_HEIGHT;

    clipsDown[ 2 ].x = FOO_WIDTH * 2;
    clipsDown[ 2 ].y = FOO_HEIGHT * 3;
    clipsDown[ 2 ].w = FOO_WIDTH;
    clipsDown[ 2 ].h = FOO_HEIGHT;

    clipsDown[ 3 ].x = FOO_WIDTH * 3;
    clipsDown[ 3 ].y = FOO_HEIGHT * 3;
    clipsDown[ 3 ].w = FOO_WIDTH;
    clipsDown[ 3 ].h = FOO_HEIGHT;
}

int menu ()
{
    apply_surface (0, 0, backmenu, screen, NULL);
    message = TTF_RenderText_Solid ( font, "... (START)", textcolor);
    apply_surface ((SCREEN_WIDTH/2 - message->w/2), (SCREEN_HEIGHT/2 - message->h/2), message, screen, NULL);
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
            if ((mx >= (SCREEN_WIDTH - message->w)/2) && (mx <= (SCREEN_WIDTH - message->w)/2 + message->w) && (my >= (SCREEN_HEIGHT - message->h)/2) && (my <= ((SCREEN_HEIGHT - message->h)/2) + message->h))
                return 1;
        }
    }
    SDL_FreeSurface (message);
    return 0;
}

int main( int argc, char* args[] )
{
    float growth = 3;
    int checking = 0; /*Параметр, чтобы выстрел не проиходил при  нажатии кнопки "Start"*/
    int check_enemy = 0;
    int m, frame_bul = 0, frameme;
    int scory = 0;
    char score [10], timer [20], gameover [150];
    Uint32 start=SDL_GetTicks();
    Uint32 begin = SDL_GetTicks();
    int dx = SCREEN_WIDTH/2; /*Координата x точки*/
    int dy = SCREEN_HEIGHT/2; /*Координата y точки*/
    float bx = -100,by = -100; /*Координаты пули*/
    float mx,my; /*Координаты курсора*/
    float xVel, yVel;
    float kVel, b;
    float ex,ey; /*Координаты вражины*/
    int k=0;
    float sin,cos;
    set_clips();
    int quit = 0;
    Uint8 *keystates = SDL_GetKeyState(NULL);
    if (init()!=1)
        return 1;
    if (load_files()!=1)
        return 1;
    srand ( (unsigned)time ( NULL ) );
    while (quit == 0)
    {
        if (check_enemy == 0)
        {
            switch (rand()%4)
            {
            case 2:
                temp = enemy1;
                frameme = 2;
                break;
            case 1:
                temp = enemy2;
                frameme = 3;
                break;
            case 0:
                temp = enemy3;
                frameme = 1;
                break;
            case 3:
                temp = enemy4;
                frameme = 0;
                break;
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
            check_enemy = 1;
        }
        if (z == 0)
        {
            if (menu() == 1)
            {
                z = 1;
            }
        }
        start = SDL_GetTicks();
        while (SDL_PollEvent (&event))
        {
            if (event.type == SDL_QUIT)
                quit = 1;
            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    quit = 1;
                if (m == 1)
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
                    if (k==0 && checking == 1)
                    {
                        bx = dx + 10;
                        by = dy + 10;
                        b=(mx-bx)*(mx-bx) + (my-by)*(my-by);
                        kVel=sqrt (b);
                        sin=(my-by)/kVel;
                        cos=(mx-bx)/kVel;
                        yVel=sin*5;
                        xVel=cos*5;
                        k=1;
                    }
                }
                if (event.button.button == SDL_BUTTON_RIGHT)
                    frame_bul ++;
            }
        }
        if (((SDL_GetTicks()-start) < 1000 / FRAMES_PER_SECOND ))
        {
            SDL_Delay( ( 1000 / FRAMES_PER_SECOND ) - (SDL_GetTicks()-start) );
        }
        if (z == 1)
        {
            if ((keystates[SDLK_UP]) || (keystates[SDLK_w]))
            {
                dy -= 4;
                status = FOO_UP;
                if (dy % 5 == 0)
                    frame++;
            }
            if ((keystates[SDLK_DOWN]) || (keystates[SDLK_s]))
            {
                dy += 4;
                status = FOO_DOWN;
                if (dy % 5 == 0)
                    frame++;
            }
            if ((keystates[SDLK_LEFT]) || (keystates[SDLK_a]))
            {
                dx -= 4;
                status = FOO_LEFT;
                if (dx % 5 == 0)
                    frame++;
            }
            if ((keystates[SDLK_RIGHT]) || (keystates[SDLK_d]))
            {
                dx += 4;
                status = FOO_RIGHT;
                if (dx % 5 == 0)
                    frame++;
            }
            if ( frame >= 3 )
            {
                frame = 0;
            }
            if ( dx <= 0 )
            {
            dx =  0;
            }
            if ( dx >= SCREEN_WIDTH - FOO_WIDTH)
            {
                dx = SCREEN_WIDTH - FOO_WIDTH;
            }
            if ( dy <= 0 )
            {
                dy =  0;
            }
            if ( dy >= SCREEN_HEIGHT - FOO_HEIGHT)
            {
                dy = SCREEN_HEIGHT - FOO_HEIGHT;
            }
            bx+=xVel;
            by+=yVel;
            if ((bx>=SCREEN_WIDTH) || (bx <= 0 - bullet->w) || (by >= SCREEN_HEIGHT) || (by <= 0 - bullet->h))
                k=0;
            b = - sqrt ((ex-dx)*(ex-dx) + (ey-dy)*(ey-dy));
            sin=(ex-dx)/b;
            cos=(ey-dy)/b;
            growth = scory/100 + 3;
            ey = ey + cos*growth;
            ex = ex + sin*growth;
            if (((check_collision (bullet, temp, bx, by, ex, ey, 8))==1) && (frameme == frame_bul))
            {
                temp = NULL;
                bx = -874;
                by = -972;
                check_enemy = 0;
                scory +=10;
            }

        }
        if (z == 1)
        {
            if (temp!=NULL)
            {
            if (check_collision (dot, temp, dx, dx, ex, ey, 30))
            {
                snprintf (gameover, sizeof (gameover), "You've got %d score for %d seconds. Now your game is over.", scory, (SDL_GetTicks() - begin)/1000);
                message = TTF_RenderText_Solid (font, gameover, textcolor);
                apply_surface (SCREEN_WIDTH/2 - message->w/2, SCREEN_HEIGHT/2 - message->h/2, message, screen, NULL);
                SDL_Flip (screen);
                SDL_Delay (4000);
                quit = 1;
            }
            }
            snprintf (timer, sizeof (start), "%d", (SDL_GetTicks() - begin)/1000);
            snprintf (score, sizeof (score), "%d", scory);
            message = TTF_RenderText_Solid (font, score, textcolor);
            apply_surface (0, 0, background, screen, NULL);
            apply_surface (0, 0, bullet, screen, &bullets[frame_bul]);
            if ( status == FOO_RIGHT )
            {
                apply_surface( dx, dy, dot, screen, &clipsRight[ frame ] );
            }
            else if ( status == FOO_LEFT )
            {
                apply_surface( dx, dy, dot, screen, &clipsLeft[ frame ] );
            }
            else if ( status == FOO_UP )
            {
                apply_surface( dx, dy, dot, screen, &clipsUp[ frame ] );
            }
            else if ( status == FOO_DOWN )
            {
                apply_surface( dx, dy, dot, screen, &clipsDown[ frame ] );
            }
            apply_surface (SCREEN_WIDTH - message->w, 0, message, screen, NULL);
            SDL_FreeSurface (message);
            message = TTF_RenderText_Solid (font, timer, textcolor);
            apply_surface (SCREEN_WIDTH/2 - message->w/2, 0, message, screen, NULL);
            if (checking != 1)
                checking = 1;
            if (frame_bul>=4)
                frame_bul = 0;
            if (temp != NULL)
            {
                apply_surface (ex, ey, temp, screen, NULL);
                apply_surface (bx, by, bullet, screen, &bullets[frame_bul]);
            }
            SDL_Flip (screen);
        }
    }
    clean_up();
    return 0;
}
