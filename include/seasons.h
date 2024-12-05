#pragma once

#include <vector>
#include <random>

class World;

class Seasons
{
public:
    explicit Seasons(World& world, int seed);
    void get_random_location(int& x, int &y);

private:
    float get_random_float(float min, float max);

private:
    std::vector<std::vector<float>> m_distribution;
    std::vector<int> m_flattened_distribution;
    std::mt19937 m_random_engine;
    World& m_world;
};