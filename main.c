/*
===== Voronoi =====
fichier     main.c
But         genereateur de diagrammes de voronoi
Auteur      Adrien du Ranquet
Contact     adrien.duranquet@laposte.net
Date        juin 2013
*/


#define FPS             50
#define FPS_DELAY       (1000/FPS)

#define NB_POINTS       6       /* pour les couleurs, 2^3=8 max (moins 2 pour le noir et le blanc */
#define XMAX            80
#define YMAX            80
#define DELTA           0
/*#define BLACK_AND_WHITE*/ /* pour la version sans couleur */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include "My_SDL_lib.h"

void drawImg(void);
int my_pow(int nb, int pow);
Uint32 getColorFromId(int id);

int manhattan = 1;
int newImg = 1;

void drawImg(void)
{
    SDL_Surface *ecran = NULL, *pixel = NULL;
    int tabX[NB_POINTS];
    int tabY[NB_POINTS];
    int x, y, z, a;
    double dist, distTemp;
    int idColor;

    if (newImg)
    {
        for(x=0; x<NB_POINTS; x++)
        {
            tabX[x] = rand()%XMAX;
            tabY[x] = rand()%YMAX;
        }

        newImg = 0;
    }

    ecran = SDL_GetVideoSurface();
    pixel = my_createSdlSurface(1, 1, 0, 0, 0);
    SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 255, 255, 255));


    /* BOUCLE PRINCIPALE */
    for(y=0; y<YMAX; y++)
    {
        for(x=0; x<YMAX; x++)
        {
            dist = XMAX*YMAX; /* big number */
            a = 1;
            for(z=0; z<NB_POINTS; z++)
            {
                if (manhattan == 1)
                    distTemp = sqrt(my_pow((x-tabX[z]), 2)) + sqrt(my_pow((y-tabY[z]), 2));
                else
                    distTemp = sqrt(my_pow((x-tabX[z]), 2) + my_pow((y-tabY[z]), 2));

                if (distTemp+DELTA < dist)
                {
                    dist = distTemp;
                    a = 1;
                    idColor = z;
                }
                else if (distTemp-DELTA < dist)
                {
                    a++;
                }
            }

            if (a>1)
            {
                #ifdef BLACK_AND_WHITE
                my_blitSurface(pixel, x, y);
                #endif
            }
            else if (a == 1)
            {
                #ifndef BLACK_AND_WHITE
                SDL_FillRect(pixel, NULL, getColorFromId(idColor));
                my_blitSurface(pixel, x, y);
                #endif
            }
        }
    }

    SDL_Flip(ecran);
    SDL_FreeSurface(pixel);
}

int my_pow(int nb, int power)
{
    int result = 1, x;

    for (x=0; x<power; x++)
    {
        result *= nb;
    }

    return result;
}

Uint32 getColorFromId(int id)
{
    id++; /* pour virer le noir */
    return SDL_MapRGB(SDL_GetVideoSurface()->format, 255*((id/4)%2), 255*((id/2)%2), 255*(id%2));
}

int main(int argc, char *argv[])
{
    /* VARIABLES */
    SDL_Surface *ecran = NULL;
    Input in;
    int updateImg = 1;

    /* INIT */
    (void) argc, (void) argv;
    ecran = initSdl(XMAX, YMAX, "Voronoi", NULL);
    srand(time(NULL));
    memset(&in, 0, sizeof in);

    do{
        updateEvent(&in);

        if (in.key[SDLK_KP1])
        {
            in.key[SDLK_KP1] = 0;
            manhattan = 1 - manhattan;
            updateImg = 1;
        }

        if (in.key[SDLK_s])
        {
            in.key[SDLK_s] = 0;
            SDL_SaveBMP(ecran, "ecran.bmp");
        }

        if (in.key[SDLK_n])
        {
            in.key[SDLK_n] = 0;
            newImg = 1;
            updateImg = 1;
        }


        if (updateImg)
        {
            drawImg();
            updateImg = 0;
        }

        SDL_Delay(FPS_DELAY);
    }while (!in.quit);

    /* FREE */
    SDL_Quit();

    return 0;
}
