public _bot_move_up
public _bot_move_down
public _bot_move_left
public _bot_move_right
public _bot_get_x
public _bot_get_y
public _bot_get_me
public _bot_get_energy
public _bot_get_seed
public _bot_split_up
public _bot_split_down
public _bot_split_left
public _bot_split_right
public _bot_scan
public _bot_hibernate
public _bot_enable_shared_memory

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
DEFC GET_SEED = 114
DEFC ENABLE_SHARED_MEMORY = 115

_bot_move_up:
   ld a, MOVE_UP
   defb $ED, $FE
   ret

_bot_move_down:
    ld a, MOVE_DOWN
    defb $ED, $FE
    ret

_bot_move_left:
    ld a, MOVE_LEFT
    defb $ED, $FE
    ret

_bot_move_right:
    ld a, MOVE_RIGHT
    defb $ED, $FE
    ret

_bot_get_x:
    ld a, GET_X
    defb $ED, $FE
    ret

_bot_get_y:
    ld a, GET_Y
    defb $ED, $FE
    ret

_bot_get_me:
    ld a, GET_ME
    defb $ED, $FE
    ret

_bot_get_energy:
    ld a, GET_ENERGY
    defb $ED, $FE
    ret

_bot_get_seed:
    ld a, GET_SEED
    defb $ED, $FE
    ret

_bot_split_up:
    ld a, SPLIT_UP
    defb $ED, $FE
    ret

_bot_split_down:
    ld a, SPLIT_DOWN
    defb $ED, $FE
    ret

_bot_split_left:
    ld a, SPLIT_LEFT
    defb $ED, $FE
    ret

_bot_split_right:
    ld a, SPLIT_RIGHT
    defb $ED, $FE
    ret

_bot_scan:
    ld a, SCAN
    defb $ED, $FE
    ret

_bot_hibernate:
    ld a, HIBERNATE
    defb $ED, $FE
    ret

_bot_enable_shared_memory:
    ld a, ENABLE_SHARED_MEMORY
    defb $ED, $FE
    ret
