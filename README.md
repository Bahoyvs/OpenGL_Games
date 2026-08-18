# OpenGL_Games

Three real-time 2D games written in C against the fixed-function OpenGL pipeline — no engine, no math library, no sprite loader: every polygon, transform, particle and vector operation is hand-rolled.

---

## Tech Stack

| Layer | Technology |
| --- | --- |
| **Language** | C (compiled as C++ — ~2,460 LOC across three projects) |
| **Graphics API** | OpenGL fixed-function pipeline — immediate mode (`glBegin`/`glEnd`), matrix stack transforms |
| **Windowing / Input** | FreeGLUT — window, mouse, keyboard, `glutTimerFunc` main loop |
| **Math** | Custom vector library (`vec.h` / `vec.cpp`) — no GLM, no external dependency |
| **Memory** | Manual `malloc` / `free` particle buffers |
| **Toolchain** | Visual Studio (MSVC, `.sln` / `.vcxproj`) |

Written for **CTIS 164 — Technical Mathematics with Programming** (Bilkent University).

---

## Core Mechanics & Features

### Homework 1 — Thermal Gunsight
- Switches the entire render between **normal and thermal camera modes** via arrow keys, re-mapping the palette of every drawn primitive rather than post-processing a buffer.
- Destroys tanks on mouse click, with an aiming cursor that **snaps back into bounds** when the mouse leaves the designated firing area and **locks onto** a target within range.

![Homework 1](https://github.com/Bahoyvs/OpenGL_Games/assets/65951986/c9ea0343-df1c-4094-9c38-4a04ebcc4710)

### Homework 2 — Bombing Run
- Computes **bomb drop timing from the distance between plane and target**, so the player leads the shot rather than dropping on contact.
- Tracks kill count and base health with a fail state at zero, constrains the target cursor so it can never be placed on the friendly base, and despawns tanks the instant they breach it.
- Renders a multi-stage explosion effect.

![Homework 2](https://github.com/Bahoyvs/OpenGL_Games/assets/65951986/fa6225c5-5bbf-4634-832c-f976080c7405)

### Homework 3 — Missile Lock
- Runs a three-state **lock-on finite state machine** (`IDLE → STARTLOCKING → LOCKED`) with an on-screen countdown; firing is gated until lock completes.
- Hands control of the launcher from the mouse to the **tracked aircraft** once locked — the movable section follows the plane, not the cursor.
- Emits an optimised smoke trail from a manually managed particle buffer, with per-particle alpha and radius decay.

![Homework 3](https://github.com/Bahoyvs/OpenGL_Games/assets/65951986/ac593c8d-695d-4975-a866-2abd6051fcb4)

---

## Technical Architecture & Problem Solving

**The whole point of these projects is the absence of abstraction.** There is no scene graph, no sprite batcher, no physics library and no `glm::vec2` — the fixed-function pipeline gives you a matrix stack and a triangle, and everything above that is yours to build. Geometry is emitted per frame in immediate mode, with `glPushMatrix`/`glTranslatef`/`glRotatef`/`glScalef` nesting used as the composition mechanism: a launcher is a hierarchy of transformed primitives, and turret rotation is `atan2` on the vector to the mouse, applied as a rotation inside the parent's matrix.

**Vector math is implemented from scratch** in `vec.cpp`: magnitude, unit vectors, addition/subtraction/scaling, dot product, angle between two vectors, and — the part that carries most of the gameplay — **polar↔rectangular conversion** (`pol2rec` / `rec2pol`). Movement, aiming, lead calculation and lock cones are all expressed in polar form and converted back for rendering, which is far cleaner than working in components and is the reason the missile can be steered by angle while being drawn by position.

**The hardest single problem was the smoke trail, and it is a memory-management problem, not a graphics one.** The trail is an unbounded stream of particles emitted along a moving rocket, each with its own position, radius and alpha, and it must be cheap enough to run alongside everything else at 60 Hz. The implementation owns its buffer explicitly: `Point* smokePoints` is grown by allocating a new block with `malloc`, copying the existing particles across, appending the new one, and `free`-ing the old pointer — a hand-written dynamic array with no library behind it. Just as important is the *deallocation* strategy: on transition into the `LOCKED` state the entire trail is released in one `free()` and the count reset, because the particles are about to be visually irrelevant and keeping them alive costs both draw calls and memory. That is the optimisation the header comment refers to — the cheapest particle is the one you stop owning.

That grow-by-one strategy is O(n) per emitted particle and would be the first thing to replace with capacity-doubling or a fixed ring buffer in a production renderer; at the emission rates these games run at, the explicit copy is a deliberate and legible trade. **Frame pacing** is a `glutTimerFunc` on a 16 ms period (~60 FPS), with simulation and rendering driven from the same callback.

Each project's source header additionally documents its **known defects** alongside its features — for Homework 3, that the launcher stops tracking if the mouse goes completely still during lock acquisition, because lock progression is driven by the mouse-motion callback rather than the timer. Diagnosing and stating that honestly, rather than hiding it, is part of the record.

---

## Installation / How to Play

**Requirements:** Windows, Visual Studio (MSVC), and **FreeGLUT** installed and visible to the compiler/linker.

```bash
git clone https://github.com/Bahoyvs/OpenGL_Games.git
```

1. Open the `.sln` of the project you want to run:
   - `CTIS164_Homework1_Sols/CTIS164_Homework1_Sols.sln`
   - `CTIS164_Homework2_Sols/CTIS164_Homework2_Sols.sln`
   - `CTIS164_Homework3_Sols/CTIS164_Homework3_Sols.sln`
2. Ensure FreeGLUT's headers (`GL/glut.h`), `.lib` and `.dll` are on the include/library/runtime paths.
3. Build and run the **Debug** configuration.

### Controls

| Project | Input | Effect |
| --- | --- | --- |
| Homework 1 | Arrow keys | Toggle thermal / normal camera |
| Homework 1–2 | Mouse move | Aim (cursor is clamped to the valid firing area) |
| Homework 1–2 | Left click | Fire / drop bomb |
| Homework 3 | Mouse move | Aim launcher; hold over the plane to begin lock |
| Homework 3 | Left click | Fire (only permitted once `LOCKED`) |

> **Note:** all code is written in C despite the `.cpp` file extensions.

---

## Repository Layout

```
CTIS164_Homework1_Sols/CTIS164_Homework1/Yavas_IlhanBahadir.cpp     718 LOC — thermal camera, cursor lock
CTIS164_Homework2_Sols/CTIS164_Homework2/YavasIlhanBahadir.cpp      808 LOC — distance-based bombing, base health
CTIS164_Homework3_Sols/CTIS164_Homework3/
  Yavas_Ilhan_Bahadir.cpp                                           862 LOC — lock FSM, particle smoke, explosion
  vec.{h,cpp}                                                        70 LOC — vector / polar math library
```
