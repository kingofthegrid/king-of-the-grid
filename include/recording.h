#pragma once

#include <string>
#include <fstream>
#include <array>
#include <chrono>

#include "defines.h"

#define RECORDING_SIZE_X_MP (3)
#define RECORDING_SIZE_Y_MP (1)
#define RECORDING_SIZE_X (WORLD_SIZE * RECORDING_SIZE_X_MP)
#define RECORDING_SIZE_Y (WORLD_SIZE * RECORDING_SIZE_Y_MP)

#define RECORDING_EVENTS (3)
#define RECORDING_STDOUT (8)

#define RECORDING_OFFSET_Y (6)
#define RECORDING_OFFSET_X (2)

class World;

struct RecordingSTDOut
{
    std::string name;
    std::array<std::string, RECORDING_STDOUT> log;
};

class Recording
{
public:
    static constexpr int CELL_BOT_A = 1;
    static constexpr int CELL_BOT_B = 2;
    static constexpr int CELL_FOOD = 3;
    static constexpr int CELL_PREY = 4;

public:
    Recording(World& world, const std::string& name, const std::string& title);
    void start();
    ~Recording();

    void new_cell(int x, int y, int index);
    void cell_removed(int x, int y);
    void event(const std::string& event);
    void log(int x, int y, const std::string& v, int color);
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