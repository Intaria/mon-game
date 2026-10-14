# AGENTS.md — Project Conventions

> This file is the single source of truth for **how** we work: code style, policies,
> workflow, verification. **What** we build is specified in [`Docs/GDD/`](Docs/GDD/).
> Decisions and their rationale live in [`Docs/adr/`](Docs/adr/).
> The ordered plan lives in [`Docs/ROADMAP.md`](Docs/ROADMAP.md).
>
> Read this file at the start of every session. If a rule here conflicts with a habit
> or a local convenience — this file wins; propose a change to it explicitly instead of
> silently deviating.

---

## 1. Project

- **Genre:** 3D creature-collecting RPG, Pokémon-inspired (see `Docs/GDD/`).
- **Engine:** Unreal Engine **5.8.x**, version pinned. Engine upgrades are an explicit,
  discussed decision (see ADR-001) — never a side effect of other work.
- **Team:** one human developer + AI coding agent. This is why uniformity matters more
  than velocity: every session must leave the repo in a state the next session
  (human or agent) can pick up without re-deriving context.
- **Target platforms:** PC first (Windows, Linux), mobile later. Mobile is a
  *future* constraint — respect it in input/architecture decisions, do not build for it now.
- **Working project name:** `mon-game` (repository name). UE identifiers use
  PascalCase `MonGame` (`MonGame.uproject`, module `MonGame`) because hyphens are
  invalid in UBT/C++ identifiers. A final marketing name may replace it later — that
  is an explicit rename task, never a silent change.

## 2. Hard policies

These are not style preferences; breaking them breaks the workflow.

1. **All gameplay logic in C++.** Blueprints are for presentation and thin wiring only
   (UI event binding, animation hooks, VFX triggers). No gameplay rules, state
   transitions, or data validation in Blueprint graphs. Rationale: ADR-002.
2. **Text-first data.** The source of truth for game data (species, moves, items, zones,
   quests…) is text — CSV/JSON under `Data/` — with schemas defined by C++ structs.
   Binary assets are generated/imported from it via the editor or a commandlet.
   Rationale: ADR-003.
3. **The agent never edits binary assets.** No hand-editing `.uasset` / `.umap` / media
   files. If a task seems to require it, stop and re-plan: the answer is a C++ class,
   a text data file, or a task for the human in the editor.
4. **One layer / one feature per branch.** No drive-by refactors in the same diff as a
   feature. Keep diffs small and reviewable.
5. **No new third-party plugins or dependencies** without an approved ADR.
6. **The repo must stay consistent at every commit.** No "will fix later" states on the
   branch tip.

## 3. Repository layout

```
/.uproject, /.uplugin        UE project descriptors (tracked)
/AGENTS.md                    this file
/.gitignore, /.gitattributes  repo hygiene; LFS patterns for binary assets
/Docs/
  /GDD/                       normalized game design (Russian), one file per system
  /adr/                       architecture decision records
  /design/                    visual/UI design agreements (created at the design layer)
  ROADMAP.md                  ordered layers and their status
/Source/                      C++ modules (one module per game layer domain initially)
/Config/                      Default*.ini (tracked)
/Data/                        text-first game data: CSV/JSON, schemas in C++ (ADR-003)
/Content/                     UE assets (LFS) — human-authored, agent does not edit
/Plugins/                     UE plugins (empty unless an ADR adds one)
/Saved/, /Intermediate/, /Binaries/, /DerivedDataCache/   — gitignored, never commit
```

## 4. Code conventions

### Naming (UE standard, non-negotiable)

| Kind | Convention | Example |
|---|---|---|
| Classes | `U`/`A`/`F`/`S`/`E`/`I` prefix + PascalCase | `UPokemonSpeciesData` |
| Methods / functions | PascalCase | `CalculateDamage()` |
| Members | PascalCase, `b` prefix for bools | `bIsAwake` |
| Parameters / locals | camelCase | `targetLevel` |
| Constants | `kPascalCase` or `UE`-idiomatic `MAX_*` | `kMaxPartySize` |
| File name | = primary class name, one primary class per file | `PokemonSpeciesData.h` |
| Identifiers in code | English | — |

- No abbreviations beyond well-established ones (`HP`, `ID`, `NPC`, `UI`, `XP`).
- Gameplay identifiers (IDs, tags) are **stable data contracts**: rename them only with a
  data migration plan (Layer 1 defines the ID scheme).

