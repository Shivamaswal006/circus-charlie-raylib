
==================================================
           CIRCUS CHARLIE (NES INSPIRED)
==================================================

Developed in C using Raylib.

Known Issues:
In Stage 2:
- Few Monkeys comes floating (different y level than others) with hitboxes.
- Monkey spawning frequency and grouping control is still in initial stages.
- Blue Monkey doesn't have any walking animation.

For Windows - simply execute the shortcut.
For Linux - Below.

--------------------------------------------------
1. PREREQUISITES (INSTALL RAYLIB ON LINUX) (Automatically done by running ./build.sh)
--------------------------------------------------
Before compiling, ensure you have GCC and Raylib installed.

On Ubuntu / Debian / Linux Mint / Pop!_OS:
  sudo apt update
  sudo apt install libraylib-dev gcc

On Arch Linux / Manjaro:
  sudo pacman -S raylib gcc

On Fedora:
  sudo dnf install raylib-devel gcc

--------------------------------------------------
2. HOW TO BUILD & RUN
--------------------------------------------------
Open your terminal inside this src folder and run:

  ./build.sh

Then start the game with:
  ./main

--------------------------------------------------
3. CONTROLS
--------------------------------------------------
  * Left / Right Arrow: Move Clown/Lion
  * Spacebar or Up Arrow:   Jump
  * Enter or Spacebar: Select / Start
  * Escape: Exit Game

Enjoy the game!
