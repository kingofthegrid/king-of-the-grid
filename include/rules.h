#pragma once

struct WorldRules
{
    /* ============================ WORLD ============================ */

    /* World width in cells */
    static int world_width;
    /* World height in cells */
    static int world_height;
    /* Iteration limit (when reached, draw is declared) */
    static int world_iteration_limit;

    /* ============================ PREY ============================ */

    /* Maximum prey population */
    static int prey_max_population;
    /* How often to spawn prey */
    static int prey_spawn_every_nth_tick;
    /* Amount of energy spawned prey is given */
    static int prey_spawn_energy_with;

    /* ============================ BOTS ============================ */

    /* How often (ticks) bot loses 1 point of energy */
    static int bot_energy_lost_every_nth_tick;
    /* Max energy bot is allowed to have */
    static int bot_energy_max;
    /* Amount of energy bots are spawned with */
    static int bot_energy_spawn;
    /* How many ticks (z80) bot's CPU performs per tick */
    static int bot_z80_ticks_per_tick;
    /* Half of scan size (square size = bot_scan_half_size * 2 + 1) */
    static int bot_scan_half_size;

    /* ============================ FOOD ============================ */

    /* How often to spawn food */
    static int food_spawn_every_nth_tick;
    /* Amount of energy spawned food is given */
    static int food_spawn_energy_with;
    /* Amount of ticks food loses 1 energy point */
    static int food_decay_rate;
    /* When food is eaten, bot receives energy * multiplier */
    static int food_energy_multiplier;

    /* ========================= ENERGY PRICES ========================= */

    /* Amount of energy needed to perform a scan */
    static int energy_to_scan;
    /* Amount of energy needed to perform a move */
    static int energy_to_move;

    /* ========================= TIME PENALTIES ========================= */

    /* Amount of time (ticks) spent to perform a scan */
    static int time_to_scan;
    /* Amount of time (ticks) spent to perform a move */
    static int time_to_move;
    /* Amount of time (ticks) spent on hibernation when bots decides to hibernate */
    static int time_on_hibernate;
};
