#include "sdl_frontend.h"
#include "sdl_icon.h"
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>

#include "bot.h"
#include "cpu_bot.h"

#define RENDER_EVERY_NTH (8)
#define TILE_SIZE (24)
#define TILE_MOVE (128)
#define SCREEN_WIDTH (1024)
#define SCREEN_HEIGHT (800)

class SDLBotFrontend
{
private:
    static SDLIcon get_icon(SDLFrontend& frontend, Bot& bot)
    {
        CPUBot* cpu = dynamic_cast<CPUBot*>(&bot);
        if (cpu)
        {
            return SDLIcon(
                frontend.m_sdl_renderer,
                cpu->get_program().is_first() ? *frontend.m_tx_bot1 : *frontend.m_tx_bot2,
                TILE_SIZE, TILE_SIZE);;
        }

        return SDLIcon(frontend.m_sdl_renderer, *frontend.m_tx_prey, TILE_SIZE, TILE_SIZE);
    }
public:
    SDLBotFrontend(SDLFrontend& frontend, Bot& bot) :
        m_frontend(frontend), m_bot(bot),
        m_icon(get_icon(frontend, bot))
    {

    }

    SDLIcon& get_icon()
    {
        return m_icon;
    }

private:
    SDLFrontend& m_frontend;
    Bot& m_bot;
    SDLIcon m_icon;
};

SDLFrontend::SDLFrontend(World& world)
    : Frontend(world),
      m_render_cnt(0),
      m_camera_x((SCREEN_WIDTH / 2) - (WORLD_SIZE * TILE_SIZE / 2)),
      m_camera_y((SCREEN_HEIGHT / 2) - (WORLD_SIZE * TILE_SIZE / 2))
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);

    m_sdl_window = SDL_CreateWindow(
        "King Of The Grid",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    m_sdl_renderer = SDL_CreateRenderer(m_sdl_window, -1, SDL_RENDERER_ACCELERATED);

    m_tx_prey = std::make_unique<SDLIconTexture>(m_sdl_renderer, "prey.png");
    m_tx_bot1 = std::make_unique<SDLIconTexture>(m_sdl_renderer, "bot1.png");
    m_tx_bot2 = std::make_unique<SDLIconTexture>(m_sdl_renderer, "bot2.png");

    SDL_SetRenderDrawColor(m_sdl_renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(m_sdl_renderer);
    SDL_RenderPresent(m_sdl_renderer);
}

SDLFrontend::~SDLFrontend()
{
    SDL_DestroyWindow(m_sdl_window);
    SDL_Quit();
}

void SDLFrontend::update_cell(int x, int y, const Cell& cell)
{

}

void* SDLFrontend::on_new_bot(Bot& bot)
{
    return new SDLBotFrontend(*this, bot);
}

void SDLFrontend::on_bot_removed(Bot& bot, void* frontend)
{
    auto* f = reinterpret_cast<SDLBotFrontend*>(frontend);
    delete f;
}

void SDLFrontend::poll_events()
{
    SDL_Event e;

    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT)
        {
            m_world.stop();
        }
        if (e.type == SDL_KEYDOWN)
        {
            switch (e.key.keysym.sym)
            {
                case SDLK_d:
                {
                    m_camera_x -= TILE_MOVE;
                    break;
                }
                case SDLK_a:
                {
                    m_camera_x += TILE_MOVE;
                    break;
                }
                case SDLK_s:
                {
                    m_camera_y -= TILE_MOVE;
                    break;
                }
                case SDLK_w:
                {
                    m_camera_y += TILE_MOVE;
                    break;
                }
            }
        }
    }
}

void SDLFrontend::render_frame()
{
    poll_events();

    m_render_cnt++;

    if (m_render_cnt < RENDER_EVERY_NTH)
        return;

    m_render_cnt = 0;

    SDL_SetRenderDrawColor(m_sdl_renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(m_sdl_renderer);


    {
        SDL_SetRenderDrawColor(m_sdl_renderer, 255, 0, 0, 255);

        SDL_Rect rect;

        rect.x = m_camera_x - 8;
        rect.y = m_camera_y - 8;
        rect.w = WORLD_SIZE * TILE_SIZE + 16;
        rect.h = WORLD_SIZE * TILE_SIZE + 16;

        SDL_RenderDrawRect(m_sdl_renderer, &rect);
    }

    for (int y_c = 0; y_c < WORLD_SIZE; y_c++)
    {
        for (int x_c = 0; x_c < WORLD_SIZE; x_c++)
        {
            SDL_Rect rect;

            rect.x = m_camera_x + x_c * TILE_SIZE;
            rect.y = m_camera_y + y_c * TILE_SIZE;
            rect.w = TILE_SIZE;
            rect.h = TILE_SIZE;

            const Cell& cell = m_world.get_cell(x_c, y_c);

            switch (cell.state)
            {
                case CellState::empty:
                {
                    SDL_SetRenderDrawColor(m_sdl_renderer, 255, 255, 255, 255);
                    SDL_RenderFillRect(m_sdl_renderer, &rect);

                    break;
                }
                case CellState::food:
                default:
                {
                    uint8_t v = (cell.m_food_value * 255 / MAX_FOOD_VALUE);

                    SDL_SetRenderDrawColor(m_sdl_renderer, 255 - v, 255, 255 - v, 128);
                    SDL_RenderFillRect(m_sdl_renderer, &rect);

                    break;
                }
            }
        }
    }

    for (const auto& bot: m_world.get_bots())
    {
        auto* bf = reinterpret_cast<SDLBotFrontend*>(bot->get_private_frontend());

        int r = 255;
        int g = 255;
        int b = 255;

        if (bot->get_energy() < BOT_WARNING_ENERGY)
        {
            float warning = (float)(bot->get_energy()) / BOT_WARNING_ENERGY;
            g = (int)(255 * warning);
            b = g;
        }

        bf->get_icon().draw(
            m_camera_x + (int)(bot->get_live_x() * TILE_SIZE),
            m_camera_y + (int)(bot->get_live_y() * TILE_SIZE), r, g, b);
    }

    SDL_RenderPresent(m_sdl_renderer);

}

void SDLFrontend::step()
{
    render_frame();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}