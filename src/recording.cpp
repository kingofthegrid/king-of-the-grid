#include "recording.h"
#include "world.h"
#include "rules.h"

#include <iostream>
#include <cstring>
#include <sstream>
#include <iomanip>

std::ostream& operator<< (std::ostream &os, const Color& c)
{
    return os << "\\u001b[" << c.color_code << "m";
}

std::ostream& operator<< (std::ostream &os, const RichColor& c)
{
    int color_index = 16 + (36 * std::clamp(c.r, 0, 5)) + (6 * std::clamp(c.g, 0, 5)) + std::clamp(c.b, 0, 5);
    return os << "\\u001b[38;5;" << color_index << "m";
}

std::ostream& operator<< (std::ostream &os, const Position& c)
{
    return os << "\\u001b[" << c.y << ";" << c.x << "H";
}

std::ostream& operator<< (std::ostream &os, const RecordingLineBegin& c)
{
    return os << "[" << c.timestamp << ", \"o\", \"";
}

std::ostream& operator<< (std::ostream &os, const RecordingLineEnd& c)
{
    return os << "\"]" << std::endl;
}

std::ostream &operator<<(std::ostream &os, const Progress &c)
{
    const char* v;

    if (c.intensity > 0.66)
    {
        v = "⠇";
    }
    else if (c.intensity > 0.33)
    {
        v = "⠆";
    }
    else
    {
        v = "⠄";
    }

    return os << RichColor(3, 3, 3) << v;
}

Recording::Recording(World& world, const std::string& name, const std::string& title) :
    m_name(name),
    m_title(title),
    m_world(world),
    m_stream(name, std::ios_base::out),
    m_events {},
    m_stdouts {}
{
    std::cout << "Recording enabled: " << name << std::endl;

    int console_width = (WorldRules::world_width * RECORDING_SIZE_X_MP) + RECORDING_OFFSET_X + RECORDING_STDOUT_WIDTH + 5;
    int console_height = (WorldRules::world_height * RECORDING_SIZE_Y_MP) + RECORDING_OFFSET_Y;

    m_stream << "{\"version\": 2, \"width\": " << console_width <<
        ", \"height\": " << console_height <<
        ", \"timestamp\": 1504467315, \"title\": \"" << title << "\", \"env\": {\"TERM\": \"xterm-256color\", \"SHELL\": \"/bin/zsh\"}}"
        << std::endl;
    m_stream << std::fixed << std::setprecision(6);

    m_start = std::chrono::high_resolution_clock::now();
    m_stream << "[" << timestamp() << ", \"o\", \"\\u001b[H\\u001b[J\"]" << std::endl;

    {
        RECORD(Position(RECORDING_STDOUT_WIDTH + 1, RECORDING_OFFSET_Y - 1) << RichColor(1, 1, 1) <<
            "+" << std::string((WorldRules::world_width * RECORDING_SIZE_X_MP), '-') << "+");

        std::stringstream ss;

        ss << "|";

        for (int i = 0; i < WorldRules::world_width; ++i) {
            ss << EMPTY_SPACE;
        }

        ss << "|";

        for (int i = 0; i < WorldRules::world_height; ++i) {
            RECORD(Position(RECORDING_STDOUT_WIDTH + 1, RECORDING_OFFSET_Y + i) << RichColor(1, 1, 1) <<
                ss.str());
        }

        RECORD(Position(RECORDING_STDOUT_WIDTH + 1, RECORDING_OFFSET_Y + WorldRules::world_height) << RichColor(1, 1, 1) <<
            "+" << std::string((WorldRules::world_width * RECORDING_SIZE_X_MP), '-') << "+");
    }
}

void Recording::start()
{
    log(0, RECORDING_OFFSET_Y - 1, get_stdout(0).name + ":", 37);
    log(0, RECORDING_OFFSET_Y + (WorldRules::world_height / 2) - 1, get_stdout(1).name + ":", 37);
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
        log(0, RECORDING_OFFSET_Y + offset_y + i, o.log[i], 36);
    }
}

float Recording::timestamp()
{
    return (float)m_world.get_cycle() / 1000.f;
}

void Recording::log(int x, int y, const std::string& v, int color)
{
    RECORD(Position(x, y) << Color(color) << v);
}

void Recording::log_ext(int x, int y, const std::string& v, int r, int g, int b)
{
    RECORD(Position(x, y) << RichColor(r, g, b) << v);
}

void Recording::new_cell(int x, int y, int index, float intensity)
{
    intensity = std::clamp(intensity, 0.f, 1.0f);

    int xx = RECORDING_STDOUT_WIDTH + x * RECORDING_SIZE_X_MP + RECORDING_OFFSET_X;
    int yy = y * RECORDING_SIZE_Y_MP + RECORDING_OFFSET_Y;

    switch (index)
    {
        case CELL_FOOD:
        {
            RECORD(Position(xx, yy) << RichColor(5, 4, 0) << " ✿" << Progress(intensity));
            break;
        }
        case CELL_PREY:
        {
            RECORD(Position(xx, yy) << RichColor(0, 5, 0) << " ⏺" << Progress(intensity));
            break;
        }
        case CELL_BOT_A:
        {
            RECORD(Position(xx, yy) << RichColor(5, 2, 0) << " " << BOT_1 << Progress(intensity));

            break;
        }
        case CELL_BOT_B:
        default:
        {
            RECORD(Position(xx, yy) << RichColor(0, 2, 5) << " " << BOT_2 << Progress(intensity));
            break;
        }
    }
}

void Recording::cell_removed(int x, int y)
{
    RECORD(Position(RECORDING_STDOUT_WIDTH + x * RECORDING_SIZE_X_MP +
        RECORDING_OFFSET_X, y * RECORDING_SIZE_Y_MP + RECORDING_OFFSET_Y) <<
        EMPTY_SPACE);
}
