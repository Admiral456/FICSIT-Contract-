# Changelog

## 0.1.1 — Source alpha hardening

- Verify the observed inventory count after the game's void-returning inventory removal call; only confirmed removals are credited to contracts.
- Log inventory removal mismatches and avoid awarding contract progress when no removal is observed.
- Normalize persisted contract amounts and reputation when restoring the portable state engine.
- Repair a zero next-contract serial and trim restored history to the 50-entry limit.
- Expand automated tests for invalid deliveries, progress restore, active-contract limits, offer expiry, malformed save state and history trimming.
- Avoid unnecessary repeated board replication when all unlocked templates are already offered or active.

This remains a source alpha. The SML plugin has not yet been compiled with the target custom Unreal Engine or tested inside Satisfactory.

## 0.1.0 — Initial contract loop

- Added a portable C++17 contract engine.
- Added small, medium, large and long-term contract templates gated by reputation.
- Added offer generation, acceptance limit, partial delivery, completion rewards, timed expiry and capped history.
- Added SML world module, server-replicated/saveable subsystem and chat command integration source.
- Added portable CMake tests and GitHub Actions CI.
- Documented compatibility assumptions, setup requirements, known limitations and in-game test checklist.
