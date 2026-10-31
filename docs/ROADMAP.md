# Roadmap

## 0.1.x — Contract loop and source hardening

- [x] Portable contract engine.
- [x] Reputation-gated offer generation and contract catalog.
- [x] Partial delivery and exact completion/reward logic in the engine.
- [x] Timed expiry and capped history.
- [x] SML C++ world subsystem, save interface and replicated board source.
- [x] Server-authoritative chat commands.
- [x] Penalized cancellation with persisted history status and reputation-floor protection.
- [x] Sequential multi-stage orders with saved stage progress.
- [x] Inventory read-back guard so progress is only credited for observed removals.
- [x] Portable CMake tests and GitHub Actions CI.
- [ ] Compile with target custom UE 5.6.1 and SML 3.12.x.
- [ ] Verify item descriptor class paths in the target asset registry.
- [ ] In-game save/load and multiplayer test.

## 0.2.0 — Logistics and Dispatch Terminal

- [x] Native FICSIT Dispatch Terminal actor with Satisfactory build interaction.
- [x] Native board viewer for reputation, active orders, available offers and history.
- [x] Native building descriptor, build-gun recipe and Tier 5 Logistics schematic gated behind Oil Processing.
- [x] Source OBJ/MTL model and Unreal Editor asset import utility.
- [x] Physical collision, placement clearance and customization-aware mesh proxy.
- [x] Owner-bound server RPC path for terminal Accept, Deliver and Cancel actions.
- [x] Server validation of the player/controller, terminal lifetime and interaction distance.
- [x] Detailed industrial terminal source model with operator deck, triple displays, service modules, canopy and beacon.
- [x] Expanded dashboard with full program route and completed/current/locked phase display.
- [ ] Import cooked mesh/icon and create/activate Game Feature Data asset in the target Starter Project.
- [ ] Compile native buildable/widget/content classes with custom UE 5.6.1 and test build-gun placement.
- [ ] Alpakit/Game Feature packaging for content assets.
- [ ] Dispatch animation, final materials and sound assets.

## 0.2.1 — Expanded contract operations

- [x] Expand the catalog to 21 distinct templates across eight reputation thresholds.
- [x] Raise the offer board to five simultaneous offers and the active limit to four.
- [x] Add multi-material manifests where every line must be supplied.
- [x] Add six strategic programs ranging from four to seven sequential phases.
- [x] Add urgent timed grid-recovery request.
- [x] Expand reputation ranks through FICSIT Executive.
- [x] Display the complete strategic-program route in the terminal and command output.
- [ ] Confirm every new vanilla item class path in the target Satisfactory asset registry.
- [ ] Balance quantities and rewards through extended in-game playtests.
- [ ] Add optional player-facing settings for offer count, active slots, timers and rewards.

## 0.3.0 — Endgame progression foundation

- [x] Define six new Dispatch Terminal chapters and Space Elevator extensions VI–VIII.
- [x] Add the machine-readable endgame progression catalog with 96 planned unlocks.
- [x] Add four proposed resource families and their visual/processing identities.
- [x] Add 54 proposed production and equipment recipes.
- [x] Design weapon, ammunition, personal equipment, vehicle, blueprint, defense and automation categories.
- [x] Continue from the existing vanilla Blueprint Designer Mk3 with planned Mk4/Mk5 extensions.
- [x] Add validation for unique unlock/recipe IDs, phase ownership, catalog references, elevator gates and required component production.
- [ ] Implement a runtime catalog loader and a server-authoritative research/unlock registry.
- [x] Add SaveGame properties and server replication for Terminal I–VI and Space Elevator VI–VIII completion state.
- [ ] Build the remaining catalog-wide UFGSchematic/UFGUnlock registration layer; only the first Ferravium schematic is currently prototyped natively.
- [x] Add native Ferravium Ore, Concentrate and Ingot descriptors plus recovery, washing and smelting recipe classes.
- [x] Add the native AFICSITFerraviumSmelter manufacturer, factory input/output ports, buildable descriptor, Build Gun recipe and 90 MW base power setting.
- [x] Add the Frontier Survey schematic source and a prerequisite that waits for the saved Phase V / Terminal I state.
- [x] Add OBJ/MTL source meshes and the Unreal Editor importer for Ferravium item/smelter meshes and four icons.
- [ ] Import Ferravium assets, run UHT/UE 5.6.1 compilation, package with Alpakit and test the complete refinery-to-ingot loop in game.
- [ ] Prototype Blueprint Designer Mk4 and verify save compatibility, placement bounds and nested blueprints.
- [ ] Prototype the first new weapon, ammo, damage path and multiplayer replication.
- [ ] Resolve all vanilla item class paths against the target Satisfactory 1.2 asset registry.
- [ ] Resolve a supported new resource deposit/node path and ore scanner integration.
- [x] Add read-only Terminal/Space Elevator phase status to the Dispatch Terminal and `/contracts progression`.
- [ ] Connect vanilla Space Elevator Phase V completion and real custom schematic purchases to the server-side progression methods.
- [ ] Add a full research UI with category filters, item costs, unlock descriptions and purchase actions.

## 0.4.0 — Expanded gameplay

- [ ] Belt input and server-side item acceptance.
- [ ] Inventory/transfer transaction that cannot duplicate or lose items on failure.
- [ ] Additional challenge objectives beyond inventory delivery.
- [ ] Dedicated-server validation, broader multiplayer tests and performance profiling.
- [ ] Mod compatibility review and performance profiling.

## 1.0.0 — Release

- [ ] Full feature acceptance checklist.
- [ ] Stable save format and migration tests.
- [ ] Windows and Linux dedicated-server verification where supported.
- [ ] Alpakit release package, install instructions and user-facing documentation.
- [ ] Release notes, known issues and versioned compatibility declaration.
