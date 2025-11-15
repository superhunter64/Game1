#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "App.h"
#include "shared/Resources.h"
#include "shared/Paths.h"
#include "systems/Animation.h"
#include "components/Character.h"
#include "world/Map.h"

Character player{}; 
World::Map map;

Uint64 NOW = SDL_GetPerformanceCounter();
Uint64 LAST = 0;
double deltaTime = 0;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    SDL_SetAppMetadata("Game 1", "0.1", "com.superhunter64.game1");

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("Game 1", App::Width, App::Height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY, &App::Window, &App::Renderer))
    {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(App::Renderer, App::Width, App::Height, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    SDL_SetRenderScale(App::Renderer, 3, 3);

    if (!TTF_Init())
    {
        SDL_Log("Couldn't initialize SDL_ttf: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    NOW = SDL_GetPerformanceCounter();
    LAST = 0;
    deltaTime = 0;

    Resources::LoadTextures(Path::Textures);
    Resources::LoadSpriteSheets(Path::SpriteSheets);

    player.name = "Player 1"; 
    player.transform.x = 0;
    player.transform.y = 0;

    player.sprite.sheet = Resources::GetSheet("Idle");
    player.sprite.anim = Resources::GetAnim("Idle", "Idle");

    {
        using namespace World;

        MapParameters mp{};
        mp.height = 10;
        mp.width = 10;
        mp.tileSet = {};
        mp.tileSet.gridHeight = 16;
        mp.tileSet.gridWidth = 16;
        mp.tileSet.texture = Resources::Textures["Tiles"];
        mp.tileSet.columns = 25;

        map = Map(mp);
        map.Generate();
    }

    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    if (event->type == SDL_EVENT_QUIT)
    {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    if (event->type == SDL_EVENT_KEY_DOWN)
    {
        switch (event->key.key)
        {
            case SDLK_2:
            {
                Animator::Play(player.sprite, "Jump");
            } break;
            case SDLK_1:
            {
                Animator::Play(player.sprite, "Idle");
            } break;
        }
    }
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void* appstate)
{
    App::Update();

    //animate 
    
    Animator::Update(player.sprite);

    /* clear the window to the draw color. */
    SDL_RenderClear(App::Renderer);

    map.Draw(App::Renderer);

    SDL_FRect dest{ player.transform.x, player.transform.y, player.sprite.frame.src.w, player.sprite.frame.src.h };
    SDL_RenderTexture(App::Renderer, player.sprite.sheet->texture, &player.sprite.frame.src, &dest);
    /* put the newly-cleared rendering on the screen. */
    SDL_RenderPresent(App::Renderer);

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    /* SDL will clean up the window/renderer for us. */
    TTF_Quit();
}
