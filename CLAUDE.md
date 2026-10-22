# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

Sewer Breakout is a small 2D top-down C++17 game built on Allegro 5 (code comments and UI strings are in Spanish). There are three levels; the third has a boss. There are no tests; clang-format and clang-tidy are configured (see below).

## Build and run

```
cmake --preset debug && cmake --build build
./build/SewerBreakout     # must be run from the repo root
```

- The root `Makefile` wraps these: `make` (configures on first use, then builds), `make run`, `make configure` (re-run CMake), `make clean`. This is what `:make` in Neovim uses.
- The `debug` preset (Ninja) writes `build/compile_commands.json`, which `.clangd` points at for editor support.
- Format with `clang-format -i <files>` (`.clang-format`) and lint with `clang-tidy -p build <files>` (`.clang-tidy`). The existing code has not been bulk-reformatted, so run the formatter only on files you are touching.

- Assets are loaded via relative paths (`Sprites/...`, `gothic_pixel.ttf`), so the working directory must be the repo root, not `build/`.
- `CMakeLists.txt` collects sources with `file(GLOB "*.h" "*.cpp")` at the top level, so re-run `cmake` after adding or removing files. Sources live flat in the repo root.
- Requires the Allegro 5 dev libraries (core, image, primitives, font, ttf, audio, acodec, native dialog).

## Architecture

- **`main.cpp`**: creates `Engine` and runs a loop of `al_wait_for_event` → `handleEvents` → `update` → `draw`. Game logic runs only on timer events (60 fps), and `draw` renders only when the timer set `Engine::redraw` and the event queue is empty.
- **`Engine`**: owns Allegro setup, input handling, level setup, the screens, and the shared samples. It is a state machine over `GameState` (`Title`, `Playing`, `Paused`, `GameOver`, `Credits`): `keyPressed`/`keyReleased`, `update` and `draw` each switch on `state`. The constructor loads everything and starts in `Title` without blocking. Initialization failures go through `Engine::fail`, which shows a native message box, sets `error` and stops the game; the destructor frees whatever was created. Credits advance on timer ticks and then stop the game.
- **`Manager`**: singleton (`Manager::Instance()`) that owns the `gameObjectList` (all non-hero objects), the `Mapa` list and the single `Hero*` (`mainHeroe`). It runs the per-frame update, draw and collision passes. `UpdateGameObjects` deletes objects whose `toDestroy()` is true and awards 100 points for each, and it recounts enemies (`enemy` and `boss` types) each frame.
- **`GameObject`**: abstract base (position, sprite sheet, a `types` enum: player/enemy/missile/enemy_missile/item/boss). Subclasses implement `setSprites`, `draw`, `update`, `collision` and `toDestroy`. `Hero`, `Enemy`, `Boss`, `Missile`, `EnemyMissile` and `Item` derive from it.
- **Collisions** (`Manager::CheckCollissions`): O(n²) pairwise AABB checks. `boundingBox(a, b)` calls `a->collision(b)`, so each object's `collision()` decides what a hit means; it is called in both directions across the loops. The hero is not in `gameObjectList` and is checked separately.
- **`Mapa`**: each level has a visible map image plus a matching `*_COLLISION.png`. The hero samples collision-map pixels (`Hero::isBoundary`): black means a wall, and blue channel == 1 on both sampled corners means the exit and sets `nextLevel`. Edit the collision PNGs to change walls and exits. `Mapa::drawMap` applies a camera transform (scale 1.5, centered on the hero). `Manager::DrawStats` resets to an identity transform so the HUD is not scaled.
- **Levels** are data files: `levels/level1.txt`, `level2.txt`, … are loaded at startup until a number is missing (`level.h`/`level.cpp`, `loadLevel`), each with one `Mapa`. One entity per line, `#` comments: `map <image> <collision image>`, `hero x y`, `enemy x y w h hits speed distance LEFT|RIGHT X|Y`, `item x y w h SHOVEL|POTION|GEM|LAMP`, `boss x y w h hits speed distance LEFT|RIGHT`. A bad file shows an error box with `file:line`. To add a level, add the two PNGs and the next `levels/levelN.txt`; no code change.
- **Level flow**: `Engine::selectLevel` deletes the current objects (`Manager::clearGameObjectList`) and spawns the level's data. `Engine::updatePlaying` advances the level only when no enemies remain and the hero has `nextLevel` set, up to the last level file. `Hero::die` decrements lives and sets `restart`, which reloads the current level. `Hero::isDead` is `lives < 0` and switches to `GameOver`; any key press there goes to the credits.
- **Input**: `Engine::keyPressed` and `keyReleased` set per-direction flags on the hero (WASD or arrows, `move` and `stop`). Space fires on key release, `P` toggles pause, Enter starts from the title, and Escape on the title goes to the credits. Closing the window quits from any state.

## Gotchas

- `Manager::UpdateGameObjects` and `CheckCollissions` print debug output to `std::cout` every frame.
- `Boss` and `Enemy` positions are in screen/map pixel coordinates, not proportions. `engine.cpp` has TODOs about refactoring this.
- Keep `al_init_native_dialog_addon()` right after `al_init()`: without it, error boxes parented to the display segfault inside GTK on Wayland.
