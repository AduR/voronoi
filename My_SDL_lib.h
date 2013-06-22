#ifndef MY_SDL_LIB_H_INCLUDED
#define MY_SDL_LIB_H_INCLUDED

    /* DEFINES */
    /*#define WRITE_TIME_INFO*/

    /* FLAGS */
    #define MY_LOAD_COLORKEY   (1<<0)   /*1*/

    #define MY_FLIP_VERTICAL   (1<<0)   /*1*/
    #define MY_FLIP_HORIZONTAL (1<<1)   /*2*/

    /* STRUCT */
    typedef struct{
        char    key[SDLK_LAST];
        int     mousex, mousey;
        int     mousexrel, mouseyrel;
        char    mousebutton[8];
        char    quit;
        char    pause;
    }Input;

    typedef struct{
        Uint32  precedent;
        Uint32  present;
    }Temps;

    /* FUNCTIONS */
    SDL_Surface *initSdl(int largeur, int hauteur, char *titre, char *icon);
    void initTtf(void);

    SDL_Surface *my_loadSdlSurface(char *chemin, Uint32 flags);
    SDL_Surface* my_createSdlSurface(int largeur, int hauteur, Uint8 r, Uint8 g, Uint8 b);
    TTF_Font* my_loadTtfFont(char *chemin, int taille);

    Uint32 getPixel(SDL_Surface *surface, int x, int y);
    void putPixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
    SDL_Surface *flipSurface(SDL_Surface *surface, Uint32 flags);

    void my_blitSurface(SDL_Surface *surface, int x, int y);
    void updateEvent(Input *in);
    void gererTemps(Temps *temps, Uint32 delay);

#endif /* MY_SDL_LIB_H_INCLUDED */
