#include "bot.h"
#include "world.h"
#include "frontend.h"

#include <iostream>

int Bot::LAST_BOT_ID = 1;

Bot::Bot(Frontend& frontend, std::string&& name, World& world, int x, int y, int energy) :
    m_frontend(frontend),
    m_name(name),
    m_id(LAST_BOT_ID++),
    m_world(world), m_energy(energy), m_energy_timer(0), m_x(x), m_y(y), m_source_x(0), m_source_y(0), m_private_frontend(nullptr),
    m_bot_state(BotState::normal),
    m_move_timer(0),
    m_clear_cell(true)
{
    m_name += " " + std::to_string(m_id);
}

Bot::~Bot()
{
    if (m_clear_cell)
    {
        auto& cell = m_world.get_cell(m_x, m_y);
        cell.set_empty();
    }

    std::cout << "Bot removed: " << get_name() << std::endl;
    m_frontend.on_bot_removed(*this, m_private_frontend);
}

bool Bot::is_enemy(Bot* bot)
{
    return true;
}

float Bot::get_live_x() const
{
    switch (m_bot_state)
    {
        case BotState::moving:
        case BotState::splitting:
        {
            float progress = (float)m_move_timer / (float)BOT_MOVE_TIME;
            float move = (float)m_source_x + (float)(m_x - m_source_x) * progress;
            return move;
        }
        default:
        {
            return m_x;
        }
    }
}

float Bot::get_live_y() const
{
    switch (m_bot_state)
    {
        case BotState::moving:
        case BotState::splitting:
        {
            float progress = (float)m_move_timer / (float)BOT_MOVE_TIME;
            float move = (float)m_source_y + (float)(m_y - m_source_y) * progress;
            return move;
        }
        default:
        {
            return m_y;
        }
    }
}


void Bot::simulate()
{
    if (m_bot_state != BotState::hibernating)
    {
        m_energy_timer++;
        if (m_energy_timer >= BOT_TICK_ENERGY_EVERY)
        {
            m_energy--;
            m_energy_timer = 0;
        }
    }

    switch (m_bot_state)
    {
        case BotState::moving:
        case BotState::splitting:
        {
            m_move_timer++;

            if (m_move_timer >= BOT_MOVE_TIME)
            {
                m_bot_state = BotState::normal;
            }

            break;
        }
        case BotState::scanning:
        case BotState::hibernating:
        {
            m_move_timer--;

            if (m_move_timer == 0)
            {
                m_bot_state = BotState::normal;
            }

            break;
        }
        default:
        {
            break;
        }
    }
}

void Bot::move_left()
{
    move(-1, 0);
}

void Bot::move_right()
{
    move(1, 0);
}

void Bot::move_up()
{
    move(0, -1);
}

void Bot::move_down()
{
    move(0, 1);
}

void Bot::move(int x, int y)
{
    if (m_bot_state != BotState::normal)
        return;

    int new_x = m_x + x;
    int new_y = m_y + y;

    if (new_x < 0 || new_y < 0)
        return;

    if (new_x >= WORLD_SIZE || new_y >= WORLD_SIZE)
        return;

    Bot* eating = nullptr;
    auto& new_cell = m_world.get_cell(new_x, new_y);

    if (new_cell.is_bot())
    {
        if (new_cell.m_bot_value->is_enemy(this))
        {
            eating = new_cell.m_bot_value;
        }
        else
        {
            return;
        }
    }

    m_source_x = m_x;
    m_source_y = m_y;

    auto& old_cell = m_world.get_cell(m_x, m_y);

    old_cell.set_empty();

    if (new_cell.is_food())
    {
        int food = new_cell.m_food_value * FOOD_ENERGY_MULTIPLIER;
        std::cout << "Bot " << get_name() << " ate " << food << " of food " << std::endl;
        m_energy += food;

        if (m_energy > BOT_MAX_ENERGY)
        {
            m_energy = BOT_MAX_ENERGY;
        }
    }

    if (eating)
    {
        std::cout << "Bot " << get_name() << " ate bot " << eating->get_name() << std::endl;
        eating->kill();
        eating->m_clear_cell = false;
        m_energy += eating->m_energy;
    }

    m_x = new_x;
    m_y = new_y;
    new_cell.set_bot(this);

    m_energy -= BOT_MOVE_ENERGY;
    m_move_timer = 0;
    m_bot_state = BotState::moving;
}