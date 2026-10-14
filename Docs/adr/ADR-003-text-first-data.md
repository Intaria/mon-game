# ADR-003: Text-first game data

- **Status:** Accepted
- **Date:** 2026-10-14

## Context

The game is data-heavy: species, stats ranges, type chart, moves, items, encounter
zones, weather zones, NPC dialogue, quests, recipes, achievements. This data must be
editable at scale, reviewed as diffs, and co-authored by the agent (which cannot edit
binary `.uasset` files — ADR-002).

## Decision

- **Source of truth for game data is text:** CSV/JSON files under `Data/`, with
  schemas defined by C++ structs (UE `USTRUCT`/row types).
- Binary representations inside the editor (DataTables, DataAssets) are **derived**:
  imported/reimported from the text files via editor import or a commandlet.
- Schema changes update the C++ struct and the text data in the same commit.
- IDs are stable string identifiers defined at Layer 1; never reused or repurposed.
- Referential integrity, ID uniqueness and value ranges are enforced by a scripted
  validation check, not by manual review.

## Consequences

- The agent can add/balance content in reviewable diffs; the human can still tune
  values in the editor and export back (the round-trip direction is editor → CSV
  export must be done consciously, never silently).
- We forgo free-form per-asset Blueprint data inheritance for gameplay data — accepted,
  since gameplay data is plain values + references.
- Requires a small import/validation pipeline early (part of Layer 1).
