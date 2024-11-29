#pragma once

#include "bot.h"

class PreyBot: public Bot
{
public:
    static int PREY_COUNT;

public:
    PreyBot(Frontend& frontend, World& world, int x, int y, int energy);
    ~PreyBot() override;

    void simulate() override;
private:
    int m_cooldown;
};