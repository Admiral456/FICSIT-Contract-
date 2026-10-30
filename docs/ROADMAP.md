# Roadmap

## 0.1.x — Contract loop and source hardening

- [x] Portable contract engine.
- [x] Three-offer board and reputation-gated catalog.
- [x] Accept up to three active contracts.
- [x] Partial delivery and exact completion/reward logic in the engine.
- [x] Timed expiry and capped history.
- [x] SML C++ world subsystem, save interface and replicated board source.
- [x] `/contracts`, `/contracts accept`, `/contracts deliver`, `/contracts history` source.
- [x] Inventory read-back guard so progress is only credited for observed removals.
- [x] Portable CMake tests and GitHub Actions CI.
- [ ] Compile with target custom UE 5.6.1 and SML 3.12.x.
- [ ] Verify item descriptor class paths in the target asset registry.
- [ ] In-game save/load and multiplayer test.

## 0.2.0 — Logistics

- [ ] FICSIT Dispatch Terminal buildable.
- [ ] Belt input and server-side item acceptance.
- [ ] Inventory/transfer transaction that cannot duplicate or lose items on failure.
- [ ] Dispatch animation, model, material and sound assets.
- [ ] Alpakit/Game Feature packaging for content assets.

## 0.3.0 — Progression

- [ ] Reputation unlocks and progression rewards.
- [ ] Cosmetic factory decorations and FICSIT ranks.
- [ ] More contract families, repeatable chains and balancing configuration.
- [ ] Contract generation based on researched tiers / unlock state rather than reputation alone.

## 0.4.0 — Expanded gameplay

- [ ] Long-term multi-stage contracts and special challenges.
- [ ] Contract board graphical screen and improved history.
- [ ] Dedicated-server validation and broader multiplayer tests.
- [ ] Mod compatibility review and performance profiling.

## 1.0.0 — Release

- [ ] Full feature acceptance checklist.
- [ ] Stable save format and migration tests.
- [ ] Windows and Linux dedicated-server verification where supported.
- [ ] Alpakit release package, install instructions and user-facing documentation.
- [ ] Release notes, known issues and versioned compatibility declaration.
