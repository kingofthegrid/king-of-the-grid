#include "world.h"
#include "prey_bot.h"
#include "frontend.h"
#include "recording.h"
#include "rules.h"

#include <iostream>
#include <memory>

World::World(int seed) :
    m_cells({}),
    m_running(true),
    m_cycle(0),
    m_seed(seed),
    m_random_engine(seed),
    m_seasons(*this, seed)
{
    m_cells.resize(WorldRules::world_width * WorldRules::world_height);

    std::cout << "World Seed: " << seed << std::endl;
}

World::~World()
{
    m_bots.clear();
}

void World::simulate(Frontend& frontend)
{
    m_cycle++;

    if ((m_cycle % WorldRules::food_spawn_every_nth_tick) == 0)
    {
        int x, y;
        m_seasons.get_random_location(x, y);

        auto &cell = get_cell(x, y);
        if (cell.is_empty())
        {
            cell.set_food(WorldRules::food_spawn_energy_with);

            if (m_recording)
            {
                m_recording->new_cell(x, y, Recording::CELL_FOOD, 1.0f);
            }
        }
    }

    if (PreyBot::PREY_COUNT < WorldRules::prey_max_population)
    {
        if ((m_cycle % WorldRules::prey_spawn_every_nth_tick) == 0)
        {
            int x = get_random(0, WorldRules::world_width);
            int y = get_random(0, WorldRules::world_height);

            auto &cell = get_cell(x, y);
            if (cell.is_empty())
            {
                add_bot(frontend, new PreyBot(frontend, *this, x, y, WorldRules::prey_spawn_energy_with));
            }
        }
    }

    if ((m_cycle % WorldRules::food_decay_rate) == 0)
    {
        for (int y_c = 0; y_c < WorldRules::world_height; y_c++)
        {
            for (int x_c = 0; x_c < WorldRules::world_width; x_c++)
            {
                auto& cell= get_cell(x_c, y_c);

                if (cell.state == CellState::food)
                {
                    cell.m_food_value--;
                    if (cell.m_food_value == 0)
                    {
                        cell.set_empty();
                        m_recording->cell_removed(x_c, y_c);
                    }
                }
            }
        }
    }

    for (auto& bot: m_bots)
    {
        bot->simulate();
    }

    m_bots.remove_if([&](const std::unique_ptr<Bot>& bot) -> bool {
        return !bot->is_alive();
    });

    if (m_recording)
    {
        m_recording->iteration();
    }
}

Cell& World::get_cell(int x, int y)
{
    return m_cells[x + y * WorldRules::world_height];
}

void World::set_cell(int x, int y, Cell&& cell)
{
    m_cells[x + y * WorldRules::world_height] = cell;
}

const Cell& World::get_cell(int x, int y) const
{
    return m_cells[x + y * WorldRules::world_height];
}

int World::get_random(int min, int max) {
    std::uniform_int_distribution<int> distribution(min, max - 1);
    return distribution(m_random_engine);
}

void World::add_bot(Frontend& frontend, Bot* bot)
{
    m_bots.emplace_back(bot);
    bot->m_private_frontend = frontend.on_new_bot(*bot);
    auto& cell = get_cell(bot->get_x(), bot->get_y());
    cell.set_bot(bot);

    if (m_recording)
    {
        m_recording->new_cell(bot->get_x(), bot->get_y(), bot->get_bot_type(), (float)bot->get_energy() / WorldRules::bot_energy_max);
    }

    std::cout << "New bot added: " << bot->get_name() << std::endl;
}

void World::enable_recording(const std::string& name, const std::string& title)
{
    m_recording = std::make_unique<Recording>(*this, name, title);
}

void World::start()
{
    if (m_recording)
    {
        m_recording->start();
    }
}