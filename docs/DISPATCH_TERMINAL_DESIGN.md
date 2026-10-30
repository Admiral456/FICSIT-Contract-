# FICSIT Dispatch Terminal — 0.2.0 implementation

## Role
A placeable FICSIT logistics workstation. It is a shared access point to the session-wide contract board; multiple terminals do not create duplicate contract state.

## Implemented source
- Native `AFICSITDispatchTerminal : AFGBuildable` actor with Satisfactory hologram, use interaction and a dedicated interaction trace volume.
- Native `UFGInteractWidget` interface with reputation/rank, active orders, available contracts, multi-stage progress and recent history.
- Refresh action reads the replicated contract subsystem.
- Native building descriptor, build-gun recipe, recipe unlock and Tier 5 Logistics milestone.
- Root world module registers the new schematic.
- Low-poly OBJ/MTL model source with six material slots; fallback blockout remains available if the imported mesh is missing.
- Editor utility creates the 256px icon and required `UFGGameFeatureData` asset.

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

## Current interaction boundary
The terminal UI is currently a live board viewer. Accept, deliver and cancel remain server-authoritative chat commands displayed in the UI footer. Do not directly mutate the replicated subsystem from a client widget. The next UI milestone must route actions through an owner-valid remote-call object and validate the requesting player on the server.

## Editor steps
1. Copy the plugin into `Mods/GameFeatures/FICSITContracts` in the SML 3.12 Starter Project.
2. Copy `Mods/FICSITContracts/ContentSource` and `Tools/Unreal` from this repository into the project root.
3. Run `Tools/Unreal/BuildFICSITContractsAssets.py` in the Unreal Editor.
4. Open the `FICSITContracts` Game Feature Data asset and set Initial State to Active.
5. Regenerate project files, compile Development Editor and package with Alpakit.

No .uasset, cooked package, UE compile or in-game placement test is claimed by this source commit.
