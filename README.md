# FICSIT Contracts

**FICSIT Contracts** is a Satisfactory gameplay mod that turns production logistics into a larger progression system: multi-material delivery orders, timed emergency requests, strategic production chains, reputation ranks and a placeable FICSIT Dispatch Terminal.

> **Current status: 0.2.0-alpha.2 source update.** The portable contract engine now contains 21 order templates, five simultaneous offers, four active slots, multi-item manifests and six large multi-phase programs ranging from four to seven phases. The Dispatch Terminal source model and control-room UI have been upgraded. Unreal asset import, custom UE compilation, Alpakit packaging and in-game testing are still outstanding; this is not a ready-to-install release.

## Gameplay loop

- A server-authoritative board generates up to **five offers** at a time.
- Players can hold up to **four active orders** simultaneously.
- Offers are gated by FICSIT reputation, with higher-value industrial programs appearing at later ranks.
- Standard orders can request several different products in one manifest; every line must be completed.
- Long-term programs contain **four to seven sequential phases**. Later phases remain locked until every requirement in the current phase is delivered.
- One inventory delivery action can complete at most one phase, preventing a single bulk delivery from skipping the program sequence.
- Full program reputation is awarded once, only after the final phase.
- Timed emergency orders expire; expired orders grant no reputation.
- Cancelling an active order keeps its partial progress in history and charges a reputation fee (25% of its reward, rounded up, minimum 5 for positive rewards, capped by current reputation).
- Reputation, serial IDs, offers, active orders and history are saved and replicated by the Satisfactory world subsystem.

## Contract catalog

The source catalog contains **21 templates** across eight reputation thresholds. It includes basic stock orders, multi-material foundation and electrical batches, steelworks and machinery shipments, urgent grid recovery orders, and six strategic programs:

| Program | Phases | Reputation unlock | Reward |
| --- | ---: | ---: | ---: |
| Factory Backbone Program | 4 | 1,000 | 1,100 |
| Foundry Expansion Initiative | 4 | 1,000 | 1,350 |
| Automated Assembly Rollout | 5 | 1,500 | 1,650 |
| Planetary Power Grid Program | 5 | 1,500 | 1,850 |
| Planetary Logistics Network | 6 | 2,000 | 2,600 |
| Orbital Logistics Authority Program | 7 | 3,000 | 4,200 |

The programs cover site preparation, structural materials, reinforced assemblies, machinery, power distribution, control electronics and final commissioning. Each phase is a separate delivery manifest and is displayed in full in the terminal, including completed, current and locked phases.

## FICSIT Dispatch Terminal

The terminal is a placeable industrial logistics control station. Its source model is no longer a simple blockout: it includes an armoured floor plinth, lower power cabinet, central pedestal, twin service towers, operator deck, main and auxiliary screens, keypad, upper identity canopy, warning details and a status beacon.

The terminal includes:
- A native `AFICSITDispatchTerminal : AFGBuildable` actor with Satisfactory placement, collision and interaction.
- A native `UFGInteractWidget` dashboard for reputation, active operations, complete multi-phase routes, available orders and archive history.
- Accept, Deliver and Cancel actions routed through an owner-bound SML Remote Call Object.
- Server validation of the requesting player/controller, terminal lifetime and 450-unit interaction distance.
- Screen illumination, a beacon light and a fallback silhouette if the imported mesh is missing.
- A native building descriptor, build-gun recipe and Tier 5 Logistics schematic gated behind Oil Processing.

### Terminal construction cost

- 20 Heavy Modular Frames
- 20 Computers
- 50 Cable
- 40 Reinforced Iron Plates
- 10 Motors

### Dispatch Network schematic cost

- 25 Modular Frames
- 10 Computers
- 100 Cable
- 10 Motors

## Endgame Expansion design

