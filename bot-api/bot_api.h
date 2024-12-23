#ifndef __BOT_API_H__
#define __BOT_API_H__

/* C API for bots */

#ifdef __JETBRAINS_IDE__
#define __z88dk_fastcall
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

struct scan_t
{
    char scan_result[SCAN_SIZE_SQ];
};

/* get current bot's X coordinate. */
extern char bot_get_x();

/* get current bot's Y coordinate. */
extern char bot_get_y();

/* get current bot's ID. */
extern unsigned int bot_get_me();

/* get current bot's remaining energy value. */
extern unsigned int bot_get_energy();

/* get current seed (randomness). */
extern unsigned int bot_get_seed();

/* Move bot up.
 * Blocks the execution until the bot has completed the movement.
 * Takes some energy. */
extern void bot_move_up();

/* Move bot down.
 * Blocks the execution until the bot has completed the movement.
 * Takes some energy. */
extern void bot_move_down();

/* Move bot left.
 * Blocks the execution until the bot has completed the movement.
 * Takes some energy. */
extern void bot_move_left();

/* Move bot right.
 * Blocks the execution until the bot has completed the movement.
 * Takes some energy. */
extern void bot_move_right();

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
extern int bot_split_up(int energy) __z88dk_fastcall;

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
extern int bot_split_down(int energy) __z88dk_fastcall;

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
extern int bot_split_left(int energy) __z88dk_fastcall;

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
extern int bot_split_right(int energy) __z88dk_fastcall;

/* Performs surrounding scan. Variable scan is modified.
 * Blocks until scanning is complete.
 * Takes some energy. */
extern void bot_scan(struct scan_t* scan) __z88dk_fastcall;

/* Sleep for a while to conserve energy. */
extern void bot_hibernate() __z88dk_fastcall;

#endif
