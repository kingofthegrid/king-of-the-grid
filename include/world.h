#pragma once

#include <array>
#include <random>
#include <list>
#include <memory>

#include "bot.h"
#include "defines.h"

class Recording;

enum class CellState
{
    empty,
    food,
    bot
};

struct Cell
{
    CellState state;
    int m_food_value;
    Bot* m_bot_value;

    bool is_empty() const { return state == CellState::empty; }
    bool is_bot() const { return state == CellState::bot; }
    bool is_food() const { return state == CellState::food; }

    void set_empty() {
        state = CellState::empty; m_food_value = 0; m_bot_value = nullptr;
    }
    void set_food(int value)
    {
        state = CellState::food;
        m_food_value = value;
    }

    void set_bot(Bot* bot)
    {
        state = CellState::bot;
        m_bot_value = bot;
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

    std::list<std::unique_ptr<Bot>>& get_bots() { return m_bots; }
    const std::list<std::unique_ptr<Bot>>& get_bots() const { return m_bots; }

    void set_cell(int x, int y, Cell&& cell);
    int get_seed() const { return m_seed; }
    int get_random(int min, int max);

    void add_bot(Frontend& frontend, Bot* bot);
    void enable_recording(const std::string& name, const std::string& title);
    void start();

    std::unique_ptr<Recording>& get_recording() { return m_recording; }

private:
    std::array<Cell,  WORLD_SIZE_SQ> m_cells;
    std::list<std::unique_ptr<Bot>> m_bots;

    bool m_running;
    int m_cycle;
    int m_seed;
    std::mt19937 m_random_engine;
    std::unique_ptr<Recording> m_recording;
};