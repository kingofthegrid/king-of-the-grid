#include "recording.h"
#include "world.h"
#include "rules.h"

#include <iostream>
#include <cstring>
#include <sstream>
#include <iomanip>

Recording::Recording(World& world, const std::string& name, const std::string& title) :
    m_name(name),
    m_title(title),
    m_world(world),
    m_stream(name, std::ios_base::out),
    m_events {},
    m_stdouts {}
{
    std::cout << "Recording enabled: " << name << std::endl;

    int console_width = (WorldRules::world_width * RECORDING_SIZE_X_MP) + RECORDING_OFFSET_X + RECORDING_STDOUT_WIDTH + 2;
    int console_height = (WorldRules::world_height * RECORDING_SIZE_Y_MP) + RECORDING_OFFSET_Y;

    m_stream << "{\"version\": 2, \"width\": " << console_width <<
        ", \"height\": " << console_height <<
        ", \"timestamp\": 1504467315, \"title\": \"" << title << "\", \"env\": {\"TERM\": \"xterm-256color\", \"SHELL\": \"/bin/zsh\"}}"
        << std::endl;
    m_stream << std::fixed << std::setprecision(6);

    m_start = std::chrono::high_resolution_clock::now();
    m_stream << "[" << timestamp() << ", \"o\", \"\\u001b[H\\u001b[J\"]" << std::endl;

    {
        {
            std::stringstream top;
            top << "+" << std::string((WorldRules::world_width * RECORDING_SIZE_X_MP), '-') << "+";
            log(RECORDING_OFFSET_X - 1, RECORDING_OFFSET_Y - 1, top.str(), 37);
        }

        for (int i = 0; i < WorldRules::world_height; ++i) {
            std::stringstream row;
            row << "|" << std::string(WorldRules::world_width * RECORDING_SIZE_X_MP, ' ') << "|";
            log(RECORDING_OFFSET_X - 1, RECORDING_OFFSET_Y + i, row.str(), 37);
        }

        {
            std::stringstream bottom;
            bottom << "+" << std::string(WorldRules::world_width * RECORDING_SIZE_X_MP, '-') << "+";
            log(RECORDING_OFFSET_X - 1, RECORDING_OFFSET_Y + WorldRules::world_height, bottom.str(), 37);
        }
    }
}

void Recording::start()
{
    log(RECORDING_OFFSET_X + WorldRules::world_width * RECORDING_SIZE_X_MP + 2,
        RECORDING_OFFSET_Y - 1, get_stdout(0).name + ":", 37);
    log(RECORDING_OFFSET_X + WorldRules::world_width * RECORDING_SIZE_X_MP + 2,
        RECORDING_OFFSET_Y + (WorldRules::world_height / 2) - 1, get_stdout(1).name + ":", 37);
}

Recording::~Recording()
{
    m_stream << "[" << timestamp() << ", \"o\", \"" <<
    "\\u001b[" << WorldRules::world_height + RECORDING_OFFSET_Y + 2 << ";" << 0 << "H"
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

    if (o.log[0].length() != RECORDING_STDOUT_WIDTH)
    {
        o.log[0].resize(RECORDING_STDOUT_WIDTH, ' ');
    }

    int offset_y = index == 0 ? 0 : (WorldRules::world_height / 2) * RECORDING_SIZE_Y_MP;

    for (int i = 0; i < RECORDING_STDOUT; i++)
    {
        log(RECORDING_OFFSET_X + WorldRules::world_width * RECORDING_SIZE_X_MP + 2,
            RECORDING_OFFSET_Y + offset_y + i, o.log[i], 36);
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

void Recording::log_ext(int x, int y, const std::string& v, int r, int g, int b)
{
    int color_index = 16 + (36 * std::clamp(r, 0, 5)) + (6 * std::clamp(g, 0, 5)) + std::clamp(b, 0, 5);

    m_stream << "[" << timestamp() << ", \"o\", \"" <<
        "\\u001b[" << y << ";" << x<< "H"
        << "\\u001b[38;5;" << color_index << "m"
        << v
        << "\"]" << std::endl;
}

void Recording::new_cell(int x, int y, int index, float intensity)
{
    const char* v;

    intensity = std::clamp(intensity, 0.f, 1.0f);

    int r;
    int g;
    int b;

    switch (index)
    {
        case CELL_FOOD:
        {
            v = "<%>";
            r = 2;
            g = 2;
            b = 2;
            break;
        }
        case CELL_PREY:
        {
            v = "(~)";
            r = 5;
            g = 5;
            b = 5;
            break;
        }
        case CELL_BOT_A:
        {
            v = "{1}";

            r = 5 - int(intensity * 5);
            g = int(intensity * 5);
            b = 0;

            break;
        }
        case CELL_BOT_B:
        default:
        {
            v = "{2}";

            r = 5 - int(intensity * 5);
            g = int(intensity * 5);
            b = 0;

            break;
        }
    }

    log_ext(x * RECORDING_SIZE_X_MP + RECORDING_OFFSET_X, y * RECORDING_SIZE_Y_MP + RECORDING_OFFSET_Y, v, r, g, b);
}

void Recording::cell_removed(int x, int y)
{
    log(x * RECORDING_SIZE_X_MP + RECORDING_OFFSET_X, y * RECORDING_SIZE_Y_MP + RECORDING_OFFSET_Y, "   ", 30);
}
