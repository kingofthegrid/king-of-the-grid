#include "bot.h"
#include "world.h"
#include "frontend.h"
#include "recording.h"
#include "rules.h"

#include <iostream>
#include <sstream>

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
    if (!m_world.is_running())
        return;

    if (m_clear_cell)
    {
        auto& cell = m_world.get_cell(m_x, m_y);
        cell.set_empty();
        if (m_world.get_recording())
        {
            m_world.get_recording()->cell_removed(m_x, m_y);
        }
    }

    std::cout << "Bot removed: " << get_name() << std::endl;
    m_frontend.on_bot_removed(*this, m_private_frontend);
}

bool Bot::is_enemy(Bot* bot)
{
    return true;
}

void Bot::simulate()
{
    if (m_bot_state != BotState::hibernating)
    {
        m_energy_timer++;
        if (m_energy_timer >= WorldRules::bot_energy_lost_every_nth_tick)
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

            if (m_move_timer >= WorldRules::time_to_move)
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

    if (new_x >= WorldRules::world_width || new_y >= WorldRules::world_height)
        return;

    Bot* eating = nullptr;
    auto& new_cell = m_world.get_cell(new_x, new_y);

    if (new_cell.is_bot())
    {
        if (is_prey())
        {
            // prey can't eat other bots
            return;
        }

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

    if (m_world.get_recording())
    {
        m_world.get_recording()->cell_removed(m_x, m_y);
    }

    if (new_cell.is_food())
    {
        int food = new_cell.m_food_value * WorldRules::food_energy_multiplier;
        std::cout << "Bot " << get_name() << " ate " << food << " of food " << std::endl;

        if (!is_prey())
        {
            if (m_world.get_recording())
            {
                std::stringstream ss;
                ss << "Bot " << get_name() << " ate " << food << " of food";
                m_world.get_recording()->event(ss.str());
            }
        }

        m_energy += food;

        if (m_energy > WorldRules::bot_energy_max)
        {
            m_energy = WorldRules::bot_energy_max;
        }
    }

    if (eating)
    {
        std::cout << "Bot " << get_name() << " ate bot " << eating->get_name() << std::endl;

        if (!is_prey())
        {
            if (m_world.get_recording())
            {
                std::stringstream ss;
                ss << "Bot " << get_name() << " ate bot " << eating->get_name();
                m_world.get_recording()->event(ss.str());
            }
        }

        eating->kill();
        eating->m_clear_cell = false;
        m_energy += eating->m_energy;
    }

    m_x = new_x;
    m_y = new_y;
    new_cell.set_bot(this);

    if (m_world.get_recording())
    {
        m_world.get_recording()->new_cell(new_x, new_y, get_bot_type(), (float)m_energy / (float)WorldRules::bot_energy_max);
    }

    m_energy -= WorldRules::energy_to_move;
    m_move_timer = 0;
    m_bot_state = BotState::moving;
}