/*
===== My_SDL_lib =====
fichier     My_SDL_lib.c
But         diverses fonctions bien pratiques
Auteur      Adrien du Ranquet
Date        mars 2013
*/


#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include "My_SDL_lib.h"


/*===== INITIALISATIONS =====*/

SDL_Surface *initSdl(int largeur, int hauteur, char *titre, char *icon)
{
    SDL_Surface *ecran = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("ERREUR d'initialisation de la SDL : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_putenv("SDL_VIDEO_WINDOW_POS=40,60");
    if (titre != NULL) SDL_WM_SetCaption(titre, NULL);
    if (icon != NULL) SDL_WM_SetIcon(my_loadSdlSurface(icon, (Uint32)NULL), NULL);

    ecran = SDL_SetVideoMode(largeur, hauteur, 32, SDL_SWSURFACE);
    if (ecran == NULL)
    {
        printf("ERREUR d'initialisation de l'ecran : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    return ecran;
}



void initTtf(void)
{
    if (TTF_Init() == -1)
    {
        printf("ERREUR d'initialisation de SDL_ttf : %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
}

/*===== LOAD - CREATE =====*/

SDL_Surface *my_loadSdlSurface(char chemin[256], Uint32 flags)
{
    SDL_Surface *tmp = IMG_Load(chemin);

    if (tmp == NULL)
    {
        printf("Erreur d'ouverture de l'image %s : %s\n", chemin, IMG_GetError());
        exit(EXIT_FAILURE);
    }
    else
    {
        SDL_Surface *surface = SDL_DisplayFormat(tmp);

        if (surface == NULL)
        {
            printf("Erreur d'optimisation de l'image %s : %s\n", chemin, IMG_GetError());
            return tmp;
        }
        else
        {
            SDL_FreeSurface(tmp);

            if (flags & MY_LOAD_COLORKEY)
            {
                if (SDL_SetColorKey(surface, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(surface->format, 255, 255, 255)) == -1)
                    printf("Erreur de setColorKey sur l'image %s : %s\n", chemin, IMG_GetError());
            }

            return surface;
        }
    }
}



SDL_Surface* my_createSdlSurface(int largeur, int hauteur, Uint8 r, Uint8 g, Uint8 b)
{
    SDL_Surface *surface = SDL_CreateRGBSurface(SDL_HWSURFACE, largeur, hauteur, 32, 0, 0, 0, 0);

    if (surface == NULL)
    {
        printf("ERREUR de creation d'une surface : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    else
    {
        SDL_FillRect(surface, NULL, SDL_MapRGB(SDL_GetVideoSurface()->format, r, g, b));

        return surface;
    }
}



TTF_Font* my_loadTtfFont(char *chemin, int taille)
{
    TTF_Font *police = TTF_OpenFont(chemin, taille);

    if (police == NULL)
    {
        printf("ERREUR d'ouverture d'une police : %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }

    return police;
}

/*===== SDL_SURFACE =====*/

Uint32 getPixel(SDL_Surface *surface, int x, int y)
{
    Uint32 *pixels = (Uint32*)surface->pixels;

    return pixels[(y*surface->w)+x];
}



void putPixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    Uint32 *pixels = (Uint32*)surface->pixels;

    pixels[(y*surface->w)+x] = pixel;
}



SDL_Surface *flipSurface(SDL_Surface *surface, Uint32 flags)
{
    int x, y, rx, ry;

    SDL_Surface *flipped = SDL_CreateRGBSurface(SDL_SWSURFACE, surface->w, surface->h, surface->format->BitsPerPixel,
                           surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, surface->format->Amask);

    if (SDL_MUSTLOCK(surface)) SDL_LockSurface(surface);
    if (SDL_MUSTLOCK(flipped)) SDL_LockSurface(flipped);

    for(x=0, rx=flipped->w-1; x<flipped->w; x++, rx--)
    {
        for(y=0, ry=flipped->h-1; y<flipped->h; y++, ry--)
        {
            Uint32 pixel = getPixel(surface, x, y);

            if ((flags & MY_FLIP_VERTICAL) && (flags & MY_FLIP_HORIZONTAL))
                putPixel(flipped, rx, ry, pixel);
            else if (flags & MY_FLIP_HORIZONTAL)
                putPixel(flipped, rx, y, pixel);
            else if (flags & MY_FLIP_VERTICAL)
                putPixel(flipped, x, ry, pixel);
        }
    }

    if (SDL_MUSTLOCK(flipped)) SDL_UnlockSurface(flipped);
    if (SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);

    return flipped;
}

/*===== USEFULL FUNCTIONS =====*/

void my_blitSurface(SDL_Surface *surface, int x, int y)
{
    SDL_Rect position;
    position.x = x;
    position.y = y;

    SDL_BlitSurface(surface, NULL, SDL_GetVideoSurface(), &position);
}



void updateEvent(Input *in)
{
    SDL_Event event;

    /* pour eviter "l'inertie" de la souris */
    in->mousexrel = 0;
    in->mouseyrel = 0;

    while (SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_MOUSEBUTTONDOWN:
                in->mousebutton[event.button.button] = 1;
                break;
            case SDL_MOUSEBUTTONUP:
                in->mousebutton[event.button.button] = 0;
                break;
            case SDL_MOUSEMOTION:
                in->mousex = event.motion.x;
                in->mousey = event.motion.y;
                in->mousexrel = event.motion.xrel;
                in->mouseyrel = event.motion.yrel;
                break;
            case SDL_KEYDOWN:
                in->key[event.key.keysym.sym] = 1;
                break;
            case SDL_KEYUP:
                in->key[event.key.keysym.sym] = 0;
                break;
            case SDL_QUIT:
                in->quit = 1;
                in->pause = 0;
                break;
            default:
                break;

        }
        switch(event.button.button)
        {
            case SDL_BUTTON_WHEELUP:
                in->mousebutton[SDL_BUTTON_WHEELUP] = 1;
                break;
            case SDL_BUTTON_WHEELDOWN:
                in->mousebutton[SDL_BUTTON_WHEELDOWN] = 1;
                break;
            default:
                break;
        }
    }

    if (in->key[SDLK_ESCAPE])
    {
        in->pause = !in->pause;
        in->key[SDLK_ESCAPE] = 0;
    }

}



void gererTemps(Temps *temps, Uint32 delay)
{
    int pause;

    temps->present = SDL_GetTicks();
    pause = delay - (temps->present - temps->precedent);

    if (pause >= 0)
    {
        SDL_Delay(pause);
        #ifdef WRITE_TIME_INFO
        printf("Pas d'exeption\n");
        printf("SDL_Delay() [%dms à %d,%ds]\n", pause, temps->present/1000, temps->present%1000);
        #endif
    }
    else
    {
        #ifdef WRITE_TIME_INFO
        printf("SDL_Delay() [%dms à %d,%ds]\n", pause, temps->present/1000, temps->present%1000);
        #endif
    }
    temps->precedent = temps->present;
}

