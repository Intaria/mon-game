# ADR-004: Git + Git LFS for source control

- **Status:** Accepted
- **Date:** 2026-10-14

## Context

Epic's conventional Unreal workflow is Perforce-centric (checkout/lock for binary
assets). Git does not text-merge `.uasset`/`.umap`. Alternatives: Perforce, Git+LFS,
plain Git.

## Decision

Use **Git + Git LFS**:

- Git for all text (C++, config, CSV/JSON, docs) — the agent's entire domain is text.
- LFS for binaries (`.uasset`, `.umap`, meshes, textures, audio — patterns in
  `.gitattributes`).
- Every contributor runs `git lfs install` once per machine.
- Discipline rule instead of locking: **the human is the only editor of binary assets
  in the working session**; agent commits never touch LFS-tracked files. Conflicts in
  binaries are resolved by "take one side", never merged.

## Consequences

- Familiar GitHub-style workflow, reviewable agent diffs, simple branching.
- If the project later grows multi-person asset workloads (many parallel map/Blueprint
  edits), revisit — Perforce or LFS locking may then beat discipline.
