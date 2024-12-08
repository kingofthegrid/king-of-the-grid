#include <stdio.h>
#include <stdlib.h>

#include "bot_api.h"

char food_x;
char food_y;

static char scan_food()
{
    struct scan_t scan;
    bot_scan(&scan);

    char* scan_block = scan.scan_result;

    for (char y = -SCAN_SIZE_HALF; y <= SCAN_SIZE_HALF; y++)
    {
        for (char x = -SCAN_SIZE_HALF; x <= SCAN_SIZE_HALF; x++, scan_block++)
        {
            if (*scan_block == SCAN_FOOD)
            {
                food_x = bot_get_x() + x;
                food_y = bot_get_y() + y;
                return 1;
            }
        }
    }

    return 0;
}

int main()
{
    printf("Hello, world!\n");
    srand(bot_get_seed());

    bot_hibernate();

    while (1)
    {
        for (int i = 0; i < 20; i++)
        {
            switch (rand() % 4) {
                case 0:
                {
                    bot_move_up();
                    bot_move_up();
                    break;
                }
                case 1:
                {
                    bot_move_down();
                    bot_move_down();
                    break;
                }
                case 2:
                {
                    bot_move_left();
                    bot_move_left();
                    break;
                }
                case 3:
                {
                    bot_move_right();
                    bot_move_right();
                    break;
                }
            }
        }

        printf("Cloning myself!\n");

        int clone_id;

        switch (rand() % 4) {
            case 0:
            {
                clone_id = bot_split_down(4000);
                break;
            }
            case 1:
            {
                clone_id = bot_split_up(4000);
                break;
            }
            case 2:
            {
                clone_id = bot_split_left(4000);
                break;
            }
            case 3:
            default:
            {
                clone_id = bot_split_right(4000);
                break;
            }
        }

        if (clone_id == 0)
        {
            // clone failed
        }
        else if (clone_id == bot_get_me())
        {
            printf("I was just cloned!\n");
        }
        else
        {
            printf("Cloned: %d!\n", clone_id);
        }
    }
}

