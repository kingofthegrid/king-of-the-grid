#include "cpu_bot.h"
#include "Z80.h"
#include "bot_api.h"
#include "world.h"
#include "recording.h"
#include "rules.h"
#include <fstream>
#include <iostream>
#include <cstring>
#include <ctype.h>
#include "utils.h"

static void z80_retn(void *context);
static zuint8 z80_nmi_read(void *context, zuint16 address);
static zuint8 z80_int_fetch_read(void *context, zuint16 address);
static void z80_io_write(void *context, zuint16 address, zuint8 value);
static zuint8 z80_io_read(void *context, zuint16 address);
static void z80_memory_write(void *context, zuint16 address, zuint8 value);
static zuint8 z80_memory_fetch(void *context, zuint16 address);
static zuint8 z80_memory_fetch_opcode(void *context, zuint16 address);
static zuint8 z80_memory_read(void *context, zuint16 address);

void replace_all(std::string &str, const std::string &from, const std::string &to)
{
    if (from.empty()) {
        return; // Avoid infinite loop if 'from' is an empty string
    }

    size_t pos = 0;

    while ((pos = str.find(from, pos)) != std::string::npos) {
        str.replace(pos, from.length(), to);
        pos += to.length(); // Advance position past the inserted substring
    }
}

static zuint8 z80_memory_read(void *context, zuint16 address)
{
    auto* computer = (CPUBot*)context;

    if (computer->is_shared_memory_enabled() && (address >= SHARED_MEM_LOCATION))
    {
        return computer->get_shared_memory()[address - SHARED_MEM_LOCATION];
    }
    else
    {
        return computer->get_private_memory()[address & 0xffff];
    }
}

