# FICSIT Contracts

**FICSIT Contracts** is a Satisfactory gameplay mod that adds a production-contract board, item delivery orders, company reputation and contract progression.

> **Current status: 0.2.0 source alpha.** The portable contract engine, sequential multi-stage orders and a native FICSIT Dispatch Terminal source implementation are now in the repository. The terminal includes a native buildable actor, Satisfactory interaction widget, native building descriptor/recipe/Tier 5 schematic and a low-poly OBJ/MTL source model. Unreal asset import, custom UE compilation and in-game testing are still outstanding; this is not a ready-to-install release.

## Version 0.1.x

The first playable slice is intentionally focused on the contract loop:

- A server-authoritative board creates up to three offers.
- Offers are selected from a fixed catalog and gated by reputation.
- Players can accept up to three active contracts.
- `/contracts` displays reputation, rank, available orders and active progress.
- `/contracts accept <ID>` accepts an offer.
- `/contracts deliver <ID>` deposits matching items from the player's main inventory. Only the amount still required is removed.
- `/contracts cancel <ID>` abandons an active order; its history is retained and a reputation fee is charged (25% of its reward, rounded up, minimum 5 when the reward is positive, capped by current reputation).
- Multi-stage orders unlock their requirements sequentially; future-stage products cannot be delivered early.
- Completing every stage grants reputation once and moves the order to history.
- Timed orders expire; expired orders do not grant reputation.
- Reputation, serial IDs, offers, active orders and history (including cancelled orders) are saved and replicated by the Satisfactory world subsystem.

The current offer catalog includes Iron Plates, Iron Rods, Wire, Cable, Reinforced Iron Plates, Rotors, Motors and Modular Frames. The long-term Factory Backbone Program is a three-stage supply chain: 2,000 Iron Plates, then 500 Reinforced Iron Plates, then 150 Motors. Its 750 REP reward is paid only after the final stage.

## Repository layout

```text
Source/FICSITContractsCore/        Portable contract rules shared with UE and tests
Mods/GameFeatures/FICSITContracts/ SML content-bearing Game Feature plugin
Mods/FICSITContracts/ContentSource/ Source OBJ/MTL model assets
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
2. Copy `Mods/GameFeatures/FICSITContracts` into the Starter Project's `Mods/GameFeatures` directory. Also copy `Mods/FICSITContracts/ContentSource` and `Tools/Unreal` to the project root.
3. Regenerate Visual Studio project files.
4. Build the `FactoryGameEditor` target in Development Editor configuration.
5. Open the Starter Project in the custom editor and use Alpakit to stage/package FICSIT Contracts.
6. Install the staged mod into a test copy of Satisfactory with the matching SML version.

The inventory delivery adapter now checks the actual inventory count before crediting progress; the core engine also clamps malformed amounts, repairs a zero serial, trims oversized restored history, records penalized cancellations and persists multi-stage progress. Older single-stage save records remain supported through the legacy Requirements field. This repository deliberately does not include the proprietary/custom Unreal Engine, Satisfactory game files, Starter Project, generated project files, or binary cooked assets.

## In-game commands

| Command | Purpose |
| --- | --- |
| `/contracts` | Show offers, active contracts, progress, reputation and rank |
| `/contracts accept FC-000001` | Accept the offer with the shown ID |
| `/contracts deliver FC-000001` | Deliver matching items from the player's main inventory |
| `/contracts cancel FC-000001` | Cancel an active order with a reputation penalty |
| `/contracts history` | Show the latest completed, expired and cancelled contracts |

The command IDs shown by the game are examples; use the IDs on your own board.

## Automated tests

The contract rules are kept in a portable C++17 library and tested without Unreal Engine:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

GitHub Actions runs these portable tests on pushes and pull requests. **This CI job is not an Unreal Engine build** and cannot prove that the SML plugin compiles or loads in Satisfactory.

## Known limitations in 0.2.0

- The terminal UI now has Accept, Deliver and Cancel buttons. Those actions are routed through an owner-bound SML Remote Call Object and revalidated against the requesting player and server-side inventory. The original chat commands remain available as a fallback. Belt input, dispatch animation and audio are not implemented yet.
- The board is shared by the session rather than a separate per-player economy.
- Item classes are resolved from vanilla asset class paths at runtime. These paths must be confirmed in the target Starter Project/game build before a packaged release.
- Timed contracts use UTC wall-clock time, so they can expire while the game is closed.
- Reputation tuning and offer generation are fixed in source for this alpha; a player-facing configuration file is not included yet.
- The Unreal plugin has not yet been compiled or tested in-game in this environment.

See [technical analysis](docs/TECHNICAL_ANALYSIS.md), [roadmap](docs/ROADMAP.md), [balance rules](docs/BALANCING.md) and the [in-game test checklist](docs/GAME_TEST_CHECKLIST.md).


### Dispatch Terminal asset import

Run `Tools/Unreal/BuildFICSITContractsAssets.py` inside the SML 3.12 Unreal Editor. It imports the OBJ/MTL mesh, generates and imports the 256px terminal icon, and creates the required `UFGGameFeatureData` asset. Set the Game Feature's Initial State to Active before compiling and packaging. See [Dispatch Terminal design](docs/DISPATCH_TERMINAL_DESIGN.md).
