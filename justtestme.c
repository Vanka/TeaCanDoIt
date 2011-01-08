#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_net.h"
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
float mx, my;
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
int audio_rate = 22050;
Uint16 audio_format = AUDIO_S16;
int audio_channels = 2;
int audio_buffers = 4096;
int buletChannel = -1;
Mix_Music *music=NULL;
Mix_Chunk *bulet = NULL;
Mix_Music *music1 = NULL;

SDL_Surface *bonus = NULL;
SDL_Surface *bonus_bul = NULL;
SDL_Surface *bonus_dot = NULL;
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
SDL_Color textcolor1 = {255, 255, 255};
int status=0;
int statul=0;
int frame=0;
int volume = 50;
TCPsocket sd, csd; /* Socket descriptor, Client socket descriptor */
IPaddress ip, *remoteIP;

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

int check_collision(float Ax, float Ay, float Bx, float By, float A, float B)
{
    //The sides of the rectangles
    float leftA, leftB;
    float rightA, rightB;
    float topA, topB;
    float bottomA, bottomB;

    //Calculate the sides of rect A
    leftA = Ax;
    rightA = Ax + A;
    topA = Ay;
    bottomA = Ay + A;

    //Calculate the sides of rect B
    leftB = Bx;
    rightB = Bx + B;
    topB = By;
    bottomB = By + B;

    //If none of the sides from A are outside B
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
    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
        return 0;
    if (Mix_Init(MIX_INIT_MP3) == -1)
        return 0;
    if (SDLNet_Init() == -1)
        return 0;
    SDL_WM_SetCaption (">_<", "data/img/icon.bmp");
    SDL_WM_SetIcon (Load_Image ("data/img/icon.bmp"), NULL);
    return 1;
}

int load_files ()
{
    bonus_bul = Load_Image ("data/img/bonus_bul.jpg");
    bonus_dot = Load_Image ("data/img/bonus_dot.png");
    bulet = Mix_LoadWAV( "data/music/mes.wav" );
    music = Mix_LoadMUS ("data/music/call.mp3");
    background = Load_Image ("data/img/background.png");
    dot = Load_Image ("data/img/dot.png");
    backmenu = Load_Image("data/img/backmenu.bmp");
    bullet = Load_Image ("data/img/bullet.png");
    enemy1 = Load_Image ("data/img/enemy1.png");
    enemy2 = Load_Image ("data/img/enemy2.png");
    enemy3 = Load_Image ("data/img/enemy3.png");
    enemy4 = Load_Image ("data/img/enemy4.png");
    font = TTF_OpenFont ("data/font/245.ttf", 25);
    if (background == NULL)
        return 0;
    return 1;
}

void clean_up ()
{
    Mix_FreeMusic (music1);
    Mix_FreeChunk (bulet);
    Mix_FreeMusic (music);
    SDL_FreeSurface (background);
    SDL_FreeSurface (dot);
    SDL_FreeSurface (backmenu);
    SDL_FreeSurface (bullet);
    SDL_FreeSurface (enemy1);
    SDL_FreeSurface (enemy2);
    SDL_FreeSurface (enemy3);
    SDL_FreeSurface (enemy4);
    Mix_CloseAudio();
    TTF_CloseFont (font);
    TTF_Quit();
    SDLNet_Quit();
    SDL_Quit();
    Mix_Quit();
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
     char voltext[20];
     Uint8 *keystates = SDL_GetKeyState(NULL);
     if (keystates[SDLK_1])
            {
                volume = volume - 1;
                if (volume < 10)
                    volume = 0;
            }
     if (keystates[SDLK_2])
            {
                volume = volume + 1;
                if (volume > 100)
                    volume = 100;
            }
    Mix_VolumeMusic(volume);
    apply_surface (0, 0, backmenu, screen, NULL);
    message = TTF_RenderText_Solid ( font, "Start", textcolor1);
    apply_surface ((SCREEN_WIDTH/2 - message->w/2), (SCREEN_HEIGHT/2 - message->h/2), message, screen, NULL);
    SDL_FreeSurface(message);
    snprintf (voltext, sizeof (voltext), "Music: %d", volume);
    message = TTF_RenderText_Solid (font, voltext, textcolor1);
    apply_surface (SCREEN_WIDTH -  message->w, 0, message, screen, NULL);
    SDL_FreeSurface (message);
    message = TTF_RenderText_Solid ( font, "Exit", textcolor1);
    apply_surface ((SCREEN_WIDTH/2 - message->w/2), (SCREEN_HEIGHT - message->h), message, screen, NULL);
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
                if ((mx >= (SCREEN_WIDTH - message->w)/2) && (mx <= (SCREEN_WIDTH - message->w)/2 + message->w) && (my >= (SCREEN_HEIGHT - message->h)) && (my <= ((SCREEN_HEIGHT - message->h)+ message->h)))
                    return 2;
            }
    }
    SDL_FreeSurface (message);
    return 0;
}

