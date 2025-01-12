# King Of The Grid

*A Z80 Sandbox Competition Game* in which developers can write
bots to compete on the most efficient algorithm to be King Of The Grid!

<div align="center">

https://github.com/user-attachments/assets/10258c56-2554-46ed-8e49-46176c138275
  
  Example of a single competition.
</div>

# Game Rules

* See [rules.cpp](src/rules.cpp) for exact rules (how often, what values etc).
* World size: 32x32 cells
* Two bots (or programs) spawn in top-left and bottom-right corners
* Each bot has a set of energy. See [rules.cpp](src/rules.cpp) for exact values.
* Bots run on Z80 CPU. See CPUs section.
* Food (static) and prey (moves) are spawned periodically.
* Food is not spawned evenly: random noise is applied using [FastNoiseLite](https://github.com/Auburn/FastNoiseLite) on rules described on [seasons.cpp](src/seasons.cpp)
* Both can move around, eat, scan environment and clone themselves. See [bot_api.h](bot-api/bot_api.h)
* Bot that survives the longest, wins.
* If both bots survive for too long, it's a draw.

# Live demo

See [kingofthegrid.com](https://kingofthegrid.com/run.html)

# Easiest: build and run web version

Web version can be built with [Emscripten framework](https://emscripten.org/docs/getting_started/downloads.html).

```bash
make kotg-web
cd bin
python3 -m http.server 8000
# Open http://localhost:8000/kotg.html in browser
```

# How to build the game engine in command line

* Have `cmake` and `gcc` installed.
* `git submodule update --init --recursive`
* Do `make`
* To test an [example bot](./examples/test-bot-z88dk) do `make testbot`. This example
  runs the bot against itself.

# How to run game engine

## Single session:

Go into folder with two (or one) of your programs, they must have `.bin` extension.

Run: `kotg <program1> [<program2>] [<seed>]`.

### Watching the results

Game outputs files `recording-xxx.txt`, which you can play with [asciinema](https://asciinema.org/):

```bash
asciinema play recording-xxx.txt
# or, to share the game on the web
asciinema upload recording-xxx.txt
```

Export `KOTG_AUTOPLAY=1` to play results automatically.
Export `KOTG_AUTOUPLOAD=1` to upload them automatically with a review URL.

## Mass session:

Have all your programs (2+) placed into current folder.

Run `kotg`

In this mode, each pair of programs will play against one another twice on the same seed.
That way both bots have a chance.

This process is deterministic too. You can specify `MASS_SEED` environment variable with seed
from previous play out and have the process repeated exactly for debugging purposes.

Any game session can be replicated. Given a session outcome AAA BBB 1338, you can just pass these arguments to kotg
and play the game again.

## What is `seed`

Every game is deterministic: given two programs and a seed, it will play out exactly the same.
That way, when you loose in a mass session, you can replicate the session with your target in order to 
adjust to your opponent's behavior.

# CPUs

* Each bot runs on a Z80-equipped virtual machine, with limited Z80-ticks per game tick. So if your bot thinks a lot,
  it is going to be outrun.
* Bots can perform action according to API. See [bot_api.h](bot-api/bot_api.h)
* `printf` is supported for debugging.
* Each action consumes energy.
* When bot decides to take action, execution stops until action is done.
* Program address starts at address 0, and program can be of size of up to 65536
* Last 8192 (0xE000 - 0xFFFF) of memory are potentially SHARED across all bots, if enabled. 
  That way they can communicate.
* Bots can clone (or fork) themselves in order to spread, fork-bomb style, or hibernate to preserve energy.

# Shared Memory

By default, bots do not have shared memory between themselves, e.g. every instance has 0x0000 - 0xFFFF memory all
to itself. If bot calls `bot_enable_shared_memory()`, last 8192 bytes (0xE000 - 0xFFFF) of its memory becomes sharable. 

1. Any write to addresses 0xE000 - 0xFFFF will be stored for all bots that have enabled shared memory. 
2. Any read will return shareable memory – potentially written by other instances of a bot.

Keep in mind, that

1. When shared memory is enabled, "local" memory region 0xE000 - 0xFFFF is lost, therefore shared memory shall
   be enabled as soon as possible.
2. If your stack pointer (SP) is located in that region – which is typically default – you will see unexpected
   issues, therefore take care to set SP to 0xE000 on init. On z88dk, this is done using
   "-pragma-define=REGISTER_SP=57344", on sdcc you will need to have a custom crt0. (which sdcc-backend does).
3. By default, shared memory is initialized with zeros.

<table>
<tr>
<th>Memory Region</th>
<th>Description, when shared memory is enabled.</th>
</tr>
<tr>
<td>0x0000 - 0xDFFF</td>
<td>Program memory (including stack)</td>
<tr>
<td>0xE000 - 0xFFFF</td>
<td>Shared memory across all friendly bots
</tr>
</table>

# How to write your Bot

## SDCC

* Install sdcc (Mac: `brew install sdcc` Ubuntu: `sudo apt install sdcc` Windows: [here](https://sdcc.sourceforge.net/snap.php#Windows))
* Include `bot_api.h` and `sdcc-backend.asm` from [bot-api](bot-api) folder in your project.
* See [example bot](./examples/test-bot-sdcc) for an example on how to build with SDCC.
* Do `make`, or run the following script:

  ```bash
  sdasz80 -o sdcc-backend.rel sdcc-backend.asm
  sdcc -mz80 --no-std-crt0 test_bot.c sdcc-backend.rel -o test-bot-sdcc.ihx
  objcopy --input-target=ihex --output-target=binary test-bot-sdcc.ihx test-bot-sdcc.bin
  ```

## Z88DK

* [Download z88dk](https://github.com/z88dk/z88dk/releases) or install z88dk from sources:

  ```bash
  # dependencies on Ubuntu
  sudo apt install -y cmake git build-essential libxml2 libxml2-dev m4 perl                                                                           
  
  # dependencies on Mac
  brew install gmp libxml2 pkgconf
  # add the following to your bashrc
  # export CFLAGS="-I/opt/homebrew/include"
  # export LDFLAGS="-L/opt/homebrew/lib"
  
  git clone --recursive https://github.com/z88dk/z88dk.git
  cd z88dk
  # takes a while (10+m)
  ./build.sh -p zx
  ./build.sh -k -p test
  sudo make install
  ```
  
* Include `bot_api.h` and `z88dk-backend.asm` from [bot-api](bot-api) folder in your project.
* See [example bot](./examples/test-bot-z88dk) for an example on how to build with C and z88dk.
  It uses CMake, but you can build with just zcc: `zcc +test z88dk-backend.asm <program>.c -o mybot.bin`
* If you prefer assembly, see section below.

### How to write your bot without CMake

Have the following `example.c` and copy `bot_api.h` and `z88dk-backend.asm` into your folder.

```c
#include "bot_api.h"
#include <stdio.h>
int main()
{
    bot_move_right();
    bot_move_left();
}
```

Then simply do:
```bash
zcc +test z88dk-backend.asm test_bot.c -o example.bin
```

# I don't like z88dk or C for that matter

* Make sure your code has base address (`.ORG`) of 0.
* For communication, see below.
* Other than that, your bot can do whatever!

See [z88dk-backend.asm](bot-api/z88dk-backend.asm) on how bot interacts with the world.

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
