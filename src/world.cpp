#include "world.h"
#include "prey_bot.h"
#include "frontend.h"
#include "recording.h"
#include "rules.h"

#include <iostream>
#include <memory>
#include <utility>
#include <vector>

World::World(int seed) :
    m_cells({}),
    m_running(true),
    m_cycle(0),
    m_seed(seed),
    m_random_engine(seed),
    m_seasons(*this, 0),
    m_walls(*this, 0)
{
    m_cells.resize(WorldRules::world_width * WorldRules::world_height);

    m_seasons.generate(seed, 20.f);

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
                add_bot(frontend, std::make_shared<PreyBot>(frontend, *this, x, y, WorldRules::prey_spawn_energy_with));
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

                    if (cell.m_food_value % (WorldRules::food_spawn_energy_with / 4) == 0)
                    {
                        m_recording->new_cell(x_c, y_c, Recording::CELL_FOOD,
                            (float)cell.m_food_value / (float)WorldRules::food_spawn_energy_with);
                    }

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

    m_bots.remove_if([&](const std::shared_ptr<Bot>& bot) -> bool {
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

void World::add_bot(Frontend& frontend, const std::shared_ptr<Bot>& bot)
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

    int walls_seed = m_seed + 1;

    while (true)
    {
        m_walls.generate(walls_seed, 8.f);

        std::vector<bool> wall_map = {};
        wall_map.resize(WorldRules::world_height * WorldRules::world_width);

        for (int i = 0; i < WorldRules::walls_count; i++)
        {
            int x, y;
            m_walls.get_random_location(x, y);
            wall_map[x + y * WorldRules::world_width] = true;
        }

        if (path_exists(0, 0, WorldRules::world_width - 1, WorldRules::world_height - 1,
            WorldRules::world_height, WorldRules::world_width, [&wall_map](int x, int y) -> bool {
                return wall_map[x + y * WorldRules::world_width];
            }
        )) {

            for (int y = 0; y < WorldRules::world_height; y++)
            {
                for (int x = 0; x < WorldRules::world_width; x++)
                {
                    if (wall_map[x + y * WorldRules::world_width])
                    {
                        auto &cell = get_cell(x, y);
                        if (cell.is_empty())
                        {
                            cell.set_wall();

                            if (m_recording)
                            {
                                m_recording->new_cell(x, y, Recording::CELL_WALL, 1.0f);
                            }
                        }
                    }
                }
            }

            break;
        }

        walls_seed++;
    }

    std::cout << "Walls Seed: " << walls_seed << std::endl;
}

bool World::has_path(std::vector<std::vector<bool>> &visited,
     int x, int y, int target_x, int target_y, int rows, int cols,
     const std::function<bool(int x, int y)>& is_obstacle)
{
    if (x < 0 || y < 0 || x >= rows || y >= cols || visited[x][y] || is_obstacle(x, y))
    {
        return false;
    }

    if (x == target_x && y == target_y)
    {
        return true;
    }

    visited[x][y] = true;

    // Explore all four directions (up, down, left, right)
    if (has_path(visited, x + 1, y, target_x, target_y, rows, cols, is_obstacle) ||
        has_path(visited, x - 1, y, target_x, target_y, rows, cols, is_obstacle) ||
        has_path(visited, x, y + 1, target_x, target_y, rows, cols, is_obstacle) ||
        has_path(visited, x, y - 1, target_x, target_y, rows, cols, is_obstacle))
    {
        return true;
    }

    return false;
}

bool World::path_exists(int start_x, int start_y, int end_x, int end_y, int rows,
                        int cols, const std::function<bool(int x, int y)>& is_obstacle)
{
    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
    return has_path(visited, start_x, start_y, end_x, end_y, rows, cols, is_obstacle);
}
