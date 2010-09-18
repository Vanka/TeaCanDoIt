#include <SDL/SDL.h>
#include <stdio.h>
#include <string.h>

const int SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 240;
const int SCREEN_BPP = 32;

SDL_Surface *message = NULL;
SDL_Surface *background = NULL;
SDL_Surface *screen = NULL;

SDL_Surface *load_image (char *filename)
{
    SDL_Surface* loadedImage = NULL;
    SDL_Surface* optimizedImage = NULL;
    loadedImage = SDL_LoadBMP (filename);
    if (loadedImage != NULL)
    {
        optimizedImage = SDL_DisplayFormat (loadedImage);
        SDL_FreeSurface (loadedImage);
    }
    if( optimizedImage != NULL )
        {
            //Map the color key
            Uint32 colorkey = SDL_MapRGB( optimizedImage->format, 0xFF, 0xFF, 0xFF );
            //Set all pixels of color R 0xFF, G 0xFF, B 0xFF to be transparent
            SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, colorkey );
        }
    return optimizedImage;
}

void apply_surface (int x, int y, SDL_Surface* source, SDL_Surface* destination)
{
    SDL_Rect offset;
    offset.x =x;
    offset.y = y;
    SDL_BlitSurface (source, NULL, destination, &offset);
}

int main (int argc, char* args[])
{
    if (SDL_Init( SDL_INIT_EVERYTHING ) == -1)
    {
        return 1;
    }
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );
    if (screen == NULL)
    {
        return 1;
    }
    SDL_WM_SetCaption ("Fuuuu", NULL);
    message = load_image ("foo.bmp");
    background = load_image ("background.bmp");
    apply_surface (0, 0, background, screen);
    apply_surface (20, 10, message, screen);
    if (SDL_Flip (screen) == -1)
    {
        return 1;
    }
    SDL_Delay (10000);
    SDL_FreeSurface (message);
    SDL_FreeSurface (background);
    SDL_Quit ();
    return 0;
}
