#pragma once

#include "SDL.h"
#include "SDL_image.h"

#include <iostream>

struct SDLIconTexture
{
    // Constructor
    SDLIconTexture(SDL_Renderer* renderer, const std::string& path) :
        renderer(renderer), texture(nullptr), width(0), height(0)
    {
        load_from_file(path);
    }

    // Load texture from a file
    bool load_from_file(const std::string& path)
    {
        // Free the existing texture
        if (texture) {
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }

        // Load image
        SDL_Surface* loadedSurface = IMG_Load(path.c_str());
        if (!loadedSurface) {
            std::cerr << "Unable to load image " << path << "! SDL_image Error: " << IMG_GetError() << std::endl;
            return false;
        }

        // Create texture from surface
        texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if (!texture) {
            std::cerr << "Unable to create texture from " << path << "! SDL Error: " << SDL_GetError() << std::endl;
            SDL_FreeSurface(loadedSurface);
            return false;
        }

        // Set width and height
        width = loadedSurface->w;
        height = loadedSurface->h;

        SDL_FreeSurface(loadedSurface);
        return true;
    }

    // Destructor to free the texture
    ~SDLIconTexture() {
        if (texture) {
            SDL_DestroyTexture(texture);
        }
    }


    SDL_Renderer* renderer;
    SDL_Texture* texture;
    int width;
    int height;
};

class SDLIcon
{
public:
    // Constructor
    SDLIcon(SDL_Renderer* renderer, SDLIconTexture& texture, int width, int height) :
        renderer(renderer), texture(texture),
        width(width), height(height)
    {
    }

    void draw(int x, int y, int r, int g, int b) const
    {
        if (texture.texture)
        {
            SDL_Rect renderQuad = {x, y, width, height};
            SDL_SetTextureColorMod(texture.texture, r, g, b);
            SDL_RenderCopy(renderer, texture.texture, nullptr, &renderQuad);
        }
    }

private:
    SDL_Renderer* renderer;
    SDLIconTexture& texture;
    int width;
    int height;
};