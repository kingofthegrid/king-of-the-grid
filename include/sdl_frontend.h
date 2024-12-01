#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <memory>

#include "frontend.h"
#include "sdl_icon.h"


class SDLFrontend: public Frontend
{
    friend class SDLBotFrontend;

public:
    explicit SDLFrontend(World& world);
    ~SDLFrontend() override;

    void* on_new_bot(Bot& bot) override;
    void on_bot_removed(Bot& bot, void* frontend) override;

    void update_cell(int x, int y, const Cell& cell) override;
    void step() override;

protected:
    void poll_events();
    void render_frame();

private:
    SDL_Window* m_sdl_window;
    SDL_Renderer* m_sdl_renderer;
    int m_render_cnt;

    std::unique_ptr<SDLIconTexture> m_tx_prey;
    std::unique_ptr<SDLIconTexture> m_tx_bot1;
    std::unique_ptr<SDLIconTexture> m_tx_bot2;

    float m_camera_x;
    float m_camera_y;
};