#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL_ttf.h>

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
int main()
{
    printf("SDL Version:%d\n", SDL_VERSION);
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
    }
    SDL_Window *window = SDL_CreateWindow("TuringLive", 800, 600, SDL_WINDOW_RESIZABLE);
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {

        case SDL_EVENT_QUIT:
            SDL_DestroyWindow(window);
            SDL_Quit();
            break;

        default:
            break;
        }
    }

    srand(time(NULL));
    Program pgm;
    CreateProgram(&pgm, 256);
    for (uint8_t *i = pgm.data; i < pgm.tail; i++)
    {
        *i = rand();
    }

    while (1)
    {
        system("clear");
        Step(&pgm);
        int cow = 10;
        for (uint8_t *j = pgm.data; j < pgm.tail; j++)
        {
            // printf("h\n");
            if (j == pgm.point)
            {
                printf("\033[32;40m%3d \033[0m", *j);
            }
            else
            {
                printf("%3d ", *j);
            }
            if ((pgm.tail - j + 1) % cow == 0)
            {
                printf("\n");
            }
        }
        if (getchar() == 'c')
            break;
    }

    DestroyProgram(&pgm);
}