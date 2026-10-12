# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

Sewer Breakout is a small 2D top-down C++17 game built on Allegro 5 (code comments and UI strings are in Spanish). There are three levels; the third has a boss. There are no tests and no linter configured.

## Build and run

```
cmake -B build && cmake --build build
./build/SewerBreakout     # must be run from the repo root
```

- Assets are loaded via relative paths (`Sprites/...`, `gothic_pixel.ttf`), so the working directory must be the repo root, not `build/`.
- `CMakeLists.txt` collects sources with `file(GLOB "*.h" "*.cpp")` at the top level, so re-run `cmake` after adding or removing files. Sources live flat in the repo root.
- Requires the Allegro 5 dev libraries (core, image, primitives, font, ttf, audio, acodec, native dialog).
- `.gitignore` does not exclude `build/`; do not commit it.

## Architecture

- **`main.cpp`**: creates `Engine` and runs a loop of `al_wait_for_event` → `handleEvents` → `update` → `draw`. Rendering is gated on the timer event setting `Engine::redraw` (60 fps).
- **`Engine`**: owns Allegro setup, input handling, level setup, pause, the title/game-over/credits screens, and the shared samples. The constructor blocks on the title screen's Enter prompt before building maps, the hero and the first level. Initialization failures set `error` rather than throwing.
- **`Manager`**: singleton (`Manager::Instance()`) that owns the `gameObjectList` (all non-hero objects), the `Mapa` list and the single `Hero*` (`mainHeroe`). It runs the per-frame update, draw and collision passes. `UpdateGameObjects` deletes objects whose `toDestroy()` is true and awards 100 points for each, and it recounts enemies (`enemy` and `boss` types) each frame.
- **`GameObject`**: abstract base (position, sprite sheet, a `types` enum: player/enemy/missile/enemy_missile/item/boss). Subclasses implement `setSprites`, `draw`, `update`, `collision` and `toDestroy`. `Hero`, `Enemy`, `Boss`, `Missile`, `EnemyMissile` and `Item` derive from it.
- **Collisions** (`Manager::CheckCollissions`): O(n²) pairwise AABB checks. `boundingBox(a, b)` calls `a->collision(b)`, so each object's `collision()` decides what a hit means; it is called in both directions across the loops. The hero is not in `gameObjectList` and is checked separately.
- **`Mapa`**: each level has a visible map image plus a matching `*_COLLISION.png`. The hero samples collision-map pixels (`Hero::isBoundary`): black means a wall, and blue channel == 1 on both sampled corners means the exit and sets `nextLevel`. Edit the collision PNGs to change walls and exits. `Mapa::drawMap` applies a camera transform (scale 1.5, centered on the hero). `Manager::DrawStats` resets to an identity transform so the HUD is not scaled.
- **Level flow**: `Engine::selectLevel` clears the object list and calls `setFirstLevel`/`setSecondLevel`/`setThirdLevel`, which place the hero, enemies, items and boss. `Engine::update` advances the level only when no enemies remain and the hero has `nextLevel` set. `Hero::die` decrements lives and sets `restart`, which reloads the current level. `Hero::isDead` is `lives < 0` and sets `GameOver`; any key press on the game-over screen quits.
- **Input**: `Engine::keyPressed` and `keyReleased` set per-direction flags on the hero (WASD or arrows, `move` and `stop`). Space fires on key release, `P` toggles pause, and Enter or Escape work on the title screen.

## Gotchas

- `Manager::UpdateGameObjects` and `CheckCollissions` print debug output to `std::cout` every frame.
- `Boss` and `Enemy` positions are in screen/map pixel coordinates, not proportions. `engine.cpp` has TODOs about refactoring this.
