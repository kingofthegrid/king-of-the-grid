#include <stdio.h>
#include <stdlib.h>

#include "bot_api.h"

int main()
{
    srand(bot_get_seed());

    printf("A simple program that prints\nout scan map as bot moves!\n");

    int scan_n = 1;

    while (1)
    {
        int x = rand() % 4;

        for (int i = 0; i < 8; i++)
        {
            switch (x)
            {
                case 0: bot_move_down(); break;
                case 1: bot_move_left(); break;
                case 2: bot_move_right(); break;
                case 3: bot_move_up(); break;
            }
        }

        printf("Scanning at %dx%d...\n", bot_get_x(), bot_get_y());

        struct scan_t results;

        bot_scan(&results);

        char* scan_result = results.scan_result;

        printf("Scan %d: \n", scan_n);
        scan_n++;

        for (int y = 0; y < 7; y++)
        {
            for (int x = 0; x < 7; x++)
            {
                char cell = *scan_result;

                if (cell)
                {
                    printf("%d", cell);
                }
                else
                {
                    printf(".");
                }

                scan_result++;
            }

            printf("\n");
        }

        bot_hibernate();

    }
}