static zuint8 z80_illegal_instruction(Z80 *cpu, zuint8 opcode)
{
    Z80_PC(*cpu) += 2;

    switch (opcode)
    {
        case 0xFE:
        {
            uint8_t a = Z80_A(*cpu);

            switch (a)
            {
                // EXIT
                case 0:
                {
                    auto* computer = (CPUBot*)cpu->context;
                    computer->kill();
                    z80_break(cpu);
                    break;
                }
                // PRINTCHAR
                case 1:
                {
                    uint8_t l = Z80_L(*cpu);
                    auto* computer = (CPUBot*)cpu->context;
                    computer->on_stdout((char)l);
                    return 0;
                }
                case CMD_HIBERNATE:
                {
                    auto* computer = (CPUBot*)cpu->context;
                    computer->hibernate();
                    z80_break(cpu);
                    break;
                }
                case CMD_SPLIT_UP:
                {
                    auto* computer = (CPUBot*)cpu->context;
                    int id = computer->split(0, -1, Z80_HL(*cpu));
                    Z80_HL(*cpu) = id;
                    Z80_DE(*cpu) = id;
                    z80_break(cpu);
                    break;
                }
                case CMD_SPLIT_DOWN:
                {
                    auto* computer = (CPUBot*)cpu->context;
                    int id = computer->split(0, 1, Z80_HL(*cpu));;
                    Z80_HL(*cpu) = id;
                    Z80_DE(*cpu) = id;
                    z80_break(cpu);
                    break;
                }
                case CMD_SPLIT_LEFT:
                {
                    auto* computer = (CPUBot*)cpu->context;
                    int id = computer->split(-1, 0, Z80_HL(*cpu));;
                    Z80_HL(*cpu) = id;
                    Z80_DE(*cpu) = id;
                    z80_break(cpu);
                    break;
                }
                case CMD_SPLIT_RIGHT:
                {
                    auto* computer = (CPUBot*)cpu->context;
                    int id = computer->split(1, 0, Z80_HL(*cpu));;
                    Z80_HL(*cpu) = id;
                    Z80_DE(*cpu) = id;
                    z80_break(cpu);
                    break;
                }
                case CMD_MOVE_UP:
                {
                    auto* computer = (CPUBot*)cpu->context;
                    if (computer->move_up())
                    {
                        Z80_HL(*cpu) = 1;
                        Z80_DE(*cpu) = 1;
                        z80_break(cpu);
                    }
                    else
                    {
                        Z80_HL(*cpu) = 0;
                        Z80_DE(*cpu) = 0;
                    }
                    break;
                }
                case CMD_MOVE_DOWN:
                {
                    auto* computer = (CPUBot*)cpu->context;
                    if (computer->move_down())
                    {
                        Z80_HL(*cpu) = 1;
                        Z80_DE(*cpu) = 1;
                        z80_break(cpu);
                    }
                    else
                    {
                        Z80_HL(*cpu) = 0;
                        Z80_DE(*cpu) = 0;
                    }
                    break;
                }
                case CMD_MOVE_LEFT:
                {
                    auto* computer = (CPUBot*)cpu->context;
                    if (computer->move_left())
                    {
                        Z80_HL(*cpu) = 1;
                        Z80_DE(*cpu) = 1;
                        z80_break(cpu);
                    }
                    else
                    {
                        Z80_HL(*cpu) = 0;
                        Z80_DE(*cpu) = 0;
                    }
                    break;
                }
                case CMD_MOVE_RIGHT:
                {
                    auto* computer = (CPUBot*)cpu->context;
                    if (computer->move_right())
                    {
                        Z80_HL(*cpu) = 1;
                        Z80_DE(*cpu) = 1;
                        z80_break(cpu);
                    }
                    else
                    {
                        Z80_HL(*cpu) = 0;
                        Z80_DE(*cpu) = 0;
                    }
                    break;
                }
                case CMD_SCAN:
                {
                    auto* computer = (CPUBot*)cpu->context;
                    struct scan_t scan = {};
                    uint16_t mem = Z80_HL(*cpu);
                    computer->scan(&scan);
                    memcpy(computer->get_private_memory() + mem, scan.scan_result.data(), scan.scan_result.size());
                    z80_break(cpu);
                    break;
                }
                case CMD_GET_X:
                {
                    auto* computer = (CPUBot*)cpu->context;
                    Z80_HL(*cpu) = computer->get_x();
                    Z80_DE(*cpu) = computer->get_x();
                    break;
                }
                case CMD_GET_Y:
                {
                    auto* computer = (CPUBot*)cpu->context;
                    Z80_HL(*cpu) = computer->get_y();
                    Z80_DE(*cpu) = computer->get_y();
                    break;
                }
                case CMD_GET_ME:
                {
                    auto* computer = (CPUBot*)cpu->context;
                    Z80_HL(*cpu) = computer->get_id();
                    Z80_DE(*cpu) = computer->get_id();
                    break;
                }
                case CMD_GET_ENERGY:
                {
                    auto* computer = (CPUBot*)cpu->context;
                    Z80_HL(*cpu) = computer->get_energy();
                    Z80_DE(*cpu) = computer->get_energy();
                    break;
                }
                case CMD_GET_SEED:
                {
                    auto* computer = (CPUBot*)cpu->context;
                    Z80_HL(*cpu) = computer->get_seed();
                    Z80_DE(*cpu) = computer->get_seed();
                    break;
                }
                case CMD_ENABLE_SHARED_MEMORY:
                {
                    auto* computer = (CPUBot*)cpu->context;
                    computer->enable_shared_memory();
                    break;
                }
                default:
                {
                    break;
                }
            }

            return 0;
        }
    }

    return 0;
}

static zuint8 z80_memory_fetch_opcode(void *context, zuint16 address)
{
    zuint8 opcode = z80_memory_read(context, address);
    return opcode;
}

static zuint8 z80_memory_fetch(void *context, zuint16 address)
{
    return z80_memory_read(context, address);
}

static void z80_memory_write(void *context, zuint16 address, zuint8 value)
{
    auto* computer = (CPUBot*)context;

    if (computer->is_shared_memory_enabled() && (address >= SHARED_MEM_LOCATION))
    {
        computer->get_shared_memory()[address - SHARED_MEM_LOCATION] = value;
    }
    else
    {
        computer->get_private_memory()[address & 0xffff] = value;
    }
}

