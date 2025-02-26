#pragma once

#include <array>
#include <random>
#include <list>
#include <memory>
#include <functional>

#include "bot.h"
#include "defines.h"
#include "seasons.h"

class Recording;

enum class CellState
{
    empty,
    food,
    bot,
    wall
};

struct Cell
{
    Cell() : state(CellState::empty), m_food_value(0), m_bot_value() {}

    CellState state;
    int m_food_value;
    std::weak_ptr<Bot> m_bot_value;

    bool is_empty() const { return state == CellState::empty; }
    bool is_bot() const { return state == CellState::bot; }
    bool is_food() const { return state == CellState::food; }
    bool is_wall() const { return state == CellState::wall; }

    void set_empty() {
        state = CellState::empty; m_food_value = 0; m_bot_value.reset();
    }
    void set_food(int value)
    {
        state = CellState::food;
        m_food_value = value;
    }

    void set_bot(const std::weak_ptr<Bot>& bot)
    {
        state = CellState::bot;
        m_bot_value = bot;
    }

    void set_wall()
    {
        state = CellState::wall;
    }
};

class Frontend;

class World
{
public:
    explicit World(int seed);
    ~World();

    void simulate(Frontend& frontend);

    bool is_running() const { return m_running; }
    void stop() { m_running = false; }
    int get_cycle() const { return m_cycle; };

    Cell& get_cell(int x, int y);
    const Cell& get_cell(int x, int y) const;

    std::list<std::shared_ptr<Bot>>& get_bots() { return m_bots; }
    const std::list<std::shared_ptr<Bot>>& get_bots() const { return m_bots; }

    void set_cell(int x, int y, Cell&& cell);
    int get_seed() const { return m_seed; }
    int get_random(int min, int max);

    void add_bot(Frontend& frontend, const std::shared_ptr<Bot>& bot);
    void enable_recording(const std::string& name, const std::string& title);
    void start();

    std::unique_ptr<Recording>& get_recording() { return m_recording; }
    std::mt19937& get_random_engine() { return m_random_engine; }
    Seasons& get_seasons() { return m_seasons; }
    Seasons& get_walls() { return m_walls; }

private:
    bool path_exists(int start_x, int start_y, int end_x, int end_y, int rows, int cols, const std::function<bool(int x, int y)>& is_obstacle);
    bool has_path(std::vector<std::vector<bool>> &visited, int x, int y, int target_x, int target_y, int rows, int cols, const std::function<bool(int x, int y)>& is_obstacle);

private:
    std::vector<Cell> m_cells;
    std::list<std::shared_ptr<Bot>> m_bots;
    Seasons m_seasons;
    Seasons m_walls;

    bool m_running;
    int m_cycle;
    int m_seed;
    std::mt19937 m_random_engine;
    std::unique_ptr<Recording> m_recording;
};