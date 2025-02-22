#ifndef __BOT_API_H__
#define __BOT_API_H__

/* C API for bots */

#ifdef __SDCC
#define CALLAPI
#else
#define CALLAPI __z88dk_fastcall
#endif

// Scanning area size (square), always odd
#define SCAN_SIZE (7)
// Scanning area size / 2 - 1
#define SCAN_SIZE_HALF (3)
// Total number of items in scanning area
#define SCAN_SIZE_SQ (49)
// Scan cell: doesn't contain anything
#define SCAN_NOTHING (0)
// Scan cell: enemy bot
#define SCAN_ENEMY (1)
// Scan cell: food
#define SCAN_FOOD (2)
// Scan cell: friendly bot (my copy)
#define SCAN_FRIEND (3)
// Scan cell: a wall
#define SCAN_WALL (4)

struct scan_t
{
    char scan_result[SCAN_SIZE_SQ];
};

/* get current bot's X coordinate. */
extern char bot_get_x(void) CALLAPI;

/* get current bot's Y coordinate. */
extern char bot_get_y(void) CALLAPI;

/* get current bot's ID. */
extern unsigned int bot_get_me(void) CALLAPI;

/* get current bot's remaining energy value. */
extern unsigned int bot_get_energy(void) CALLAPI;

/* get current seed (randomness). */
extern unsigned int bot_get_seed(void) CALLAPI;

/* Move bot up.
 * Blocks the execution until the bot has completed the movement.
 * Returns 1 on success and 0 if the movement was blocked.
 * Takes some energy if successful. */
extern unsigned char bot_move_up(void) CALLAPI;

/* Move bot down.
 * Blocks the execution until the bot has completed the movement.
 * Returns 1 on success and 0 if the movement was blocked.
 * Takes some energy if successful. */
extern unsigned char bot_move_down(void) CALLAPI;

/* Move bot left.
 * Blocks the execution until the bot has completed the movement.
 * Returns 1 on success and 0 if the movement was blocked.
 * Takes some energy if successful. */
extern unsigned char bot_move_left(void) CALLAPI;

/* Move bot right.
 * Blocks the execution until the bot has completed the movement.
 * Returns 1 on success and 0 if the movement was blocked.
 * Takes some energy if successful. */
extern unsigned char bot_move_right(void) CALLAPI;

/* Make a clone of myself in the cell up.
 * Blocks until cloning is complete.
 * Must specify amount of energy a clone receives, must be less than available.
 * If less energy is available, the bot will move there instead.
 * Blocks until cloning is complete.
 * Takes some energy (on top of energy that is given up to the clone.
 * Returns ID of a newly created bot, current bot ID if it was moved instead or 0 if failed.
 * Please note that this function behaves like fork() in linux: both bots continue from the same location, saving all
 * context. You can check if you're in the cloners execution space or in the clonee by checking the return ID against
 * bot_get_me(). */
extern unsigned int bot_split_up(unsigned int energy) CALLAPI;

/* Make a clone of myself in the cell down.
 * Blocks until cloning is complete.
 * Must specify amount of energy a clone receives, must be less than available.
 * If less energy is available, the bot will move there instead.
 * Blocks until cloning is complete.
 * Takes some energy (on top of energy that is given up to the clone.
 * Returns ID of a newly created bot, current bot ID if it was moved instead or 0 if failed.
 * Please note that this function behaves like fork() in linux: both bots continue from the same location, saving all
 * context. You can check if you're in the cloners execution space or in the clonee by checking the return ID against
 * bot_get_me(). */
extern unsigned int bot_split_down(unsigned int energy) CALLAPI;

/* Make a clone of myself in the cell left.
 * Blocks until cloning is complete.
 * Must specify amount of energy a clone receives, must be less than available.
 * If less energy is available, the bot will move there instead.
 * Blocks until cloning is complete.
 * Takes some energy (on top of energy that is given up to the clone.
 * Returns ID of a newly created bot, current bot ID if it was moved instead or 0 if failed.
 * Please note that this function behaves like fork() in linux: both bots continue from the same location, saving all
 * context. You can check if you're in the cloners execution space or in the clonee by checking the return ID against
 * bot_get_me().*/
extern unsigned int bot_split_left(unsigned int energy) CALLAPI;

/* Make a clone of myself in the cell right.
 * Blocks until cloning is complete.
 * Must specify amount of energy a clone receives, must be less than available.
 * If less energy is available, the bot will move there instead.
 * Blocks until cloning is complete.
 * Takes some energy (on top of energy that is given up to the clone.
 * Returns ID of a newly created bot, current bot ID if it was moved instead or 0 if failed.
 * Please note that this function behaves like fork() in linux: both bots continue from the same location, saving all
 * context. You can check if you're in the cloners execution space or in the clonee by checking the return ID against
 * bot_get_me(). */
extern unsigned int bot_split_right(unsigned int energy) CALLAPI;

/* Performs surrounding scan. Variable scan is modified.
 * Blocks until scanning is complete.
 * Takes some energy. */
extern void bot_scan(struct scan_t* scan) CALLAPI;

/* Sleep for a while to conserve energy. */
extern void bot_hibernate(void) CALLAPI;

/* By default, bots do not have shared memory between themselves, e.g. every instance has 0x0000 - 0xFFFF memory all
 * to itself. This call makes the memory area 0xE000 - 0xFFFF shareable.
 * 1) Any write to addresses 0xE000 - 0xFFFF will be stored for all bots that have enabled shared memory.
 * 2) Any read will return shareable memory – potentially written by other instances of a bot.
 *
 * Keep in mind, that
 * a) when shared memory is enabled, "local" memory region 0xE000 - 0xFFFF is lost, therefore shared memory shall
 *    be enabled as soon as possible.
 * b) if your stack pointer (SP) is located in that region – which is typically default – you will see unexpected
 *    issues, therefore take care to set SP to 0xE000 on init. On z88dk, this is done using
 *    "-pragma-define=REGISTER_SP=57344", on sdcc you will need to have a custom crt0. (which sdcc-backend does).
 *
 * By default, shared memory is initialized with zeros. */
extern char bot_enable_shared_memory(void) CALLAPI;

#endif