The next progression expansion is documented in [ENDGAME_EXPANSION_DESIGN.md](docs/ENDGAME_EXPANSION_DESIGN.md) and defined in the machine-readable [EndgameProgression.json](Data/Progression/EndgameProgression.json).

The design catalog currently contains:
- **Six new Dispatch Terminal research chapters** and **three Space Elevator extensions (VI–VIII)**.
- **96 planned unlocks plus two native-source prototype entries (98 total)** across buildings, materials, miners, belts/lifts, storage, power, automation, weapons, ammunition, equipment, vehicles, blueprints, scanners and defense.
- **Four fictional resource families**: Ferravium, Cryonite, Asterite and Gravionite.
- **56 designed production/equipment recipes plus one prototype bootstrap recipe** with per-cycle inputs, outputs and cycle times.
- New weapons including Precision Rebar Rifle, Heavy Rebar Repeater, Cryo Projector, Photonic Rifle and Gravitic Launcher.
- Blueprint Designer Mk4 and Mk5 as extensions beyond the existing vanilla Mk3.
- A first native **Ferravium processing source prototype**: ore, concentrate and ingot descriptors; recovery/washing/smelting recipes; a 90 MW manufacturer; smelter build recipe; and a Phase-I-gated Frontier Survey schematic.
- A first **Advanced Metallurgy / Phase II source prototype**: Titan Alloy Ingot and Titan Alloy Beam descriptors, two Foundry Mk1 recipes, recipe unlock, purchase-gated milestone, and dedicated OBJ meshes/icon sources. The prototype temporarily uses the vanilla Foundry Mk1.
- Ferravium ore, concentrate, ingot, Titan Alloy Ingot, Titan Alloy Beam and smelter OBJ/MTL source meshes, plus a dedicated Unreal Editor icon/mesh importer.

A portable C++ research state machine enforces the ordered unlock gates; the SML subsystem saves and replicates progression state, and the Dispatch Terminal plus `/contracts progression` display it. The server subsystem binds to `AFGGamePhaseManager::mOnGameCompleted` and `AFGSchematicManager::PurchasedSchematicInstigatorDelegate`. It reconciles an already-completed vanilla finale through `GetGameIsCompleted()` and checks the two implemented milestone purchases explicitly in progression order with `IsSchematicPurchased()`. The implemented mapping is vanilla finale → Terminal I eligibility, Frontier Survey purchase → Terminal I complete, and Advanced Metallurgy purchase → Terminal II complete. The content now contains the Ferravium source processing line plus a Titan Alloy Ingot prototype. **Implementation status:** the wider catalogue remains design/data. The refinery recovery recipe is a temporary ore source, and both Titan Alloy bootstrap recipes temporarily use the vanilla Foundry Mk1; neither natural Ferravium deposits nor the dedicated Advanced Alloy Foundry / Industrial Press buildings are implemented. The bootstrap beam recipe is separate from the planned Industrial Press recipe in the design catalog. Terminal III–VI and Elevator VI–VIII purchase mappings, Unreal/UHT compilation, content import, Alpakit packaging and in-game tests remain outstanding.

The original vanilla Blueprint Designer already has Mk1, Mk2 and Mk3; this project intentionally adds Mk4/Mk5 rather than duplicating the existing tiers. New resource nodes in the vanilla World Partition map, custom weapon/equipment replication, defensive AI and quantum cargo transfer are called out as technical prototypes in the design document.

## Repository layout

```text
Source/FICSITContractsCore/        Portable contract rules shared with UE and tests
Mods/GameFeatures/FICSITContracts/ SML content-bearing Game Feature plugin
Mods/FICSITContracts/ContentSource/ OBJ/MTL terminal and Ferravium model sources
Tests/                             Portable C++ tests
.github/workflows/                 GitHub Actions validation
docs/                              Compatibility notes, balance, roadmap and test checklist
Tools/Unreal/                      Unreal Editor asset import utilities for the terminal and Ferravium
scripts/                           Static source and asset validation
```

