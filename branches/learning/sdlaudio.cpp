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
SDL_Surface* g_pDisplaySurface = NULL;
SDL_Event g_Event;
SDL_AudioSpec* g_SpecDesired;
SDL_AudioSpec* g_SpecObtained;
void FOSDLAudioCallback(void* userdata,Uint8* buffer,int len);
int FOSDL_ThreadFunction(void* data);

SDL_sem* g_pSemaphore;
SDL_Thread* g_pThread[3];

int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
    atexit(SDL_Quit);
    g_pDisplaySurface =
        SDL_SetVideoMode(SCREEN_WIDTH,SCREEN_HEIGHT,0,SDL_ANYFORMAT);
    g_SpecDesired=new SDL_AudioSpec;
    g_SpecObtained=new SDL_AudioSpec;
    g_SpecDesired->freq=22050;
    g_SpecDesired->format=AUDIO_S16LSB;
    g_SpecDesired->channels=1;
    g_SpecDesired->samples=8192;
    g_SpecDesired->callback=FOSDLAudioCallback;
    g_SpecDesired->userdata=NULL;
    SDL_OpenAudio(g_SpecDesired,g_SpecObtained);
    delete g_SpecDesired;
    SDL_PauseAudio(0);

    //create semaphore
    g_pSemaphore=SDL_CreateSemaphore(0);
    //create three threads
    g_pThread[0]=SDL_CreateThread(FOSDL_ThreadFunction,(void*)1);
    g_pThread[1]=SDL_CreateThread(FOSDL_ThreadFunction,(void*)2);
    g_pThread[2]=SDL_CreateThread(FOSDL_ThreadFunction,(void*)3);
    //wait for a second
    SDL_Delay(1000);
    //post to the semaphore
    SDL_SemPost(g_pSemaphore);

    for (;;)
    {
        if (SDL_PollEvent(&g_Event)==0)
        {
            SDL_UpdateRect(g_pDisplaySurface,0,0,0,0);
        }
        else
        {
            if (g_Event.type==SDL_QUIT) break;
        }
    }
    SDL_CloseAudio();
    delete g_SpecObtained;
    return(0);
}

void FOSDLAudioCallback(void* userdata,Uint8* buffer,int len)
{
    int index;
    for (index=0;index<len;index++)
    {
        buffer[index]=rand()%256;
    }
}

//thread function
int FOSDL_ThreadFunction(void* data)
{
    //grab thread number
//    int threadnumber = (int)data;
    int threadnumber = 3;

    //wait for semaphore
    fprintf(stdout,"Thread %d: Initialized.\n",threadnumber);
    fprintf(stdout,"Thread %d: Waiting for semaphore.\n",threadnumber);
    SDL_SemWait(g_pSemaphore);
    //post to semaphore
    fprintf(stdout,"Thread %d: Done waiting for semaphore.\n",threadnumber);
    fprintf(stdout,"Thread %d: Posting semaphore.\n",threadnumber);
    SDL_SemPost(g_pSemaphore);
    //wait for semaphore again before terminating
    fprintf(stdout,"Thread %d: Waiting for semaphore before terminating.\n",threadnumber);
    SDL_SemWait(g_pSemaphore);
    //terminate
    fprintf(stdout,"Thread %d: Terminating.\n",threadnumber);
    SDL_SemPost(g_pSemaphore);
    //return 0
    return(0);
}
