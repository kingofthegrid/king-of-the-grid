#pragma once

#include <cstdint>
#include <string>
#include <sstream>
#include "bot.h"
#include "Z80.h"
#include "defines.h"

class Frontend;

class CPUProgram
{
public:
    explicit CPUProgram(const std::string& name, const std::string& filename, bool first);

    const uint8_t* get_program_memory() const { return m_program; }
    uint8_t* get_shared_memory() { return m_shared_ram; }
    const std::string& get_name() const { return m_name; }

    void add_count() { m_count++; }
    void remove_count() { m_count--; }
    bool lost() { return m_count <= 0; }
    bool is_first() const { return m_first; }

    int get_score() const { return m_score; }
    void bump_score() { m_score++; }

private:
    std::string m_name;
    uint8_t m_program[TOTAL_MEM_SIZE - SHARED_MEM_SIZE];
    uint8_t m_shared_ram[SHARED_MEM_SIZE];
    int m_count;
    bool m_first;
    int m_score;
};

struct scan_t
{
    std::vector<char> scan_result;
};

class CPUBot: public Bot
{
public:
    explicit CPUBot(Frontend& frontend, CPUProgram& program, World& world, int x, int y, int energy);
    ~CPUBot() override;

    void simulate() override;
    bool is_enemy(Bot* bot) override;

    int split(int x, int y, int energy);
    void on_stdout(char c);
    void scan(struct scan_t* scan);
    void hibernate();

    uint8_t* get_private_memory() { return m_private_memory; }
    uint8_t* get_shared_memory() { return m_program.get_shared_memory(); }
    Z80* get_cpu() { return &m_cpu; }
    const CPUProgram& get_program() const { return m_program; }

    bool is_prey() const override { return false; }
    int get_bot_type() const override;

private:
    CPUProgram& m_program;
    Z80 m_cpu;
    uint8_t m_private_memory[TOTAL_MEM_SIZE];
    std::stringstream m_stdout;
    std::stringstream m_stdout_total;
};