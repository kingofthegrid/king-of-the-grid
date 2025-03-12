#pragma once

#include <string>
#include <fstream>
#include <array>
#include <chrono>

#include "defines.h"

#define RECORDING_SIZE_X_MP (3)
#define RECORDING_SIZE_Y_MP (1)

#define RECORDING_EVENTS (3)
#define RECORDING_STDOUT (10)

#define RECORDING_OFFSET_Y (6)
#define RECORDING_OFFSET_X (2)
#define RECORDING_STDOUT_WIDTH (32)

#define BOT_1 "▲"
#define BOT_2 "◆"
#define EMPTY_SPACE "   "

class World;

struct RecordingSTDOut
{
    std::string name;
    std::array<std::string, RECORDING_STDOUT> log;
};

class Color
{
public:
    explicit Color(int color_code) : color_code(color_code) {}
    friend std::ostream &operator<<(std::ostream &os, const Color &c);
private:
    int color_code;
};

class Progress
{
public:
    explicit Progress(float intensity) : intensity(intensity) {}
    friend std::ostream &operator<<(std::ostream &os, const Progress &c);
private:
    float intensity;
};

class RichColor
{
public:
    explicit RichColor(int r, int g, int b) : r(r), g(g), b(b) {}
    friend std::ostream &operator<<(std::ostream &os, const RichColor &c);
private:
    int r;
    int g;
    int b;
};

class Position
{
public:
    explicit Position(int x, int y) : x(x), y(y) {}
    friend std::ostream &operator<<(std::ostream &os, const Position &c);
private:
    int x;
    int y;
};

class RecordingLineBegin
{
public:
    explicit RecordingLineBegin(float timestamp) : timestamp(timestamp) {}
    friend std::ostream &operator<<(std::ostream &os, const RecordingLineBegin &c);
private:
    float timestamp;
};

class RecordingLineEnd
{
public:
    friend std::ostream &operator<<(std::ostream &os, const RecordingLineEnd &c);
};

#define RECORD(x) m_stream << RecordingLineBegin(timestamp()) << x << RecordingLineEnd()

class Recording
{
public:
    static constexpr int CELL_BOT_1 = 1;
    static constexpr int CELL_BOT_2 = 2;
    static constexpr int CELL_FOOD = 3;
    static constexpr int CELL_PREY = 4;
    static constexpr int CELL_WALL = 5;

public:
    Recording(World& world, const std::string& name, const std::string& title);
    void start();
    ~Recording();

    void new_cell(int x, int y, int index, float intensity);
    void cell_removed(int x, int y);
    void event(const std::string& event);
    void log(int x, int y, const std::string& v, int color);
    void log_ext(int x, int y, const std::string& v, int r, int g, int b);
    void iteration();
    void add_stdout(int index, const std::string& v);
    RecordingSTDOut& get_stdout(int index) { return m_stdouts[index]; }

    const std::string& get_name() const { return m_name; }

private:
    float timestamp();

private:
    std::string m_name;
    std::string m_title;
    World& m_world;
    std::ofstream m_stream;
    std::array<std::string, RECORDING_EVENTS> m_events;
    std::array<RecordingSTDOut, 2> m_stdouts;
    std::chrono::high_resolution_clock::time_point m_start;
};