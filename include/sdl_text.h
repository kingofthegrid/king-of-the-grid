#pragma once

#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <stdexcept>

class SDLTextLine
{
public:
    SDLTextLine(SDL_Renderer* renderer, TTF_Font* font, const SDL_Color& color)
        : renderer(renderer), font(font), color(color), texture(nullptr) {}

    ~SDLTextLine() {
        if (texture) {
            SDL_DestroyTexture(texture);
        }
    }

    // Set the text content
    void set_text(const std::string& newText)
    {
        if (newText == text) {
            return; // No change, skip update
        }
        text = newText;

        // Clean up the old texture
        if (texture) {
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }

        // Render the new text into a texture
        SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
        if (!surface) {
            throw std::runtime_error("Failed to render text surface: " + std::string(TTF_GetError()));
        }

        texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) {
            SDL_FreeSurface(surface);
            throw std::runtime_error("Failed to create text texture: " + std::string(SDL_GetError()));
        }

        width = surface->w;
        height = surface->h;

        SDL_FreeSurface(surface);
    }

    // Render the text on the screen
    void render(int x, int y, const std::string& line_text)
    {
        set_text(line_text);

        if (!texture) {
            return; // Nothing to render
        }

        SDL_Rect destRect = {x, y, width, height};
        SDL_RenderCopy(renderer, texture, nullptr, &destRect);
    }

private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    SDL_Color color;
    std::string text;
    SDL_Texture* texture;
    int width, height;
};
