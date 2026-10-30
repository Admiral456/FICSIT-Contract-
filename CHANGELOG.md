# Changelog

## 0.2.0 — FICSIT Dispatch Terminal source implementation

- Add a native placeable `AFICSITDispatchTerminal` buildable actor with Satisfactory use interaction and a dedicated trace volume.
- Add a native `UFGInteractWidget` board viewer for reputation, available offers, active multi-stage orders and recent history.
- Add native building descriptor, build-gun recipe, recipe unlock and Tier 5 Logistics milestone registration.
- Add low-poly OBJ/MTL terminal source model and an Unreal Editor utility for mesh/icon import and Game Feature Data creation.
- Move the content-bearing plugin into the SML 3.12 `Mods/GameFeatures` layout.
- Register a Game Instance module and owner-bound `UFGRemoteCallObject` for terminal button actions.
- Correct the vanilla Reinforced Iron Plate descriptor path after checking the current SML asset tree.

Add owner-bound server RPC actions for accepting, delivering and cancelling directly from the terminal UI. Each request validates the owning player and uses the server-authoritative contract subsystem. Unreal Editor import, UE compile and in-game tests are still outstanding.

## 0.1.3 — Sequential long-term contract stages

- Convert Factory Backbone Program into three ordered stages: Iron Plates, Reinforced Iron Plates, then Motors.
- Prevent delivery of products assigned to future stages; one delivery action can complete at most one stage.
- Award the long-term reputation reward only after the final stage is completed.
- Persist stage definitions and current stage through the SML save/replication structs.
- Preserve compatibility with legacy single-stage contracts saved through the original Requirements field.
- Add automated coverage for sequential gating, stage progress, snapshot restore and final-only rewards.

This remains a source alpha. The SML plugin has not yet been compiled with the target custom Unreal Engine or tested inside Satisfactory.

## 0.1.2 — Contract cancellation

- Add `/contracts cancel <ID>` for active contracts.
- Apply a 25% reputation cancellation fee, rounded up, with a minimum of 5 for positive-reward contracts; the fee cannot reduce reputation below zero.
- Preserve partial delivery progress in cancellation history and distinguish cancelled orders from expired orders.
- Persist and replicate cancellation state through the SML subsystem.
- Add portable tests for fee rounding, the minimum fee, low-reputation clamping, single cancellation and expiry precedence.

This remains a source alpha. The SML plugin has not yet been compiled with the target custom Unreal Engine or tested inside Satisfactory.

## 0.1.1 — Source alpha hardening

- Verify the observed inventory count after the game's void-returning inventory removal call; only confirmed removals are credited to contracts.
- Log inventory removal mismatches and avoid awarding contract progress when no removal is observed.
- Normalize persisted contract amounts and reputation when restoring the portable state engine.
- Repair a zero next-contract serial and trim restored history to the 50-entry limit.
- Expand automated tests for invalid deliveries, progress restore, active-contract limits, offer expiry, malformed save state and history trimming.
- Avoid unnecessary repeated board replication when all unlocked templates are already offered or active.
- Run portable C++ CI checks on both Ubuntu and Windows; update checkout action to v7.

This remains a source alpha. The SML plugin has not yet been compiled with the target custom Unreal Engine or tested inside Satisfactory.

## 0.1.0 — Initial contract loop

- Added a portable C++17 contract engine.
- Added small, medium, large and long-term contract templates gated by reputation.
- Added offer generation, acceptance limit, partial delivery, completion rewards, timed expiry and capped history.
- Added SML world module, server-replicated/saveable subsystem and chat command integration source.
- Added portable CMake tests and GitHub Actions CI.
- Documented compatibility assumptions, setup requirements, known limitations and in-game test checklist.
