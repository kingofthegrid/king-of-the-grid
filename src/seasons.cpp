#include "seasons.h"
#include "world.h"
#include "rules.h"
#include "FastNoiseLite.h"

Seasons::Seasons(World& world, int padding) :
    m_world(world), m_padding(padding),
    m_width(WorldRules::world_width - padding * 2),
    m_height(WorldRules::world_height - padding * 2)
{
    m_distribution.resize(m_height, std::vector<float>(m_width));
}

void Seasons::generate(int seed, float pow)
{
    // Initialize the FastNoiseLite generator
    FastNoiseLite noise = {};

    noise.SetSeed(seed);
    noise.SetFrequency(1.0f);
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    m_flattened_distribution.clear();

    for (int y = 0; y < m_height; ++y)
    {
        for (int x = 0; x < m_width; ++x)
        {
            float bx = x / static_cast<float>(m_width);
            float by = y / static_cast<float>(m_height);

            // Generate noise value, adjust to [0, 1]
            float noise_value = noise.GetNoise(bx, by);
            noise_value = (noise_value + 1.0f) / 2.0f;
            // invert
            noise_value = 1.0f - noise_value;
            noise_value += 0.5f;
            noise_value = std::pow(noise_value, pow);
            int n = noise_value * 100.0f;
            m_distribution[y][x] = noise_value;
            m_flattened_distribution.push_back(n);
        }
    }

}

// Generate a random float in the range [min, max]
float Seasons::get_random_float(float min, float max)
{
    std::uniform_real_distribution<float> dist(min, max);
    return dist(m_random_engine);
}

void Seasons::get_random_location(int& x, int &y)
{
    std::discrete_distribution<int> distribution(m_flattened_distribution.begin(), m_flattened_distribution.end());
    int index = distribution(m_random_engine);
    x = (index % m_width) + m_padding;
    y = (index / m_height) + m_padding;
}