#include "world.h"
#include "prey_bot.h"
#include "frontend.h"

#include <iostream>

World::World(int seed) :
    m_cells({}),
    m_running(true),
    m_cycle(0),
    m_seed(seed),
    m_random_engine(m_seed)
{
    std::cout << "World Seed: " << seed << std::endl;
}

World::~World()
{
    m_bots.clear();
}

void World::simulate(Frontend& frontend)
{
    m_cycle++;

    if ((m_cycle % FOOD_SPAWN_EVERY) == 0)
    {
        int x = get_random(0, WORLD_SIZE);
        int y = get_random(0, WORLD_SIZE);

        auto &cell = get_cell(x, y);
        if (cell.is_empty())
        {
            cell.set_food(get_random(0, FOOD_SPAWN_ENERGY));
        }
    }

    if (PreyBot::PREY_COUNT < MAX_PREY_POPULATION)
    {
        if ((m_cycle % PREY_SPAWN_EVERY) == 0)
        {
            int x = get_random(0, WORLD_SIZE);
            int y = get_random(0, WORLD_SIZE);

            auto &cell = get_cell(x, y);
            if (cell.is_empty())
            {
                add_bot(frontend, new PreyBot(frontend, *this, x, y, PREY_SPAWN_ENERGY));
            }
        }
    }

    if ((m_cycle % FOOD_DECAY) == 0)
    {
        for (int y_c = 0; y_c < WORLD_SIZE; y_c++)
        {
            for (int x_c = 0; x_c < WORLD_SIZE; x_c++)
            {
                auto& cell= get_cell(x_c, y_c);

                if (cell.state == CellState::food)
                {
                    cell.m_food_value--;
                    if (cell.m_food_value == 0)
                    {
                        cell.set_empty();
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
}

Cell& World::get_cell(int x, int y)
{
    return m_cells[x + y * WORLD_SIZE];
}

void World::set_cell(int x, int y, Cell&& cell)
{
    m_cells[x + y * WORLD_SIZE] = cell;
}

const Cell& World::get_cell(int x, int y) const
{
    return m_cells[x + y * WORLD_SIZE];
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

    std::cout << "New bot added: " << bot->get_name() << std::endl;
}
