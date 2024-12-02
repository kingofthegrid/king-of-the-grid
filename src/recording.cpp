#include "recording.h"
#include "world.h"

#include <iostream>
#include <cstring>
#include <sstream>


Recording::Recording(World& world, const std::string& name, const std::string& title) :
    m_title(title),
    m_world(world),
    m_stream(name, std::ios_base::out),
    m_events {},
    m_stdouts {}
{
    std::cout << "Recording enabled: " << name << std::endl;

    m_stream << "{\"version\": 2, \"width\": " << RECORDING_SIZE_X + RECORDING_OFFSET_X <<
        ", \"height\": " << RECORDING_SIZE_Y + RECORDING_OFFSET_Y + RECORDING_STDOUT + 2 <<
        ", \"timestamp\": 1504467315, \"title\": \"" << title << "\", \"env\": {\"TERM\": \"xterm-256color\", \"SHELL\": \"/bin/zsh\"}}"
        << std::endl;
    m_stream << std::fixed << std::setprecision(6);

    m_start = std::chrono::high_resolution_clock::now();
    m_stream << "[" << timestamp() << ", \"o\", \"\\u001b[H\\u001b[J\"]" << std::endl;

    {
        {
            std::stringstream top;
            top << "+" << std::string(RECORDING_SIZE_X, '-') << "+";
            log(RECORDING_OFFSET_X - 1, RECORDING_OFFSET_Y - 1, top.str(), 37);
        }

        for (int i = 0; i < WORLD_SIZE; ++i) {
            std::stringstream row;
            row << "|" << std::string(RECORDING_SIZE_X, ' ') << "|";
            log(RECORDING_OFFSET_X - 1, RECORDING_OFFSET_Y + i, row.str(), 37);
        }

        {
            std::stringstream bottom;
            bottom << "+" << std::string(RECORDING_SIZE_X, '-') << "+";
            log(RECORDING_OFFSET_X - 1, RECORDING_OFFSET_Y + WORLD_SIZE, bottom.str(), 37);
        }
    }
}

void Recording::start()
{
    log(1, RECORDING_OFFSET_Y + WORLD_SIZE + 1, get_stdout(0).name + ":", 37);
    log(1 + WORLD_SIZE * RECORDING_SIZE_X_MP / 2, RECORDING_OFFSET_Y + WORLD_SIZE + 1, get_stdout(1).name + ":", 37);
}

Recording::~Recording()
{
    m_stream << "[" << timestamp() << ", \"o\", \"" <<
    "\\u001b[" << WORLD_SIZE + RECORDING_OFFSET_Y + RECORDING_STDOUT + 2 << ";" << 0 << "H"
     << "\"]" << std::endl;
}

void Recording::iteration()
{
    std::stringstream ss;
    ss << m_title << " | Iteration " << m_world.get_cycle();

    log(2, 1, ss.str(), 96);
}

void Recording::event(const std::string& event)
{
    m_events[2] = m_events[1];
    m_events[1] = m_events[0];
    m_events[0] = event;

    if (m_events[0].length() < 64)
    {
        m_events[0].resize(64, ' ');
    }

    log(2, 2, m_events[0], 92);
    log(2, 3, m_events[1], 92);
    log(2, 4, m_events[2], 92);
}

void Recording::add_stdout(int index, const std::string& v)
{
    auto& o = m_stdouts[index];

    for (int i = RECORDING_STDOUT - 1; i >= 1; i--)
    {
        o.log[i] = o.log[i - 1];
    }

    o.log[0] = v;

    if (o.log[0].length() < (WORLD_SIZE / 2) * RECORDING_SIZE_X_MP)
    {
        o.log[0].resize((WORLD_SIZE / 2) * RECORDING_SIZE_X_MP, ' ');
    }

    int offset_x = index == 0 ? 0 : WORLD_SIZE * RECORDING_SIZE_X_MP / 2;

    for (int i = 0; i < RECORDING_STDOUT; i++)
    {
        log(1 + offset_x, WORLD_SIZE + RECORDING_OFFSET_Y + 2 + i, o.log[i], 36);
    }
}

float Recording::timestamp()
{
    return (float)m_world.get_cycle() / 1000.f;
}

void Recording::log(int x, int y, const std::string& v, int color)
{
    m_stream << "[" << timestamp() << ", \"o\", \"" <<
        "\\u001b[" << y << ";" << x<< "H"
        << "\\u001b[" << color << "m"
        << v
        << "\"]" << std::endl;
}

void Recording::new_cell(int x, int y, int index)
{
    const char* v;
    int color;

    switch (index)
    {
        case CELL_FOOD:
        {
            v = ".#.";
            color = 33;
            break;
        }
        case CELL_PREY:
        {
            v = ".&.";
            color = 32;
            break;
        }
        case CELL_BOT_A:
        {
            v = "[A]";
            color = 91;
            break;
        }
        case CELL_BOT_B:
        default:
        {
            v = "[B]";
            color = 95;
            break;
        }
    }

    log(x * RECORDING_SIZE_X_MP + RECORDING_OFFSET_X, y * RECORDING_SIZE_Y_MP + RECORDING_OFFSET_Y, v, color);
}

void Recording::cell_removed(int x, int y)
{
    log(x * RECORDING_SIZE_X_MP + RECORDING_OFFSET_X, y * RECORDING_SIZE_Y_MP + RECORDING_OFFSET_Y, "   ", 30);
}
