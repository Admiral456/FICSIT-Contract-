# Roadmap

## 0.1.x — Contract loop and source hardening

- [x] Portable contract engine.
- [x] Three-offer board and reputation-gated catalog.
- [x] Accept up to three active contracts.
- [x] Partial delivery and exact completion/reward logic in the engine.
- [x] Timed expiry and capped history.
- [x] SML C++ world subsystem, save interface and replicated board source.
- [x] `/contracts`, `/contracts accept`, `/contracts deliver`, `/contracts cancel`, `/contracts history` source.
- [x] Penalized cancellation with persisted history status and reputation-floor protection.
- [x] Three-stage Factory Backbone Program with sequential delivery gating and saved stage progress.
- [x] Inventory read-back guard so progress is only credited for observed removals.
- [x] Portable CMake tests and GitHub Actions CI.
- [ ] Compile with target custom UE 5.6.1 and SML 3.12.x.
- [ ] Verify item descriptor class paths in the target asset registry.
- [ ] In-game save/load and multiplayer test.

## 0.2.0 — Logistics

- [x] Native FICSIT Dispatch Terminal actor with Satisfactory use interaction.
- [x] Native terminal board viewer for offers, active orders, reputation and history, with progress bars and live refresh.
- [x] Native building descriptor, build-gun recipe and Tier 5 Logistics schematic source, gated behind Oil Processing.
- [x] Low-poly UV-mapped OBJ/MTL source model, customization-aware mesh proxy and Unreal Editor asset import utility.
- [x] Physical collision and explicit build-clearance volume.
- [ ] Import cooked mesh/icon and create/activate the Game Feature Data asset in the target Starter Project.
- [ ] Compile the native buildable/widget/content classes with custom UE 5.6.1 and test build-gun placement.
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

- [ ] Additional multi-stage contract families and special challenges.
- [x] Server-validated terminal UI actions through an owner-valid remote-call object (source implementation).
- [ ] Contract board graphical screen with interactive accept/deliver/cancel actions and improved history.
- [ ] Dedicated-server validation and broader multiplayer tests.
- [ ] Mod compatibility review and performance profiling.

## 1.0.0 — Release

- [ ] Full feature acceptance checklist.
- [ ] Stable save format and migration tests.
- [ ] Windows and Linux dedicated-server verification where supported.
- [ ] Alpakit release package, install instructions and user-facing documentation.
- [ ] Release notes, known issues and versioned compatibility declaration.
