#pragma once

#include "frontend.h"

class ServerFrontend: public Frontend
{
public:
    explicit ServerFrontend(World& world);
    virtual ~ServerFrontend() = default;

    void* on_new_bot(Bot& bot) override;
    void on_bot_removed(Bot& bot, void* frontend) override;

    void update_cell(int x, int y, const Cell& cell) override;
    void step() override;

};