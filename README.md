# FICSIT Contracts

**FICSIT Contracts** is a Satisfactory gameplay mod that adds a production-contract board, item delivery orders, company reputation and contract progression.

> **Current status: 0.1.0 source alpha.** The portable contract engine and its automated tests are implemented. The SML/Unreal C++ integration is prepared in this repository, but it has not yet been compiled with the Satisfactory custom Unreal Engine or tested inside the game. Do not treat this repository as a ready-to-install release yet.

## Version 0.1.0

The first playable slice is intentionally focused on the contract loop:

- A server-authoritative board creates up to three offers.
- Offers are selected from a fixed catalog and gated by reputation.
- Players can accept up to three active contracts.
- `/contracts` displays reputation, rank, available orders and active progress.
- `/contracts accept <ID>` accepts an offer.
- `/contracts deliver <ID>` deposits matching items from the player's main inventory. Only the amount still required is removed.
- Completing every requirement grants reputation once and moves the order to history.
- Timed orders expire; expired orders do not grant reputation.
- Reputation, serial IDs, offers, active orders and history are saved and replicated by the Satisfactory world subsystem.

The current offer catalog includes Iron Plates, Iron Rods, Wire, Cable, Reinforced Iron Plates, Rotors, Motors, Modular Frames and a multi-product long-term factory supply order.

## Repository layout

```text
Source/FICSITContractsCore/        Portable contract rules shared with UE and tests
Mods/FICSITContracts/              SML C++ runtime plugin
Tests/                             Portable C++ tests
.github/workflows/                 GitHub Actions validation
docs/                              Compatibility notes, roadmap and test checklist
```

## Development environment

Use the Satisfactory Modding Starter Project and its custom Satisfactory-compatible Unreal Engine installation. A normal Epic Games Launcher Unreal Engine installation is not sufficient.

The current compatibility target is Satisfactory 1.2 / SML 3.12.x:

- SML dependency: `^3.12.0`
- Minimum game build declared in the plugin: `CL#491125`
- Unreal Engine branch: Coffee Stain's customized UE 5.6.1
- Recommended Windows toolchain: Visual Studio 2022 with the required MSVC v143 components, following the current modding documentation

### Add the mod to the Starter Project

1. Set up the Satisfactory Modding Starter Project and the matching custom Unreal Engine using the official guide.
2. Copy this repository's `Mods/FICSITContracts` directory into the Starter Project's `Mods/` directory.
3. Regenerate Visual Studio project files.
4. Build the `FactoryGameEditor` target in Development Editor configuration.
5. Open the Starter Project in the custom editor and use Alpakit to stage/package FICSIT Contracts.
6. Install the staged mod into a test copy of Satisfactory with the matching SML version.

This repository deliberately does not include the proprietary/custom Unreal Engine, Satisfactory game files, Starter Project, generated project files, or binary cooked assets.

## In-game commands

| Command | Purpose |
| --- | --- |
| `/contracts` | Show offers, active contracts, progress, reputation and rank |
| `/contracts accept FC-000001` | Accept the offer with the shown ID |
| `/contracts deliver FC-000001` | Deliver matching items from the player's main inventory |
| `/contracts history` | Show the latest completed and expired contracts |

The command IDs shown by the game are examples; use the IDs on your own board.

## Automated tests

The contract rules are kept in a portable C++17 library and tested without Unreal Engine:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

GitHub Actions runs these portable tests on pushes and pull requests. **This CI job is not an Unreal Engine build** and cannot prove that the SML plugin compiles or loads in Satisfactory.

## Known limitations in 0.1.0

- No Dispatch Terminal building, conveyor input, custom mesh, animations or dedicated graphical contract screen yet.
- The board is shared by the session rather than a separate per-player economy.
- Item classes are resolved from vanilla asset class paths at runtime. These paths must be confirmed in the target Starter Project/game build before a packaged release.
- Timed contracts use UTC wall-clock time, so they can expire while the game is closed.
- Reputation tuning and offer generation are fixed in source for this alpha; a player-facing configuration file is not included yet.
- The Unreal plugin has not yet been compiled or tested in-game in this environment.

See [technical analysis](docs/TECHNICAL_ANALYSIS.md), [roadmap](docs/ROADMAP.md), [balance rules](docs/BALANCING.md) and the [in-game test checklist](docs/GAME_TEST_CHECKLIST.md).
