#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif
#include <iostream>
#include <thread>
#include <cstdlib>
#include <map>
#include <filesystem>
#include <fstream>
#include "world.h"
#include "recording.h"
#include "server_frontend.h"
#include "cpu_bot.h"
#include "rules.h"
#include <functional>

using namespace std::chrono;

std::vector<std::string> find_bin_files(const std::string& folder_path)
{
    std::vector<std::string> result {};

    try
    {
        for (const auto& entry : std::filesystem::directory_iterator(folder_path))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".bin")
            {
                result.push_back(entry.path().stem().string());
            }
        }
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return result;
}

int test_programs(int seed, CPUProgram& program1, CPUProgram& program2, bool simple_name)
{
    if (seed > 65535 || seed < 0)
    {
        std::cerr << "Seed range exceeded: " << seed << std::endl;
        exit(-1);
    }

    int result = 0;
    long limit = 0;
    std::unique_ptr<World> world = std::make_unique<World>(seed);

    std::cout << "Engine v" << ENGINE_VERSION << std::endl;

    std::string title = "King-Of-The-Grid | " + program1.get_name() + "(" + BOT_1 + ") vs " +
        program2.get_name() + "(" + BOT_2 + ") seed " + std::to_string(seed);

    if (simple_name)
    {
        world->enable_recording("recording.txt", title);
    }
    else
    {
        world->enable_recording("recording-" + program1.get_name() + "-" +
            program2.get_name() + "-" + std::to_string(seed) + ".txt", title);

    }

    world->get_recording()->get_stdout(0).name = program1.get_name();
    world->get_recording()->get_stdout(1).name = program2.get_name();

    std::unique_ptr<Frontend> frontend = std::make_unique<ServerFrontend>(*world);

    world->add_bot(*frontend, std::make_shared<CPUBot>(
        *frontend, program1, *world,
        0, 0,
        WorldRules::bot_energy_spawn));

    world->add_bot(*frontend, std::make_shared<CPUBot>(
        *frontend, program2, *world,
        WorldRules::world_width - 1,
        WorldRules::world_height - 1,
        WorldRules::bot_energy_spawn));

    world->start();

    std::cout << "Created world with seed " << world->get_seed() << std::endl;

    if (world->get_recording())
    {
        world->get_recording()->event("Playing: " + program1.get_name() + "(" + BOT_1 + ") vs " + program2.get_name() + "(" + BOT_2 + ")");
    }

    while (world->is_running())
    {
        world->simulate(*frontend);
        frontend->step();
        if (program1.lost() and program2.lost())
        {
            if (world->get_recording())
            {
                world->get_recording()->event(">>>>>>>>>>>> Game ended with Draw.");
            }
            world->stop();
        }
        else if (program1.lost())
        {
            if (world->get_recording())
            {
                world->get_recording()->event(">>>>>>>>>>>> Program " + program2.get_name() + " " + BOT_2 + " won.");
            }
            result = 2;
            world->stop();
            break;
        }
        else if (program2.lost())
        {
            if (world->get_recording())
            {
                world->get_recording()->event(">>>>>>>>>>>> Program " + program1.get_name() + " " + BOT_1 + " won.");
            }
            result = 1;
            world->stop();
            break;
        }

        limit++;
        if (limit > WorldRules::world_iteration_limit)
        {
            std::cout << "Reached limit of " << WorldRules::world_iteration_limit << " iterations. " << std::endl;
            if (world->get_recording())
            {
                world->get_recording()->event(
                    std::string("Reached limit of ") + std::to_string(WorldRules::world_iteration_limit) +
                    " iterations.");
            }
            result = 0;
            world->stop();
        }
    }

    std::cout << "Stopped world " << std::endl;

    std::string recording_name = world->get_recording()->get_name();

    world.reset();
    frontend.reset();

    std::cout << std::endl << "============================================================" << std::endl;

    std::cout << "OUTCOME: " << std::endl;

    switch (result)
    {
        case 1:
        {
            std::cout << "    Program " << program1.get_name() << " " << BOT_1 << " won." << std::endl;
            break;
        }
        case 2:
        {
            std::cout << "    Program " << program2.get_name() << " " << BOT_2 << " won." << std::endl;
            break;
        }
        case 0:
        default:
        {
            std::cout << "    Draw." << std::endl;
            break;
        }
    }

    std::cout << "GAME (asciinema play) RECORDING: " << std::endl;
    std::cout << "    " << recording_name << std::endl;

    std::cout << "============================================================" << std::endl;
    std::cout << "Define KOTG_AUTOPLAY=1 to play automatically." << std::endl;
    std::cout << "Define KOTG_AUTOUPLOAD=1 to upload automatically." << std::endl;


    if (getenv("KOTG_AUTOUPLOAD"))
    {
        std::string s = "asciinema upload " + recording_name;
        std::system(s.c_str());
    }
    else
    if (getenv("KOTG_AUTOPLAY"))
    {
        std::string s = "asciinema play " + recording_name;
        std::system(s.c_str());
    }

    return result;
}

