#pragma once

#include <string>
#include <memory>

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

class Bot: public std::enable_shared_from_this<Bot>
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

    bool move_left();
    bool move_right();
    bool move_up();
    bool move_down();

    virtual bool is_prey() const { return true; }
    virtual bool is_cpu() const { return false; }
    virtual int get_bot_type() const = 0;

protected:
    Frontend& m_frontend;
    World& m_world;
    bool move(int x, int y);
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