### Structure & style

- Headers: forward-declare where possible; include what you use; no include of
  unrelated headers "just in case".
- Expose anything to the reflection system (`UPROPERTY`/`UFUNCTION`) deliberately, with
  an explicit `Category` and a comment on *why* it crosses the C++↔Blueprint boundary
  (presentation hooks only — see Policy 1).
- Prefer UE subsystems (`UGameInstanceSubsystem`, `UWorldSubsystem`) over singletons for
  system access.
- Pure logic (formulas, rules, data transforms) lives in plain, engine-independent
  classes so it can be unit-tested without an engine world.
- `const`-correct; no raw owning pointers for UObject lifetimes (use `UPROPERTY` for
  anything GC-visible).
- Comments explain **why**, not what. English. No commented-out code left in commits.
- `TODO(<area>): description` format for known gaps; unexplained `TODO` is a review fail.

### Errors & logging

- Invariants: `check()` / `ensure()` (never silent recovery from impossible states).
- Expected failures: return `TOptional`/`E…Result` or error out-params; no exceptions.
  Never swallow an error.
- One logging category per system: `DECLARE_LOG_CATEGORY_EXTERN(LogCreatures, …)`.

## 5. Data conventions (specified in [`Docs/data/schema.md`](Docs/data/schema.md))

- **`Docs/data/schema.md` is the single source of truth for data structure** (ER diagrams,
  table spec, approved decisions). Any change to `Data/`, C++ row structs, or the
  save-game schema **must** update that document in the **same commit** — a schema
  change without the diagram/spec update is an incomplete commit.
- Every data record has a stable string ID (namespaced slug, e.g. `species.bulbasaur`);
  IDs are never reused or repurposed.
- CSV/JSON in `Data/` mirrors a C++ `USTRUCT` row type — schema changes touch **both**
  in one commit.
- Data files are reviewable text: no exported-from-editor blobs, no binary formatting.
- Validation (referential integrity, ID uniqueness, ranges) is a scripted check, not a
  manual habit — added in Layer 1.

## 6. Testing & verification

- **Unit tests** for pure logic (damage formulas, type chart, inheritance rolls,
  inventory rules) use UE Automation tests, placed under
  `Source/<Module>/Private/Tests/`.
- Test command (run on the human's machine, Linux):

  ```bash
  UnrealEditor-Cmd <Project>.uproject \
    -ExecCmds="Automation RunTests <Project>; Quit" \
    -unattended -nopause -nullrhi -log
  ```

- **Build/compile happens on the human's machine** (UE 5.8, Arch Linux). The agent
  prepares code and exact commands; the human pastes logs back and the agent fixes
  from the log. Never claim "it works" without a pasted build/test result.
- **Definition of Done** for every task:
  1. builds without new warnings,
  2. relevant tests pass,
  3. docs/GDD updated if behavior or schema changed,
  4. this AGENTS.md still describes reality,
  5. `Docs/data/schema.md` updated if data structure changed (ER + spec).

## 7. Workflow

- Each work session starts with: read AGENTS.md → read ROADMAP.md → agree on the
  session's single goal → update the todo list → implement → verify → commit.
- **Branches:** `feat/<layer>-<short-name>`, `fix/<area>-<short-name>`, `docs/<topic>`.
- **Commit messages:** Conventional Commits in English, why-focused:

  ```
  feat(movement): add click-to-move path request without auto-jump
  docs(gdd): normalize battle rules into per-system files
  chore(repo): set up UE gitignore and LFS tracking
  ```

- Never commit without a green local check (compile where applicable, tests where they
  exist). Never force-push shared history. Never commit secrets or machine-local paths.
- The agent maintains the session todo list; after each completed step the list is
  updated before moving on.

## 8. Language

- **Product docs** (`Docs/GDD`, `Docs/design`): Russian (source language of the design).
- **Code, identifiers, comments, commit messages, this file**: English.
- ADRs: English (they are technical), with the decision summary restated in Russian in
  the PR/commit if useful.

## 9. Quick reference

| I need… | Read |
|---|---|
| What to build next | `Docs/ROADMAP.md` |
| Game rules / feature spec | `Docs/GDD/` |
| Why the architecture is this way | `Docs/adr/` |
| UI/style/hotkeys agreement | `Docs/design/` (later) |
| Open design questions (WIP) | `Docs/GDD/99-open-questions.md` |
