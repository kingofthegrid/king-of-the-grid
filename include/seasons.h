#pragma once

#include <vector>
#include <random>

class World;

class Seasons
{
public:
    explicit Seasons(World& world, int padding);
    void generate(int seed, float pow);
    void get_random_location(int& x, int &y);

private:
    float get_random_float(float min, float max);

private:
    std::vector<std::vector<float>> m_distribution;
    std::vector<int> m_flattened_distribution;
    std::mt19937 m_random_engine;
    int m_padding;
    int m_width;
    int m_height;
    World& m_world;
};