void webnet(int host, char* args[])
{
    if (host == 1)
        SDLNet_ResolveHost(&ip, NULL, 2000);
    if (host == 0)
        SDLNet_ResolveHost(&ip, args[1], atoi(args[2]));
    sd = SDLNet_TCP_Open(&ip);
}

int main( int argc, char* args[] )
{
    int speed_bul = 6; //Скорость пули
    int speed_dot = 4; //Скорость персонажика
    float growth = 3;
    int checking = 0; /*Параметр, чтобы выстрел не проиходил при  нажатии кнопки "Start"*/
    int check_enemy = 0;
    int m, frame_bul = 0, frameme;
    int scory = 0;
    char score [10], timer [20], gameover [80], voltext[20];
    Uint32 start=SDL_GetTicks();
    Uint32 begin = SDL_GetTicks();
    Uint32 bonus_time = SDL_GetTicks();
    Uint32 bonus_dur = SDL_GetTicks();
    int dx = SCREEN_WIDTH/2; /*Координата x точки*/
    int dy = SCREEN_HEIGHT/2; /*Координата y точки*/
    float bx = -100,by = -100; /*Координаты пули*/
    float mx,my; /*Координаты курсора*/
    float xVel, yVel;
    float kVel, b;
    float bonus_x, bonus_y;
    int bonus_stat, bonus_check=0;
    float ex,ey; /*Координаты вражины*/
    int k=0;
    float sin,cos;
    int quit = 0;
    Uint8 *keystates = SDL_GetKeyState(NULL);
    set_clips();
    if (init()!=1)
        return 1;
    if (load_files()!=1)
        return 1;
    Mix_PlayMusic (music, -1);
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
        if (bonus_check == 0 && (SDL_GetTicks() - begin)%500 == 0 && (SDL_GetTicks() - begin)!=0)
        {
            switch (rand()%2)
            {
                case 0:
                    bonus_x = rand()%(SCREEN_WIDTH - 20);
                    bonus_y = rand()%(SCREEN_HEIGHT - 20)+20;
                    bonus = bonus_bul;
                    bonus_stat = 0;
                    break;
                case 1:
                    bonus_x = rand()%(SCREEN_WIDTH - 20);
                    bonus_y = rand()%(SCREEN_HEIGHT - 20)+20;
                    bonus = bonus_dot;
                    bonus_stat = 1;
                    break;
            }
            bonus_check = 1;
            bonus_time = SDL_GetTicks();
        }
        if (z == 0)
        {
            if (menu() == 2)
                quit = 1;
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
                        bx = dx + 15;
                        by = dy + 15;
                        b=(mx-bx)*(mx-bx) + (my-by)*(my-by);
                        kVel=sqrt (b);
                        sin=(my-by)/kVel;
                        cos=(mx-bx)/kVel;
                        yVel=sin*speed_bul;
                        xVel=cos*speed_bul;
                        k=1;
                        buletChannel = Mix_PlayChannel(-1, bulet, 0);
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
            if (keystates[SDLK_1])
            {
                volume = volume - 1;
                if (volume < 10)
                    volume = 0;
            }
            if (keystates[SDLK_2])
            {
                volume = volume + 1;
                if (volume > 100)
                    volume = 100;
            }
            if ((keystates[SDLK_UP]) || (keystates[SDLK_w]))
            {
                dy -= speed_dot;
                status = FOO_UP;
                if (dy % 5 == 0)
                    frame++;
            }
            if ((keystates[SDLK_DOWN]) || (keystates[SDLK_s]))
            {
                dy += speed_dot;
                status = FOO_DOWN;
                if (dy % 5 == 0)
                    frame++;
            }
            if ((keystates[SDLK_LEFT]) || (keystates[SDLK_a]))
            {
                dx -= speed_dot;
                status = FOO_LEFT;
                if (dx % 5 == 0)
                    frame++;
            }
            if ((keystates[SDLK_RIGHT]) || (keystates[SDLK_d]))
            {
                dx += speed_dot;
                status = FOO_RIGHT;
                if (dx % 5 == 0)
                frame++;
            }
            if( dx <= 0 )
            {
                dx =  0;
            }
            if ( dx >= SCREEN_WIDTH - FOO_WIDTH)
            {
                dx = SCREEN_WIDTH - FOO_WIDTH;
            }
            if ( dy >= SCREEN_HEIGHT - FOO_HEIGHT)
            {
                dy = SCREEN_HEIGHT - FOO_HEIGHT;
            }
            if ( dy <= 20 )
            {
                dy = 20;
            }
            bx+=xVel;
            by+=yVel;
            if ((bx>=SCREEN_WIDTH) || (bx <= 0 - bullet->w) || (by >= SCREEN_HEIGHT) || (by <= 0 - bullet->h))
                k=0;
            b = - sqrt ((ex-dx)*(ex-dx) + (ey-dy)*(ey-dy));
            sin=(ex-dx)/b;
            cos=(ey-dy)/b;
            growth = (scory/100)/2 + 3;
            ey = ey + cos*growth;
            ex = ex + sin*growth;
            if (((check_collision (bx, by, ex, ey, 8, 20))==1) && (frameme == frame_bul))
            {
                temp = NULL;
                bx = -874;
                by = -972;
                check_enemy = 0;
                scory +=10;
            }
        }
        if( frame >= 4 )
        {
            frame = 0;
        }
        if (z == 1)
        {
            if (check_collision (dx, dy, bonus_x, bonus_y, 30, 20))
            {
                if (bonus_stat == 1)
                {
                    speed_dot = 8;
                }
                if (bonus_stat == 0)
                {
                    speed_bul = 12;
                }
                bonus_dur = SDL_GetTicks();
                bonus = NULL;
                bonus_check = 0;
                bonus_x = -900;
                bonus_y = -900;
            }
            if (((SDL_GetTicks() - bonus_dur)%300 == 0) && (SDL_GetTicks() - bonus_dur)!=0)
            {
                speed_bul = 6;
                speed_dot = 4;
            }
            if (check_collision (dx, dy, ex, ey, 30, 20) == 1)
            {
                snprintf (gameover, sizeof (gameover), "You've got %d score for %d seconds.", scory, (SDL_GetTicks() - begin)/1000);
                message = TTF_RenderText_Solid (font, gameover, textcolor);
                apply_surface (SCREEN_WIDTH/2 - message->w/2, SCREEN_HEIGHT/2 - message->h/2, message, screen, NULL);
                SDL_FreeSurface (message);
                message = TTF_RenderText_Solid (font, "Now your game is over", textcolor);
                apply_surface (SCREEN_WIDTH/2 - message->w/2, SCREEN_HEIGHT/2 + message->h/2, message, screen, NULL);
                SDL_Flip (screen);
                SDL_Delay (4000);
                quit = 1;
            }
            Mix_VolumeMusic (volume);
            snprintf (timer, sizeof (start), "%d", (SDL_GetTicks() - begin)/1000);
            snprintf (score, sizeof (score), "%d", scory);
            snprintf (voltext, sizeof (voltext), "Music: %d", volume);
            message = TTF_RenderText_Solid (font, score, textcolor1);
            apply_surface (0, 0, background, screen, NULL);
            apply_surface (4, 4, bullet, screen, &bullets[frame_bul]);
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
            message = TTF_RenderText_Solid (font, timer, textcolor1);
            apply_surface (SCREEN_WIDTH/2 - message->w/2, 0, message, screen, NULL);
            SDL_FreeSurface (message);
            message = TTF_RenderText_Solid (font, voltext, textcolor1);
            apply_surface (30, 0, message, screen, NULL);
            if (((SDL_GetTicks() - bonus_time)%200 == 0) && (SDL_GetTicks() - bonus_dur)!=0)
            {
                bonus_x = -900;
                bonus_y = -900;
                bonus = NULL;
                bonus_check = 0;
            }
            if (checking != 1)
                checking = 1;
            if (frame_bul>=4)
                frame_bul = 0;
            apply_surface (bonus_x, bonus_y, bonus, screen, NULL);
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
