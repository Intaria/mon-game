# ADR-002: Gameplay logic in C++, Blueprints for presentation only

- **Status:** Accepted
- **Date:** 2026-10-14

## Context

The team is a human + an AI coding agent. The agent edits text files; Unreal
Blueprints (`.uasset`) are binary graphs with no reliable text representation or
text-merge path (Epic's own tooling documents a purpose-built visual diff instead of
textual diff/merge). If gameplay logic lives in Blueprints, the agent becomes blind
to half the codebase, code review breaks, and uniformity across sessions — the
project's stated priority — is impossible.

## Decision

- **All gameplay rules, state transitions, data validation, and reusable systems live
  in C++.**
- **Blueprints are allowed only for presentation and thin integration:** UI layout and
  event binding, animation state hooks, VFX triggers, level-specific visual wiring.
- Anything that must cross the C++↔Blueprint boundary is exposed deliberately
  (`UPROPERTY`/`UFUNCTION` with explicit categories), never "just in case".
- The agent never edits `.uasset`/`.umap`; such tasks are re-planned or handed to the
  human in the editor.

## Consequences

- Slightly more C++ boilerplate up front (custom widget classes instead of pure
  widget Blueprints), bought with full agent visibility into the whole gameplay code.
- Level design and art still use the editor freely — they are human-owned domains.
- Every session can be reviewed as a text diff, in full.
