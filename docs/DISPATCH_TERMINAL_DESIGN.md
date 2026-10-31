# FICSIT Dispatch Terminal — 0.2.0 implementation

## Role
A placeable FICSIT logistics workstation. It is a shared access point to the session-wide contract board; multiple terminals do not create duplicate contract state.

## Implemented source
- Native `AFICSITDispatchTerminal : AFGBuildable` actor with Satisfactory's buildable hologram, physical collision, placement-clearance data and a customisable mesh proxy.
- Native `UFGInteractWidget` interaction window with reputation/rank, active orders, available contracts, stage progress, recent history and an explicit refresh action.
- Accept, deliver and cancel buttons dispatch requests through the player's owner-bound SML Remote Call Object.
- The server revalidates the owning player/controller pair, terminal lifetime and 450-unit interaction radius before changing the shared board or the requesting player's inventory.
- Native building descriptor, build-gun recipe, recipe unlock and Tier 5 Logistics milestone registered by the root world module.
- The Dispatch Network milestone requires Oil Processing before it becomes available.
- Low-poly OBJ/MTL model source with six material slots; a blockout fallback remains available if the imported mesh is missing.
- Editor utility imports the mesh and 256px icon and creates the required `UFGGameFeatureData` asset.

## Terminal build cost
- 20 Heavy Modular Frames
- 20 Computers
- 50 Cable
- 40 Reinforced Iron Plates
- 10 Motors

## Schematic cost
- 25 Modular Frames
- 10 Computers
- 100 Cable
- 10 Motors

## Multiplayer and authority boundary
The terminal is a shared front-end, not a second contract database. All actions are sent through an RCO owned by the requesting player's controller. The server resolves that owner again and refuses requests from a player who is no longer the controller's character, from an invalid/destroyed terminal or from more than 450 Unreal units away. Delivery uses only that validated player's main inventory. The existing chat commands remain a fallback and all contract rules still live in the authoritative world subsystem.

## Editor steps
1. Copy the plugin into `Mods/GameFeatures/FICSITContracts` in the SML 3.12 Starter Project.
2. Copy `Mods/FICSITContracts/ContentSource` and `Tools/Unreal` from this repository into the project root.
3. Run `Tools/Unreal/BuildFICSITContractsAssets.py` in the Unreal Editor.
4. Open the `FICSITContracts` Game Feature Data asset and set Initial State to Active.
5. Regenerate project files, compile Development Editor and package with Alpakit.

No .uasset, cooked package, UE compile or in-game placement test is claimed by this source commit.
