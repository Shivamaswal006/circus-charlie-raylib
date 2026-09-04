<div align="center">

# 🎪 Circus Charlie (NES)

Made with architectural assistance from AI during my basics of C classes. I went through and heavily commented the codebase to understand the mechanics.

**A modern 16:9 widescreen remake of the 1984 arcade classic written in C using Raylib.**

[![Language](https://img.shields.io/badge/Language-C-00599C?style=for-the-badge&logo=c&logoColor=white)](#)
[![Library](https://img.shields.io/badge/Framework-Raylib-EA4335?style=for-the-badge)](#)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-555555?style=for-the-badge)](#)

<br/>

Includes fully playable **Stage 1 (Fire Rings)** and an in-progress **Stage 2 (Tightrope Monkeys)** across both **Classic Normal** and **Endless Arcade** modes.

</div>

---

## 🎮 Gameplay Features

* **Stage 1: Lion & Fire Rings** — Precision jumping through dynamic short and long fire hoops while collecting bonus money bags.
* **Stage 2: Tightrope Walking** *(WIP)* — Tightrope physics engine featuring slow brown monkeys and jumping blue monkey AI.
* **Dual Game Modes:** 
  * **Normal Mode:** Classic experience with distance countdown ($100\text{m} \rightarrow 0\text{m}$) and end-of-stage bonus drain.
  * **Endless Mode:** Procedural, infinite survival tracks for high-score chasers.
* **8-Bit Audio Pipeline:** Authentic arcade music streaming and instant sound effects powered by Raylib's audio engine.

---

## 🛠️ Technical Highlights

* **Aspect Ratio Preservation:** Internal virtual canvas rendered at $426 \times 240$ and dynamically scaled to native monitor resolution using `fmin` letterboxing/pillarboxing.
* **Object Pooling:** Obstacles (fire rings, pots, monkeys) are recycled and repositioned off-screen to avoid continuous memory reallocation.
* **Kinematic Physics:** Custom vertical velocity and gravity integration for player and enemy jump arcs (`speedY += gravity`).
* **Parallax Background:** Dual-speed horizontal scrolling layer to produce 2.5D visual depth.
* **Custom Hitboxes (AABB):** Sub-rectangles offset specifically inside transparent sprite borders to prevent unfair collision triggers.

---

## 🎮 Controls

| Action | Controls |
| :--- | :--- |
| **Move Clown / Lion** | <kbd>←</kbd> <kbd>→</kbd> or <kbd>A</kbd> <kbd>D</kbd> |
| **Jump** | <kbd>Space</kbd> or <kbd>↑</kbd> <kbd>W</kbd> |
| **Select / Confirm** | <kbd>Enter</kbd> or <kbd>Space</kbd> |
| **Exit Game** | <kbd>Esc</kbd> |

---

## 🐛 Known Issues & Roadmap

> *Stage 2 is currently in unfinished.*

- [ ] **Monkey Vertical Alignment:** Some monkeys occasionally spawn on varying Y levels on the rope.
- [ ] **Spawn Clumping:** Spawn frequency and group pacing are still in early tuning stages.
- [ ] **Blue Monkey Sprites:** Blue jumping monkeys currently lack a dedicated walking animation cycle.

---

<div align="center">
<sub>Enjoy the game! Built as an exploratory systems & understanding basics of C.</sub>
</div>

---

<div align="center">
<sub>**Disclaimer:** *This is a free, open-source, and non-commercial educational project built strictly for learning C programming and the Raylib framework. "Circus Charlie", its characters, audio, and visual assets are the intellectual property of Konami. No copyright infringement is intended.*</sub>
</div>

---
