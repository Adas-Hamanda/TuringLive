#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <SDL2/SDL.h>
// #include <SDL_ttf.h>
#include <SDL2/SDL_main.h>
#include <SDL2/SDL_thread.h>
#include <stdbool.h>

typedef struct
{
    uint8_t *data;
    uint8_t *tail;
    uint8_t *point;
    int size;
} Program;
int CreateProgram(Program *pgm, int size)
{

    pgm->data = malloc(size);
    pgm->tail = pgm->data + size;
    pgm->point = pgm->data;
    pgm->size = size;

    return 0;
}
int DestroyProgram(Program *pgm)
{
    free(pgm->data);
    return 0;
}
#define SET 1
int set(Program *pgm)
{
    int instruct_size = 2;
    if ((pgm->point + instruct_size) > pgm->tail)
        return 0;
    uint8_t target = *(pgm->point + 1);
    uint8_t number = *(pgm->point + 2);
    target %= pgm->size;
    *(pgm->data + target) = number;
    return 0;
}
#define COPY 2
int copy(Program *pgm)
{
    int instruct_size = 2;
    if ((pgm->point + instruct_size) > pgm->tail)
        return 0;
    uint8_t source = *(pgm->point + 1);
    uint8_t target = *(pgm->point + 2);
    source %= pgm->size;
    target %= pgm->size;
    *(pgm->data + target) = *(pgm->data + source);
    return 0;
}
#define JUMP 3
int jump(Program *pgm)
{
    int instruct_size = 2;
    if ((pgm->point + instruct_size) > pgm->tail)
        return 0;
    uint8_t location = *(pgm->point + 1);
    uint8_t *count = pgm->point + 2;
    if (*count == 0)
    {
        return 0;
    }
    else
    {
        (*count)--;
    }
    location %= pgm->size;
    if (pgm->data + location != pgm->point)
        pgm->point = pgm->data + location;
    return 0;
}
#define INST_BOUND 4
int Step(Program *pgm)
{
    switch ((*(pgm->point)) % INST_BOUND)
    {
    case SET:
        printf("set\n");
        set(pgm);
        break;
    case COPY:
        printf("copy\n");
        copy(pgm);
        break;
    case JUMP:
        printf("jump\n");
        jump(pgm);
        break;
    default:
        printf("null\n");
        break;
    }
    pgm->point++;
    if (pgm->point == pgm->tail)
        pgm->point = pgm->data;
    return 0;
}
bool bwork_thread_run = true;
int evTyStep;
int WorkThread(void *data)
{
    srand(time(NULL));
    Program pgm;
    CreateProgram(&pgm, 256);
    for (uint8_t *i = pgm.data; i < pgm.tail; i++)
    {
        *i = rand();
    }
    evTyStep = SDL_RegisterEvents(1);
    if (evTyStep == -1)
    {
        SDL_LogDebug(SDL_LOG_PRIORITY_ERROR, "SDL Error at:%s\n", SDL_GetError());
    }
    SDL_Event evStep;
    evStep.type = evTyStep;
    evStep.user.data1 = (void *)&pgm;
    while (bwork_thread_run)
    {

        Step(&pgm);
        if (!SDL_PushEvent(&evStep))
        {
            SDL_LogDebug(SDL_LOG_PRIORITY_ERROR, "SDL Error at:%s\n", SDL_GetError());
        }
    }

    DestroyProgram(&pgm);
}
int main(int argc, char **argv)
{
    // printf("SDL Version:%d\n", SDL_VERSION);
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_LogDebug(SDL_LOG_PRIORITY_ERROR, "SDL Error at:%s\n", SDL_GetError());
    }
    SDL_Window *window = SDL_CreateWindow("TuringLive", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        SDL_LogDebug(SDL_LOG_PRIORITY_ERROR, "SDL Error at:%s\n", SDL_GetError());
    }

    SDL_Thread *work_thread = SDL_CreateThread(WorkThread, "Work", NULL);
    if (work_thread == NULL)
    {
        SDL_LogDebug(SDL_LOG_PRIORITY_ERROR, "SDL Error at:%s\n", SDL_GetError());
    }
    SDL_Event event;
    while (SDL_WaitEvent(&event))
    {
        // printf("event!\n");
        switch (event.type)
        {

        case SDL_QUIT:
            // printf("quit!\n");
            bwork_thread_run = false;
            SDL_DestroyWindow(window);
            SDL_Quit();
            break;
        default:
            if (event.type == evTyStep)
            {
                        }

            break;
        }
    }
    SDL_WaitThread(work_thread, NULL);
    return 0;
}