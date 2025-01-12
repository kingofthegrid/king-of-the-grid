; crt0 for SDCC
; build this using the following command
; sdasz80 -o sdcc-backend.rel sdcc-backend.asm
; and then include it as part of your code
; sdcc sdcc-backend.rel yourcode.rel -o result

	.module crt0
	.globl	_main

	.area	_HEADER (ABS)
	;; Reset vector
	.org 	0
	jp	init

	.org	0x08
	reti
	.org	0x10
	reti
	.org	0x18
	reti
	.org	0x20
	reti
	.org	0x28
	reti
	.org	0x30
	reti
	.org	0x38
	reti

	.org	0x100
init:
	ld	sp, #0xE000

    ;; Initialise globl variables
	call	_main
	jp	_exit

	;; Ordering of segments for the linker.
	.area	_HOME
	.area	_CODE
	.area	_INITIALIZER
	.area   _GSINIT
	.area   _GSFINAL

	.area	_DATA
	.area	_INITIALIZED
	.area	_BSEG
	.area   _BSS
	.area   _HEAP

	.area   _CODE

__clock::
	ld	a,#2
	rst     0x08
	ret

_exit::
	;; Exit - special code to the emulator
	ld	a,#0
	.db 0xED, 0xFE
1$:
	halt
	jr	1$

	.area   _GSINIT
	.area   _GSFINAL
	ret


    .globl _bot_move_up
    .globl _bot_move_down
    .globl _bot_move_left
    .globl _bot_move_right
    .globl _bot_get_x
    .globl _bot_get_y
    .globl _bot_get_me
    .globl _bot_get_energy
    .globl _bot_get_seed
    .globl _bot_split_up
    .globl _bot_split_down
    .globl _bot_split_left
    .globl _bot_split_right
    .globl _bot_scan
    .globl _bot_hibernate
    .globl _bot_enable_shared_memory
    .globl _putchar


_bot_move_up:
    ld a, #100
    .db 0xED, 0xFE
    ret

_bot_move_down:
    ld a, #101
    .db 0xED, 0xFE
    ret

_bot_move_left:
    ld a, #102
    .db 0xED, 0xFE
    ret

_bot_move_right:
    ld a, #103
    .db 0xED, 0xFE
    ret

_bot_get_x:
    ld a, #104
    .db 0xED, 0xFE
    ret

_bot_get_y:
    ld a, #105
    .db 0xED, 0xFE
    ret

_bot_get_me:
    ld a, #106
    .db 0xED, 0xFE
    ret

_bot_get_energy:
    ld a, #107
    .db 0xED, 0xFE
    ret

_bot_get_seed:
    ld a, #114
    .db 0xED, 0xFE
    ret

_bot_split_up:
    ld a, #108
    .db 0xED, 0xFE
    ret

_bot_split_down:
    ld a, #109
    .db 0xED, 0xFE
    ret

_bot_split_left:
    ld a, #110
    .db 0xED, 0xFE
    ret

_bot_split_right:
    ld a, #111
    .db 0xED, 0xFE
    ret

_bot_scan:
    ld a, #112
    .db 0xED, 0xFE
    ret

_bot_hibernate:
    ld a, #113
    .db 0xED, 0xFE
    ret

_bot_enable_shared_memory:
    ld a, #115
    .db 0xED, 0xFE
    ret

_putchar:
    ld a, #1
    .db 0xED, 0xFE
    ret
