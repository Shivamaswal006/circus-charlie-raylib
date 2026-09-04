
==================================================
           CIRCUS CHARLIE (NES INSPIRED)
==================================================

A modern 16:9 widescreen remake of the 1984 arcade classic **Circus Charlie**, written in **C** using the **Raylib** library. 

Includes fully playable Stage 1 (Fire Rings) and Stage 2 (Tightrope Monkeys) (unfinished) with both Normal and Endless arcade modes.

---

## 🎮 Gameplay Features
- **Stage 1: Lion & Fire Rings** — Precision jumping through short and long fire hoops with bonus bags.
- **Stage 2: Tightrope Walking** — Tightrope physics featuring fast-moving Blue Monkeys with jump AI and slow Brown Monkeys.
- **Game Modes:** Classic Normal Mode with distance countdown & bonus tally, plus Endless Survival modes.
- **Audio & SFX:** Full 8-bit arcade soundtrack and sound effects using Raylib's audio streaming pipeline.

---

## 🛠️ Technical Highlights
- **Aspect Ratio Preservation:** Internal virtual canvas rendered at $426 \times 240$ and dynamically scaled to native monitor resolution with pillarboxing.
- **Object Pooling:** Obstacles (rings, pots, monkeys) are recycled and randomized off-screen to avoid continuous memory reallocation.
- **Kinematic Physics:** Custom gravity and vertical velocity simulation for player and AI jumping arcs.
- **Parallax Background:** Dual-speed scrolling engine to simulate 2.5D visual depth.
- **Tight Hitboxes:** Custom Axis-Aligned Bounding Boxes (AABB) configured specifically to ignore sprite transparency.

---

Known Issues:
In Stage 2:
- Few Monkeys comes floating (different y level than others) with hitboxes.
- Monkey spawning frequency and grouping control is still in initial stages.
- Blue Monkey doesn't have any walking animation.

--------------------------------------------------
           CONTROLS
--------------------------------------------------
  * Left / Right Arrow: Move Clown/Lion
  * Spacebar or Up Arrow:   Jump
  * Enter or Spacebar: Select / Start
  * Escape: Exit Game

Enjoy the game!
