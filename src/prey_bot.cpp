#include "prey_bot.h"
#include "world.h"

int PreyBot::PREY_COUNT = 0;

PreyBot::PreyBot(Frontend& frontend, World& world, int x, int y, int energy)
    : Bot(frontend, "Prey", world, x, y, energy), m_cooldown(0)
{
    PREY_COUNT++;
}

PreyBot::~PreyBot()
{
    PREY_COUNT--;
}

void PreyBot::simulate()
{
    Bot::simulate();

    if (get_state() == BotState::normal)
    {
        if (m_cooldown == 0)
        {
            switch (m_world.get_random(0, 4))
            {
                case 0:
                {
                    move_up();
                    break;
                }
                case 1:
                {
                    move_down();
                    break;
                }
                case 2:
                {
                    move_left();
                    break;
                }
                case 3:
                {
                    move_right();
                    break;
                }
            }

            m_cooldown = m_world.get_random(100, 1000);
        }
        else
        {
            m_cooldown--;
        }
    }
}