## Development environment

Use the Satisfactory Modding Starter Project and its Satisfactory-compatible Unreal Engine installation. A normal Epic Games Launcher Unreal Engine installation is not sufficient.

The current compatibility target is Satisfactory 1.2 / SML 3.12.x:

- SML dependency: `^3.12.0`
- Minimum game build declared in the plugin: `CL#491125`
- Unreal Engine branch: Coffee Stain's customized UE 5.6.1
- Recommended Windows toolchain: Visual Studio 2022 with the required MSVC v143 components

### Add the mod to the Starter Project

1. Set up the Satisfactory Modding Starter Project and matching custom Unreal Engine using the official guide.
2. Copy `Mods/GameFeatures/FICSITContracts` into the Starter Project's `Mods/GameFeatures` directory. Also copy `Mods/FICSITContracts/ContentSource` and `Tools/Unreal` to the project root.
3. Regenerate Visual Studio project files.
4. Build the `FactoryGameEditor` target in Development Editor configuration.
5. Open the Starter Project in the custom editor and use Alpakit to stage/package FICSIT Contracts.
6. Install the staged mod into a test copy of Satisfactory with the matching SML version.

The inventory delivery adapter checks the actual inventory count before crediting progress; the core engine clamps malformed amounts, repairs a zero serial, trims oversized restored history, records penalized cancellations and persists multi-phase progress. Older single-stage save records remain supported through the legacy Requirements field.

## In-game commands

| Command | Purpose |
| --- | --- |
| `/contracts` | Show offers, active contracts, progress, reputation and rank |
| `/contracts accept FC-000001` | Accept the offer with the shown ID |
| `/contracts deliver FC-000001` | Deliver matching items from the player's main inventory |
| `/contracts cancel FC-000001` | Abandon an active order |
| `/contracts history` | Show the latest completed, expired and cancelled contracts |
| `/contracts progression` | Show vanilla Phase V confirmation, Terminal I–VI and Space Elevator VI–VIII status |

The IDs shown by the game are examples; use the IDs on your own board. The terminal provides the same actions through its interactive buttons.

## Automated tests

The portable contract rules are kept in a C++17 library and tested without Unreal Engine:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

GitHub Actions runs the portable build and tests on Ubuntu and Windows, plus static plugin/model checks. **This CI job is not an Unreal Engine build** and cannot prove that the SML plugin compiles or loads in Satisfactory.

## Known limitations in 0.2.0-alpha.2

- The terminal mesh, icon and Game Feature Data asset must still be imported/generated in the target Starter Project.
- The plugin has not yet been compiled with custom UE 5.6.1 or tested in-game.
- Vanilla item descriptor class paths must be confirmed in the target game asset registry before packaging.
- Deliveries currently come from the requesting player's main inventory; belt input and automated logistics acceptance are not implemented.
- Delivery removal is guarded by inventory read-back, but a transactional belt/inventory transfer layer is still required before automated intake.
- Dispatch animation, moving indicators and final sound assets are not implemented.
- The board is shared by the session rather than a separate per-player economy.
- Timed contracts use UTC wall-clock time, so they can expire while the game is closed.
- Reputation tuning and offer generation are source-defined; a player-facing configuration file is not included yet.

See [technical analysis](docs/TECHNICAL_ANALYSIS.md), [roadmap](docs/ROADMAP.md), [balance rules](docs/BALANCING.md), [Dispatch Terminal design](docs/DISPATCH_TERMINAL_DESIGN.md) and the [in-game test checklist](docs/GAME_TEST_CHECKLIST.md).

### Dispatch Terminal asset import

Run `Tools/Unreal/BuildFICSITContractsAssets.py` inside the SML 3.12 Unreal Editor. It imports the OBJ/MTL mesh, generates/imports the 256px terminal icon and creates the required `UFGGameFeatureData` asset. Set the Game Feature's Initial State to Active before compiling and packaging.
