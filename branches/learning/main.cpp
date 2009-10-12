#ifdef __cplusplus
    #include <cstdlib>
#else
    #include <stdlib.h>
#endif
#ifdef __APPLE__
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif
const int SCREEN_WIDTH=640;
const int SCREEN_HEIGHT=480;

int main ( int argc,char** argv )
{
    // initialize SDL video
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "Unable to init SDL: %s\n",SDL_GetError() );
        return 1;
    }

    // make sure SDL cleans up before exit
    atexit(SDL_Quit);

    // set the title bar
    SDL_WM_SetCaption("SDL Test", "SDL Test");

    // create a new window
    SDL_Surface* screen = SDL_SetVideoMode(SCREEN_WIDTH,SCREEN_HEIGHT,16,
                                           SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_RESIZABLE);
    if ( !screen )
    {
        printf("Unable to set 640x480 video: %s\n",SDL_GetError());
        return 1;
    }

    //Print out Video info
    const SDL_VideoInfo* g_pVideoInfo = SDL_GetVideoInfo();
    fprintf(stdout,"\nVideo Information:\n");
    fprintf(stdout,"hw_available: %d\n",g_pVideoInfo->hw_available);
    fprintf(stdout,"wm_available: %d\n",g_pVideoInfo->wm_available);
    fprintf(stdout,"blit_hw: %d\n",g_pVideoInfo->blit_hw);
    fprintf(stdout,"blit_hw_CC: %d\n",g_pVideoInfo->blit_hw_CC);
    fprintf(stdout,"blit_hw_A: %d\n",g_pVideoInfo->blit_hw_A);
    fprintf(stdout,"blit_sw: %d\n",g_pVideoInfo->blit_sw);
    fprintf(stdout,"blit_sw_CC: %d\n",g_pVideoInfo->blit_sw_CC);
    fprintf(stdout,"blit_sw_A: %d\n",g_pVideoInfo->blit_sw_A);
    fprintf(stdout,"blit_fill: %d\n",g_pVideoInfo->blit_fill);
    fprintf(stdout,"video memory(in K): %d\n",g_pVideoInfo->video_mem);
    fprintf(stdout,"bits per pixel: %d\n",g_pVideoInfo->vfmt->BitsPerPixel);

    // load an image
    SDL_Surface* bmp = SDL_LoadBMP("cb.bmp");
    if (!bmp)
    {
        printf("Unable to load bitmap: %s\n",SDL_GetError());
        return 1;
    }

    // convert bitmap to display format
    SDL_Surface* bg = SDL_DisplayFormat(bmp);
    // free the temp surface
//    SDL_FreeSurface(bmp);
    SDL_Surface* pSurface = SDL_CreateRGBSurface(SDL_HWSURFACE, 100, 100, 16, 0xF800, 0x07E, 0x1F, 0x00);
    SDL_Surface* conv = SDL_DisplayFormat(pSurface);

    // centre the bitmap on screen
    SDL_Rect dstrect;
    dstrect.x = (screen->w - bmp->w) / 2;
    dstrect.y = (screen->h - bmp->h) / 2;

    SDL_Rect g_Rect;
    Uint8 g_Red, g_Green, g_Blue;
    Uint32 g_Color;

    // program main loop
    bool done = false;
    while (!done)
    {
        // message processing loop
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            g_Red=rand()%256;
            g_Green=rand()%256;
            g_Blue=rand()%256;
            g_Color=SDL_MapRGB(bmp->format,g_Red,g_Green,g_Blue);
            g_Rect.x=rand()%SCREEN_WIDTH;
            g_Rect.y=rand()%SCREEN_HEIGHT;
            g_Rect.w=rand()%(SCREEN_WIDTH-g_Rect.x);
            g_Rect.h=rand()%(SCREEN_HEIGHT-g_Rect.y);
            SDL_FillRect(bmp,&g_Rect,g_Color);
            SDL_UpdateRect(bmp,0,0,0,0);

            char* pData;
            //grab the frame buffer
            pData=(char*)bmp->pixels;

            //vertical offset
            pData+=(g_Rect.y*bmp->pitch);
            //horizontal offset
            pData+=(g_Rect.x*bmp->format->BytesPerPixel);
            //copy color
            memcpy(pData,&g_Color,bmp->format->BytesPerPixel);

            // check for messages
            switch (event.type)
            {
                // exit if the window is closed
            case SDL_QUIT:
                done = true;
                break;

                // check for keypresses
            case SDL_KEYDOWN:
                {
                    // exit if ESCAPE is pressed
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                        done = true;
                    break;
                }
            } // end switch
        } // end of message processing

        // DRAWING STARTS HERE

        // clear screen
        SDL_FillRect(screen,0,SDL_MapRGB(screen->format,0,0,0));

        // draw bitmap
        SDL_BlitSurface(bmp,0,screen,&dstrect);

        // DRAWING ENDS HERE

        // finally,update the screen :)
        SDL_Flip(screen);
    } // end main loop

    // free loaded bitmap
    SDL_FreeSurface(bmp);

    // all is well ;)
    printf("Exited cleanly\n");
    return 0;
}
