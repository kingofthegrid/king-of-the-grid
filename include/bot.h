#pragma once

#include <string>

#define BOT_MAX_ENERGY (65536)
#define BOT_WARNING_ENERGY (10000)
#define BOT_MOVE_TIME (100)
#define BOT_MOVE_ENERGY (500)
#define BOT_TICK_ENERGY_EVERY (10)

class World;
class Frontend;

enum class BotState
{
    normal,
    moving,
    splitting,
    scanning,
    hibernating
};

class Bot
{
public:
    static int LAST_BOT_ID;

public:
    friend class World;

    explicit Bot(Frontend& frontend, std::string&& name, World& world, int x, int y, int energy);
    virtual ~Bot();

    virtual void simulate();
    virtual bool is_enemy(Bot* bot);

    void kill() { m_energy = 0; }
    bool is_alive() const { return m_energy > 0; }
    const std::string& get_name() const { return m_name; }

    void set_private_frontend(void* frontend) { m_private_frontend = frontend; }
    void* get_private_frontend() { return m_private_frontend; }

    void set_state(BotState state) { m_bot_state = state; }
    BotState get_state() const { return m_bot_state; }
    int get_energy() const { return m_energy; }

    int get_x() const { return m_x; }
    int get_y() const { return m_y; }
    int get_id() const { return m_id; }

    float get_live_x() const;
    float get_live_y() const;

    void move_left();
    void move_right();
    void move_up();
    void move_down();

    virtual bool is_prey() const { return true; }
    virtual int get_bot_type() const = 0;

protected:
    Frontend& m_frontend;
    World& m_world;
    void move(int x, int y);
    int m_energy;
    int m_energy_timer;
    int m_move_timer;

    bool m_clear_cell;
    int m_x;
    int m_y;
    int m_source_x;
    int m_source_y;

private:
    std::string m_name;
    int m_id;

    void* m_private_frontend;
    BotState m_bot_state;
};