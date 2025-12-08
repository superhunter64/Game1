#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <Windows.h>
#include <dxgi.h>

#include "App.h"
#include "shared/Resources.h"
#include "shared/Paths.h"
#include "systems/Animation.h"
#include "components/Character.h"
#include "world/Map.h"
#include "UI/ui.h"
#include "wireframe/Wireframe.h"

#include "systems/EntityManager.h"
#include <cassert>

#include "backend/DX12_Pipeline.h"
#include "backend/DeviceResources.h"

//#include <Core/GpuResource.h>
//#include <Core/Gpu.h>

Character player = {};

EntityManager entities = {};
Label testLabel;

Uint64 NOW = SDL_GetPerformanceCounter();
Uint64 LAST = 0;
double deltaTime = 0;

const char* D3D11 = "direct3d11";
const char* D3D12 = "direct3d12";

//static DX12_Pipeline Pipeline = {};
static DX::DeviceResources dr;

static bool streq(const char* str1, const char* str2)
{
    return strcmp(str1, str2) == 0;
}

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    SDL_SetAppMetadata("Game 1", "0.1", "com.superhunter64.game1");

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    App::Window = SDL_CreateWindow("Game 1", App::Width, App::Height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (not App::Window)
    {
        SDL_Log("Couldn't create window: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    auto props = SDL_GetWindowProperties(App::Window);
    App::hwnd = (HWND)SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);

    NOW = SDL_GetPerformanceCounter();
    LAST = 0;
    deltaTime = 0;

    //GPU::DisplayAdapters();

    dr.DisplayAdapters();
    dr.SetWindow();
    dr.CreateDeviceResources(2);
    dr.CreateWindowDependentResources();
    dr.LoadAssets();

    float aspectRatio = 1920.f / 1080.f;
    std::vector<Vertex> triVerts =
    {
        // pos                  color
        {{0.0f, 0.25f * aspectRatio, 0.0f}, {.5f, 0.0f}},
        {{0.25f, -0.25f * aspectRatio, 0.0f}, {1.f, 1.f}},
        {{-0.25f, -0.25f * aspectRatio, 0.0f}, {0.0f, 1.f}}
    };

    dr.LoadVertexBuffer(&triVerts);
    dr.PopulateCommandList();

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
#ifdef _DEBUG
            case SDLK_BACKSLASH: {
                App::DebugEnabled = !App::DebugEnabled;
                SDL_Log("Debug toggled");
            } break;
#endif
            case SDLK_D:
            {
                testLabel.transform.location.x = 10;
            } break;

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
    if (event->type == SDL_EVENT_WINDOW_RESIZED)
    {
    }
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void* appstate)
{
    App::Update();

    dr.Render();

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    /* SDL will clean up the window/renderer for us. */
    //App::CleanUp();
    //TTF_Quit();
}
