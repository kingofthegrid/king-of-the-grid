#pragma once

#include "world.h"
#include "bot.h"

class Frontend
{
public:
    explicit Frontend(World& world);
    virtual ~Frontend() = default;

    virtual void* on_new_bot(Bot& bot) { return nullptr; }
    virtual void on_bot_removed(Bot& bot, void* frontend) {}

    virtual void update_cell(int x, int y, const Cell& cell) = 0;
    virtual void step() = 0;

protected:
    World& m_world;
};