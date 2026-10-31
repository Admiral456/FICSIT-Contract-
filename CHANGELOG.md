# Changelog

## Unreleased — Endgame Expansion design catalog

- Define six Dispatch Terminal research chapters and three additional Space Elevator projects (VI–VIII).
- Add a machine-readable progression catalog with 96 planned unlocks across production, logistics, blueprints, weapons, ammunition, equipment, vehicles, defense, power and automation.
- Define four fictional endgame resource families: Ferravium, Cryonite, Asterite and Gravionite.
- Add 54 proposed production/equipment recipes, including all components required by Space Elevator VI–VIII.
- Design Blueprint Designer Mk4/Mk5 as extensions beyond the vanilla Mk3.
- Add CI validation for phase order, unique IDs, unlock references, recipe references, elevator gates and production of every Elevator requirement.
- Record implementation risks for World Partition resource nodes, ore scanner registration, custom equipment/weapons, AI drones and quantum material transfer.
- Implement a portable C++ state machine for Terminal I–VI and Elevator VI–VIII, with snapshot normalization and regression tests.
- Compile-check the same portable core wrapper used by the SML module on Linux and Windows.
- Correct the `/contracts` summary to show the four-slot active-contract limit and guard it in static validation.
- Store vanilla Phase V confirmation and Terminal/Elevator progression flags in SaveGame properties and replicate them to clients.
- Expose read-only progression status in the Dispatch Terminal and `/contracts progression` command.
- Add server-only progression adapter methods that re-check phase gates before modifying saved state.
- Extend static CI validation and the in-game checklist for save, replication, UI and progression gate behavior.

**Design only:** the catalog and validation do not register live schematics, items, weapons, buildings or recipes. Runtime integration and in-game testing remain pending.

## 0.2.0-alpha.2 — Expanded contracts and deluxe dispatch terminal

- Expand the catalog to 21 templates covering five reputation bands, multi-material delivery manifests and timed emergency orders.
- Add four- to seven-phase strategic programs for factory backbone, foundry expansion, automated assembly, planetary power, planetary logistics and orbital logistics.
- Increase the board to five simultaneous offers and the active-order limit to four.
- Expand the rank ladder through Regional Logistics Director and FICSIT Executive.
- Show the entire program route in the terminal, including completed, current and locked phases with every material line.
- Redesign the terminal dashboard as a logistics control room with clearer hierarchy, large operation cards, stronger status labels and phase progress.
- Replace the simple terminal blockout with a detailed industrial station model: layered plinth, armoured power cabinet, service towers, operator deck, triple display arrangement, keypad, upper canopy, vents, warning details and beacon.
- Add a beacon light and strengthen the screen illumination; expand the placement-clearance envelope for the upgraded console.
- Add regression coverage for multi-material phase gating, persistence, long production programs, expanded active slots and executive progression.

Unreal Editor import, custom UE compile, Alpakit packaging and in-game tests remain outstanding. The portable CI validates only the standalone C++ engine and static source/model rules.

## 0.2.0 — FICSIT Dispatch Terminal source implementation

- Add a native placeable `AFICSITDispatchTerminal` buildable actor with Satisfactory use interaction and a dedicated trace volume.
- Add a native `UFGInteractWidget` board viewer for reputation, available offers, active multi-stage orders and recent history.
- Add native building descriptor, build-gun recipe, recipe unlock and Tier 5 Logistics schematic gated behind Oil Processing.
- Add low-poly OBJ/MTL terminal source model and Unreal Editor utility for mesh/icon import and Game Feature Data creation.
- Move the content-bearing plugin into the SML 3.12 `Mods/GameFeatures` layout.
- Register a Game Instance module and owner-bound `UFGRemoteCallObject` for terminal button actions.

## 0.1.3 — Sequential long-term contract stages

- Convert Factory Backbone Program into ordered delivery stages.
- Prevent delivery of products assigned to future stages; one delivery action can complete at most one stage.
- Award the long-term reputation reward only after the final stage is completed.
- Persist stage definitions and current stage through the SML save/replication structs.
- Preserve compatibility with legacy single-stage contracts saved through the original Requirements field.
- Add automated coverage for sequential gating, stage progress, snapshot restore and final-only rewards.

## 0.1.2 — Contract cancellation

- Add `/contracts cancel <ID>` for active contracts.
- Apply a 25% reputation cancellation fee, rounded up, with a minimum of 5 for positive-reward contracts; the fee cannot reduce reputation below zero.
- Preserve partial delivery progress in cancellation history and distinguish cancelled orders from expired orders.
- Persist and replicate cancellation state through the SML subsystem.

## 0.1.1 — Source alpha hardening

- Verify the observed inventory count after the game's void-returning inventory removal call; only confirmed removals are credited to contracts.
- Normalize persisted contract amounts and reputation when restoring the portable state engine.
- Repair a zero next-contract serial and trim restored history to the 50-entry limit.
- Expand automated tests for invalid deliveries, progress restore, active-contract limits, offer expiry, malformed save state and history trimming.

## 0.1.0 — Initial contract loop

- Add a portable C++17 contract engine.
- Add offer generation, acceptance limit, partial delivery, completion rewards, timed expiry and capped history.
- Add SML world module, server-replicated/saveable subsystem and chat command integration source.
- Add portable CMake tests and GitHub Actions CI.