#ifdef EMSCRIPTEN
extern "C" EMSCRIPTEN_KEEPALIVE
int sum(int a, int b)
{
    return a + b;
}

extern "C" EMSCRIPTEN_KEEPALIVE
int test_programs(char* program1_name, char* program2_name, int seed)
{
    std::cout << "Hello " << program1_name << " " << program2_name << " " << seed << std::endl;
    std::cout << "Engine v" << ENGINE_VERSION << std::endl;

    std::unique_ptr<CPUProgram> first_program = std::make_unique<CPUProgram>(program1_name, program1_name, true);
    std::unique_ptr<CPUProgram> second_program = std::make_unique<CPUProgram>(program2_name, program2_name, false);

    int result = test_programs(seed, *first_program, *second_program, true);
    return result;
}
#else
int main(int argc, char** argv)
{
    srand(time(0));

    if (argc <= 1)
    {
        std::map<std::string, CPUProgram> programs {};

        for (auto program: find_bin_files("."))
        {
            programs.emplace(std::piecewise_construct,
                std::forward_as_tuple(program),
                std::forward_as_tuple(program, program + ".bin", programs.empty()));
        }

        if (programs.empty())
        {
            throw std::runtime_error("No programs found.");
        }

        if (programs.size() < 2)
        {
            throw std::runtime_error("Server runtime needs at least two runtimes.");
        }

        unsigned int master_seed_v;

        if (getenv("MASS_SEED"))
        {
            master_seed_v = atol(getenv("MASS_SEED"));
        }
        else
        {
            master_seed_v = time(0);
        }


        std::cout << "c++ version: " << __cplusplus << std::endl;

#if defined(_GLIBCXX_RELEASE)
        std::cout << "libstdc++ version: " << _GLIBCXX_RELEASE << std::endl;
#elif defined(_LIBCPP_VERSION)
        std::cout << "libc++ version: " << _LIBCPP_VERSION << std::endl;
#elif defined(_MSC_VER)
        std::cout << "MSVC version: " << _MSC_VER << std::endl;
#endif

        std::cout << "Using master (mass) seed: " << master_seed_v << " define MASS_SEED to overwrite" << std::endl;
        std::mt19937 master_seed(master_seed_v);

        std::function<int(int max)> get_master_random = [&](int max){
            std::uniform_int_distribution<int> distribution(0, max);
            return distribution(master_seed);
        };

        std::map<std::string, std::vector<std::string>> wins {};

        // Nested loop to test each program against all subsequent programs
        for (auto it1 = programs.begin(); it1 != programs.end(); ++it1)
        {
            for (auto it2 = std::next(it1); it2 != programs.end(); ++it2)
            {
                for (int i = 1; i <= 3; i++)
                {
                    int seed = get_master_random(0xFFFF);

                    std::cout << "-------------------" << std::endl;
                    std::cout << " TAKE #" << i << " testing programs " << it1->first << " and " << it2->first << " on seed " << seed << "." << std::endl;
                    std::cout << "-------------------" << std::endl;

                    {
                        int result1 = test_programs(seed, it1->second, it2->second, false);

                        std::cout << "-------------------" << std::endl;
                        switch (result1)
                        {
                            case 1:
                            {
                                it1->second.bump_score();
                                wins[it1->first].push_back("recording-" + it1->first + "-" + it2->first + "-" + std::to_string(seed) + ".txt");
                                std::cout << "A: Program " << it1->first << " " << BOT_1 << " won." << std::endl;
                                break;
                            }
                            case 2:
                            {
                                it2->second.bump_score();
                                wins[it2->first].push_back("recording-" + it1->first + "-" + it2->first + "-" + std::to_string(seed) + ".txt");
                                std::cout << "A: Program " << it2->first << " " << BOT_2 << " won." << std::endl;
                                break;
                            }
                            case 0:
                            default:
                            {
                                std::cout << "A: Draw." << std::endl;
                                break;
                            }
                        }

                        std::cout << "-------------------" << std::endl;
                    }

                    // swap places
                    {
                        int result2 = test_programs(seed, it2->second, it1->second, false);

                        switch (result2)
                        {
                            case 1:
                            {
                                it2->second.bump_score();
                                wins[it2->first].push_back("recording-" + it2->first + "-" + it1->first + "-" + std::to_string(seed) + ".txt");
                                std::cout << "B: Program " << it2->first << " " << BOT_2 << " won." << std::endl;
                                break;
                            }
                            case 2:
                            {
                                it1->second.bump_score();
                                wins[it1->first].push_back("recording-" + it2->first + "-" + it1->first + "-" + std::to_string(seed) + ".txt");
                                std::cout << "B: Program " << it1->first << " " << BOT_1 << " won." << std::endl;
                                break;
                            }
                            case 0:
                            default:
                            {
                                std::cout << "B: Draw." << std::endl;
                                break;
                            }
                        }
                    }
                }
            }
        }

        std::cout << "-------------------" << std::endl;
        std::cout << "OUTCOME" << std::endl;
        std::cout << "-------------------" << std::endl;

        {
            std::stringstream outcome;

            outcome << "{\"seed\": " << master_seed_v << ", \"results\": [";

            std::vector<CPUProgram*> sorted_programs;

            // Populate vector with pointers
            for (auto& [key, value] : programs)
            {
                sorted_programs.push_back(&value);
            }

            // Sort the vector based on a custom criteria
            std::sort(sorted_programs.begin(), sorted_programs.end(),
                [](CPUProgram* a, CPUProgram* b)
            { return a->get_score() > b->get_score(); });

            bool first_program = true;

            for (auto p: sorted_programs)
            {
                if (first_program)
                {
                    first_program = false;
                }
                else
                {
                    outcome << "," << std::endl;
                }

                outcome << "{" << std::endl;
                outcome << "    \"bot\": \"" << p->get_name() << "\"," << std::endl;
                outcome << "    \"score\": " << p->get_score() << "," << std::endl;
                outcome << "    \"wins\": [";

                bool first = true;
                for (const auto& win: wins[p->get_name()])
                {
                    if (first)
                    {
                        first = false;
                        outcome << "\"" << win << "\"";
                    }
                    else
                    {
                        outcome << ", \"" << win << "\"";
                    }
                }

                outcome << "]" << std::endl;
                outcome << "}";
            }

            outcome << "]}" << std::endl;

            std::cout << outcome.str();

            {
                std::ofstream fo("outcome.json");
                fo << outcome.str();
            }
        }

        std::cout << "-------------------" << std::endl;

        return 0;
    }
    else
    {
        std::string program1_name = argv[1];

        std::string program2_name;
        if (argc > 2)
        {
            program2_name = argv[2];
        }
        else
        {
            std::cout << "Only one program specified: using against itself." << std::endl;
            program2_name = program1_name;
        }

        int seed;

        if (argc > 3)
        {
            seed = std::atoi(argv[3]);
        }
        else
        {
            srand(time(0));
            seed = rand() & 0xFFFF;

            std::cout << "Seed not provided. Using random: " << seed << std::endl;
        }

        CPUProgram first_program(program1_name, program1_name + ".bin", true);
        CPUProgram second_program(program2_name, program2_name + ".bin", false);

        int result = test_programs(seed, first_program, second_program, false);
        return result;
    }
}
#endif