static zuint8 z80_io_read(void *context, zuint16 address)
{
    return 0;
}

static void z80_io_write(void *context, zuint16 address, zuint8 value)
{
}

static zuint8 z80_int_fetch_read(void *context, zuint16 address)
{
    return 0;
}

static zuint8 z80_nmi_read(void *context, zuint16 address)
{
    return 0;
}

static void z80_retn(void *context)
{
}

CPUProgram::CPUProgram(const std::string& name, const std::string& filename) :
    m_name(name),
    m_program {},
    m_shared_ram {},
    m_score(0)
{
    std::cout << "Reading program " << name << " ..." << std::endl;

    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        exit(-1);
    }

    std::streamsize file_size = file.tellg();
    if (file_size > sizeof(m_program))
    {
        std::cerr << "Program is too big: " << std::to_string(file_size) << std::endl;
        exit(-1);
    }

    file.seekg(0, std::ios::beg); // Move back to the beginning

    // Read the data into the array
    if (!file.read(reinterpret_cast<char*>(m_program), file_size))
    {
        std::cerr << "Failed to read file data." << std::endl;
        exit(-1);
    }

    file.close();
}

CPUBot::CPUBot(Frontend& frontend, CPUProgram& program, World& world, int x, int y, int energy, bool bot_1) :
    Bot(frontend, "CPU " + program.get_name(), world, x, y, energy),
    m_program(program),
    m_cpu {},
    m_private_memory {},
    m_stdout(),
    m_stdout_total(),
    m_seed(world.get_seed()),
    m_shared_memory_enabled(false),
    m_bot_1(bot_1)
{
    m_program.add_count();

    memcpy(m_private_memory, program.get_program_memory(), TOTAL_MEM_SIZE);

    m_cpu.context = this;
    m_cpu.fetch = z80_memory_fetch;
    m_cpu.fetch_opcode = z80_memory_fetch_opcode;
    m_cpu.read = z80_memory_read;
    m_cpu.write = z80_memory_write;
    m_cpu.in = z80_io_read;
    m_cpu.out = z80_io_write;
    m_cpu.int_fetch = z80_int_fetch_read;
    m_cpu.nmia = z80_nmi_read;
    m_cpu.retn = z80_retn;
    m_cpu.options = Z80_OPTION_HALT_SKIP;
    m_cpu.illegal = z80_illegal_instruction;

    z80_power(&m_cpu, Z_TRUE);

    Z80_PC(m_cpu) = 0;
}

CPUBot::~CPUBot()
{
    m_program.remove_count();

    {
        auto out = m_stdout.str();
        if (!out.empty())
        {
            std::cout << get_name() << " | " << out;

            if (m_world.get_recording())
            {
                replace_all(out, "\n", "\\n");
                m_world.get_recording()->add_stdout(m_bot_1 ? 0 : 1, out);
            }
        }
    }
    {
        const auto& out = m_stdout_total.str();
        if (!out.empty())
        {
            std::cout << get_name() << " full stdout:" << std::endl;
            std::cout << "-----------------------------------" << std::endl;
            std::cout << escape_str(out);
            std::cout << "-----------------------------------" << std::endl;
        }
    }
}

bool CPUBot::is_enemy(Bot* bot)
{
    if (!bot->is_cpu())
    {
        return true;
    }
    auto* other = static_cast<CPUBot*>(bot);
    return &other->m_program != &m_program;
}

void CPUBot::simulate()
{
    Bot::simulate();

    if (get_state() == BotState::normal)
    {
        z80_run(&m_cpu, WorldRules::bot_z80_ticks_per_tick);
    }
}

void CPUBot::hibernate()
{
    m_move_timer = WorldRules::time_on_hibernate;
    set_state(BotState::hibernating);
}

