#ifdef __cplusplus
#include <cstdlib>
#else
#include <stdlib.h>
#endif
#ifdef __APPLE__
#include <SDL/SDL.h>
#include <SDL/SDL_net.h>
#else
#include <SDL.h>
#include <SDL_net.h>
#endif

#include <stdlib.h>

int main(int argc,char* argv[])
{
    if (SDL_Init(SDL_INIT_EVERYTHING)==-1)
    {
        return(0);
    }
    atexit(SDL_Quit);
    if (SDLNet_Init())
    {
        return(0);
    }
    atexit(SDLNet_Quit);
    IPaddress ip;
    SDLNet_ResolveHost(&ip,NULL,16);
    fprintf(stdout,"Local Host: %s\n",SDLNet_ResolveIP(&ip));
    SDLNet_ResolveHost(&ip,"www.gamedev.net",16);
    fprintf(stdout,"Remote Host: %s\n",SDLNet_ResolveIP(&ip));
    return(0);
}
