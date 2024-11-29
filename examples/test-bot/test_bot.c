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
    srand(0);

    bot_hibernate();

    while (1)
    {
        for (char i = 0; i < 5; i++)
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

        if (scan_food())
        {
            printf("Found food at %dx%d\n", food_x, food_y);
            // found food

            while (bot_get_x() < food_x)
            {
                bot_move_right();
            }

            while (bot_get_x() > food_x)
            {
                bot_move_left();
            }

            while (bot_get_y() < food_y)
            {
                bot_move_down();
            }

            while (bot_get_y() > food_y)
            {
                bot_move_up();
            }

            // we should have eaten the food
            printf("Reached the food.\n");
        }
    }
}