void CPUBot::scan(struct scan_t* scan)
{
    m_energy -= WorldRules::energy_to_scan;
    m_move_timer = WorldRules::time_to_scan;
    set_state(BotState::scanning);

    int sz = WorldRules::bot_scan_half_size * 2 + 1;
    scan->scan_result.resize(sz * sz);
    memset(scan->scan_result.data(), 0, scan->scan_result.size());

    int xx = get_x();
    int yy = get_y();
    char *c = scan->scan_result.data();

    for (int y = -WorldRules::bot_scan_half_size; y <= WorldRules::bot_scan_half_size; y++)
    {
        for (int x = -WorldRules::bot_scan_half_size; x <= WorldRules::bot_scan_half_size; x++, c++)
        {
            int x1 = xx + x;
            int y1 = yy + y;

            if (x1 < 0 || y1 < 0)
                continue;

            if (x1 >= WorldRules::world_width || y1 >= WorldRules::world_height)
                continue;

            auto& cell = m_world.get_cell(x1, y1);

            char result;

            if (cell.is_food())
            {
                result = SCAN_FOOD;
            }
            else if (cell.is_bot())
            {
                auto bot = cell.m_bot_value.lock();

                if (bot)
                {
                    if (bot->is_enemy(this))
                    {
                        result = SCAN_ENEMY;
                    }
                    else
                    {
                        result = SCAN_FRIEND;
                    }
                }
                else
                {
                    result = SCAN_NOTHING;
                }
            }
            else if (cell.is_wall())
            {
                result = SCAN_WALL;
            }
            else
            {
                result = SCAN_NOTHING;
            }

            *c = result;
        }
    }
}

int CPUBot::split(int x, int y, int energy)
{
    if (energy == 0)
    {
        return 0;
    }

    if (energy >= get_energy())
    {
        move(x, y);
        return 0;
    }
    else
    {
        int new_x = get_x() + x;
        int new_y = get_y() + y;

        if (new_x < 0 || new_y < 0)
            return 0;

        if (new_x >= WorldRules::world_width || new_y >= WorldRules::world_height)
            return 0;

        auto& clone_cell = m_world.get_cell(new_x, new_y);
        if (clone_cell.is_wall())
            return 0;

        m_energy -= energy;
        int new_bot_energy = energy;

        if (clone_cell.is_food())
        {
            new_bot_energy += clone_cell.m_food_value * WorldRules::food_energy_multiplier;
        }
        else if (clone_cell.is_bot())
        {
            auto bot = clone_cell.m_bot_value.lock();

            if (bot)
            {
                if (bot->is_enemy(this))
                {
                    new_bot_energy += bot->get_energy();
                }
                else
                {
                    return 0;
                }
            }
        }

        auto new_bot = std::make_shared<CPUBot>(
            m_frontend, m_program, m_world, new_x, new_y, new_bot_energy, m_bot_1);
        new_bot->m_source_x = get_x();
        new_bot->m_source_y = get_y();
        m_world.add_bot(m_frontend, new_bot);

        // clone state and private memory to a new bot
        // so when it executes, it gets the same context (PC & etc)
        new_bot->m_cpu = m_cpu;
        // fix context after
        new_bot->m_cpu.context = new_bot.get();
        memcpy(new_bot->m_private_memory, m_private_memory, sizeof(m_private_memory));

        // fix HL/DE for new context
        Z80_HL(new_bot->m_cpu) = new_bot->get_id();
        Z80_DE(new_bot->m_cpu) = new_bot->get_id();

        new_bot->set_state(BotState::splitting);
        new_bot->m_move_timer = 0;
        return new_bot->get_id();
    }
}

void CPUBot::on_stdout(char c)
{
    if ( c == '\n' || c == '\r' )
    {
        m_stdout_total << std::endl;
        std::cout << get_name() << " | " << m_stdout.str() << std::endl;
        std::string o = m_stdout.str();
        replace_all(o, "\n", "\\n");
        replace_all(o, "\r", "");
        m_world.get_recording()->add_stdout(m_bot_1 ? 0 : 1, o);

        m_stdout.str("");
    }
    else
    {
        if (isprint((unsigned char)c))
        {
            m_stdout << c;
            m_stdout_total << c;
        }
    }
}

int CPUBot::get_bot_type() const
{
    return m_bot_1 ? Recording::CELL_BOT_1 : Recording::CELL_BOT_2;
}
