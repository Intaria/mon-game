# ADR-001: Unreal Engine 5.8.x as the game engine

- **Status:** Accepted
- **Date:** 2026-10-14

## Context

The project is a 3D creature-collecting RPG: seamless handcrafted world, third-person
camera, turn-based battles, hundreds of species with deep data (stats, mutations,
pigmentation, evolution, encounters), mostly single-player with optional online
features (news, trade by player ID, tournaments/raids). Team: one developer + one AI
agent. Targets: PC (Windows, Linux) first, mobile later.

Candidates evaluated:

- **Unity** — balanced ecosystem, but the developer explicitly does not favor it.
- **Godot** — open and light, but its large-world/open-world streaming workflow could
  not be verified as production-ready for this scope; developer judged it not a fit.
- **Unreal Engine 5** — heaviest option, but mature 3D/world tooling, first-class C++,
  supported Linux editor workflow and Windows/Linux/mobile targets.

## Decision

Use **Unreal Engine 5.8.x** (currently 5.8.2), version pinned for the project.
Engine upgrades are a separate, explicit decision recorded as a new ADR.

Key facts driving this decision (verified 2026-10):

- Royalty: 5% of product revenue above $1M lifetime **per product** — not a factor
  for a solo project at this stage.
- Epic documents the editor workflow on Linux (matches the developer's Arch Linux
  workstation) and Windows/Linux packaging targets; Android/iOS are supported targets
  for the future mobile phase.
- C++ gives us text-based, diffable, agent-editable source — essential for the
  human+agent workflow (see ADR-002).

## Consequences

- Compilation and editor runs happen on the developer's machine; the agent prepares
  code + commands and fixes from pasted logs (AGENTS.md §6).
- Binary assets (`.uasset`/`.umap`) are opaque to the agent → text-first data policy
  (ADR-003) and presentation-only Blueprints (ADR-002) become mandatory, not optional.
- Engine install/first build is a heavy one-time setup (~50 GB+, long builds).
- Mobile support is possible later but must be kept in mind in input/architecture
  decisions now (abstract input actions, scalable UI later).
