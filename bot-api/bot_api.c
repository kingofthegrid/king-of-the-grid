#include "bot_api.h"

#ifdef __JETBRAINS_IDE__
#define __IDE__
#endif

#ifndef __IDE__
#asm

DEFC PRINT_CHAR = 1
DEFC MOVE_UP = 100
DEFC MOVE_DOWN = 101
DEFC MOVE_LEFT = 102
DEFC MOVE_RIGHT = 103
DEFC GET_X = 104
DEFC GET_Y = 105
DEFC GET_ME = 106
DEFC GET_ENERGY = 107
DEFC SPLIT_UP = 108
DEFC SPLIT_DOWN = 109
DEFC SPLIT_LEFT = 110
DEFC SPLIT_RIGHT = 111
DEFC SCAN = 112
DEFC HIBERNATE = 113

syscall:
  defb $ED, $FE
  ret
#endasm
#endif

void bot_move_up()
{
#ifndef __IDE__
#asm
   ld a, MOVE_UP
   call syscall
#endasm
#endif
}

void bot_move_down()
{
#ifndef __IDE__
#asm
   ld a, MOVE_DOWN
   call syscall
#endasm
#endif
}

void bot_move_left()
{
#ifndef __IDE__
#asm
   ld a, MOVE_LEFT
   call syscall
#endasm
#endif
}

void bot_move_right()
{
#ifndef __IDE__
#asm
   ld a, MOVE_RIGHT
   call syscall
#endasm
#endif
}

char bot_get_x()
{
#ifndef __IDE__
#asm
    ld a, GET_X
    call syscall
#endasm
#endif
}

char bot_get_y()
{
#ifndef __IDE__
#asm
    ld a, GET_Y
    call syscall
#endasm
#endif
}

int bot_get_me()
{
#ifndef __IDE__
#asm
    ld a, GET_ME
    call syscall
#endasm
#endif
}

int bot_get_energy()
{
#ifndef __IDE__
#asm
    ld a, GET_ENERGY
    call syscall
#endasm
#endif
}

int bot_split_up(int energy) __z88dk_fastcall
{
#ifndef __IDE__
#asm
    ld a, SPLIT_UP
    call syscall
#endasm
#endif
}

int bot_split_down(int energy) __z88dk_fastcall
{
#ifndef __IDE__
#asm
    ld a, SPLIT_DOWN
    call syscall
#endasm
#endif
}

int bot_split_left(int energy) __z88dk_fastcall
{
#ifndef __IDE__
#asm
    ld a, SPLIT_LEFT
    call syscall
#endasm
#endif
}

int bot_split_right(int energy) __z88dk_fastcall
{
#ifndef __IDE__
#asm
    ld a, SPLIT_RIGHT
    call syscall
#endasm
#endif
}

void bot_scan(struct scan_t* scan) __z88dk_fastcall
{
#ifndef __IDE__
#asm
    ld a, SCAN
    call syscall
#endasm
#endif
}

void bot_hibernate(struct scan_t* scan) __z88dk_fastcall
{
#ifndef __IDE__
#asm
    ld a, HIBERNATE
    call syscall
#endasm
#endif
}