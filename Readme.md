# King Of The Grid

*A Z80 Sandbox Competition Game* in which developers can write
bots to compete on the most efficient algorithm to be King Of The Grid!

![img.png](doc/img.png)

# Game Rules

* World size: 32x32 cells
* Two bots (or programs) spawn in top-left and bottom-right corners
* Each bot has a set of energy. See [defines.h](include/defines.h) for exact values.
* Bots run on Z80 CPU. See CPUs section.
* Food (static) and prey (moves) are spawned periodically.
* Both can move around, eat, scan environment and clone themselves. See [bot_api.h](bot-api/bot_api.h)
* Bot that survives the longest, wins.
* If both bots survive for too long, it's a draw.

# How to build

* Install z88dk:
  ```bash
  git clone https://github.com/z88dk/z88dk.git
  cd z88dk
  git submodule update --init --recursive
  ./build.sh -p test
  sudo make install
  ```

* Have `cmake` and `gcc` installed.
* `git submodule update --init --recursive`
* Do `make`
* To test an [example bot](./examples/test-bot) do `make test-bot`. This example
  runs the bot against itself.

# How to run

## SDL single session:

Go into folder with two (or one) of your programs, they must have `.bin` extantion.

Run: `kotg_sdl2 <program1> <program2> <seed>`.

## Mass session:

Have all your programs (2+) placed into current folder.

Run `kotg`

In this mode, each pair of programs will play against one another twice on the same seed.
That way both bots have a chance.

## What is `seed`

Every game is deterministic: given two programs and a seed, it will play out exactly the same.
That way, when you loose in a mass session, you can replicate the session with your target in order to 
adjust to your opponent's behavior.

# CPUs

* Each bot runs on a Z80-equipped virtual machine.
* Bots can perform action according to API. See [bot_api.h](bot-api/bot_api.h)
* `printf` is supported for debugging.
* Each action consumes energy.
* When bot decides to take action, execution stops until action is done.
* Program address starts at address 8192, and program can be of size of up to 65536-8192=57344
* First 8192 (0x0000 - 0x1FFF) of memory are SHARED across all bots. That way they can communicate.
* Bots can clone (or fork) themselves in order to spread, or hibernate to preserve energy.

<table>
<tr>
<th>Memory Region</th>
<th>Description</th>
</tr>
<tr>
<td>0x0000 - 0x1FFF</td>
<td>Shar
<tr>
<td>0x2000 - 0xFFFF</td>
<td>Program memory</td>
</tr>
</table>

# I don't like z88dk or C for that matter

See [bot_api.c](bot-api/bot_api.c) on how bot interacts with the world.

To call a system call, load call number into register a `A` and them do

```asm
defb $ED, $FE
```

This is an illegal z80 instruction that emulator will understand and stop exection to process
your input. Both input (like energy) and output are taken/returned into argument `HL`.

See line 10 for exact syscall codes.

## stdout

To print a character, do syscall `1`:

```asm
ld a, 1
ld l, 'H'
defb $ED, $FE
```

# How to debug your bot

* `printf`s are supported
* GDB support can be added if project kicks off.
