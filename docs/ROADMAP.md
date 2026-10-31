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
- [x] Add the machine-readable endgame progression catalog with 95 planned unlocks plus four explicitly tracked source prototypes (99 total).
- [x] Add four proposed resource families and their visual/processing identities.
- [x] Add 56 designed production/equipment recipes plus separately tracked prototype/bootstrap and Build Gun recipes.
- [x] Design weapon, ammunition, personal equipment, vehicle, blueprint, defense and automation categories.
- [x] Continue from the existing vanilla Blueprint Designer Mk3 with planned Mk4/Mk5 extensions.
- [x] Add validation for unique unlock/recipe IDs, phase ownership, catalog references, elevator gates and required component production.
- [ ] Implement a runtime catalog loader and a server-authoritative research/unlock registry.
- [x] Add SaveGame properties and server replication for Terminal I–VI and Space Elevator VI–VIII completion state.
- [ ] Build the remaining catalog-wide UFGSchematic/UFGUnlock registration layer; only Frontier Survey and Advanced Metallurgy have native milestone prototypes so far.
- [x] Add native Ferravium Ore, Concentrate and Ingot descriptors plus recovery, washing and smelting recipe classes.
- [x] Add the native AFICSITFerraviumSmelter manufacturer, factory input/output ports, buildable descriptor, Build Gun recipe and 90 MW base power setting.
- [x] Add the Frontier Survey schematic source and a prerequisite that waits for the saved Phase V / Terminal I state.
- [x] Add the Advanced Metallurgy / Terminal II schematic, gated by purchased Frontier Survey and the saved Phase II availability state.
- [x] Add the native Titan Alloy Ingot descriptor and 8-second Advanced Alloy Foundry recipe (60 Ferravium Ingots + 60 Steel Ingots -> 60 Titan Alloy Ingots).
- [x] Add Titan Alloy Beam as a second Phase II product: 30 Titan Alloy Ingots + 60 Steel Pipes -> 30 Titan Alloy Beams per 12-second Advanced Alloy Foundry cycle, with its own descriptor, OBJ mesh and icon source.
- [x] Add Reinforced Titan Frame as a third Phase II product: 8 Titan Alloy Beams + 2 Heavy Modular Frames -> 2 frames per 24-second Manufacturer Mk1 bootstrap cycle, with its own descriptor, open-cage OBJ mesh and icon source; keep the final Heavy-Duty Assembler recipe separate.
- [x] Connect confirmed Advanced Metallurgy schematic purchases to the saved Terminal II completion flag and reconcile the Phase I/II purchases in order.
- [x] Add OBJ/MTL source meshes and the Unreal Editor importer for Ferravium and Titan Alloy items, the Ferravium Smelter, Advanced Alloy Foundry and Reinforced Titan Frame with eight generated icons.
- [ ] Import all Ferravium/Titan Alloy/Reinforced Frame/Foundry assets, run UHT/UE 5.6.1 compilation, package with Alpakit and test the complete production/research loops in game.
- [ ] Prototype Blueprint Designer Mk4 and verify save compatibility, placement bounds and nested blueprints.
- [ ] Prototype the first new weapon, ammo, damage path and multiplayer replication.
- [ ] Resolve all vanilla item class paths against the target Satisfactory 1.2 asset registry.
- [x] Implement the native Advanced Alloy Foundry manufacturer, buildable descriptor, Build Gun recipe, mixed-feed conveyor input/output, 125 MW base power and source OBJ/MTL model.
- [x] Move both Titan Alloy prototype recipes from the vanilla Foundry Mk1 to the dedicated Advanced Alloy Foundry; register the building recipe in the Advanced Metallurgy unlock.
- [x] Add Reinforced Titan Frame descriptor, Manufacturer Mk1 bootstrap recipe, separate OBJ mesh and generated icon; keep the Heavy-Duty Assembler recipe as the final design target.
- [ ] Prototype the dedicated Heavy-Duty Assembler and move the final Reinforced Titan Frame recipe there after in-game balancing.
- [ ] Balance the Advanced Alloy Foundry's production, power and construction costs in the target game.
- [ ] Resolve a supported new resource deposit/node path and ore scanner integration.
- [x] Add read-only Terminal/Space Elevator phase status to the Dispatch Terminal and `/contracts progression`.
- [x] Bind the server subsystem to `AFGGamePhaseManager::mOnGameCompleted`; reconcile saves where the vanilla Project Assembly finale is already complete.
- [x] Bind to `AFGSchematicManager::PurchasedSchematicInstigatorDelegate`, verify purchase state, and reconcile existing purchased schematics for the implemented Frontier Survey → Terminal I transition.
- [x] Add the first two native milestone schematics and map their confirmed purchases to Terminal I/II completion.
- [ ] Add actual schematic content and purchase mappings for Terminal III–VI and Space Elevator VI–VIII before marking those progression transitions implemented.
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
