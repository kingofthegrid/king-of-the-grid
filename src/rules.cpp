#include "rules.h"

/* World width in cells */
int WorldRules::world_width
    = 32;

/* World height in cells */
int WorldRules::world_height
    = 32;

/* Iteration limit (when reached, draw is declared) */
int WorldRules::world_iteration_limit
    = 100000;

/* Maximum prey population */
int WorldRules::prey_max_population
    = 10;

/* How often to spawn prey */
int WorldRules::prey_spawn_every_nth_tick
    = 1000;

/* Amount of energy spawned prey is given */
int WorldRules::prey_spawn_energy_with
    = 20000;

/* How often (ticks) bot loses 1 point of energy */
int WorldRules::bot_energy_lost_every_nth_tick
    = 10;

/* Max energy bot is allowed to have */
int WorldRules::bot_energy_max
    = 65536;

/* Amount of energy bots are spawned with */
int WorldRules::bot_energy_spawn
    = 40000;

/* How many ticks (z80) bot's CPU performs per tick */
int WorldRules::bot_z80_ticks_per_tick
    = 10000;

/* Half of scan size (square size = bot_scan_half_size * 2 + 1) */
int WorldRules::bot_scan_half_size
    = 3;

/* How often to spawn food */
int WorldRules::food_spawn_every_nth_tick
    = 250;

/* Amount of energy spawned food is given */
int WorldRules::food_spawn_energy_with
    = 500;

/* Amount of ticks food loses 1 energy point */
int WorldRules::food_decay_rate
    = 50;

/* When food is eaten, bot receives energy * multiplier */
int WorldRules::food_energy_multiplier
    = 64;

/* Amount of energy needed to perform a scan */
int WorldRules::energy_to_scan
    = 1000;

/* Amount of energy needed to perform a move */
int WorldRules::energy_to_move
    = 20;

/* Amount of time (ticks) spent to perform a scan */
int WorldRules::time_to_scan
    = 20;

/* Amount of time (ticks) spent to perform a move */
int WorldRules::time_to_move
    = 100;

/* Amount of time (ticks) spent on hibernation when bots decides to hibernate */
int WorldRules::time_on_hibernate
    = 1000;
