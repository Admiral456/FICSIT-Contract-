# FICSIT Dispatch Terminal — upgraded industrial control station

## Role
A placeable logistics command station and the primary in-world interface for the shared FICSIT production-contract board. Multiple terminals are access points to one session-wide board; they do not create duplicate order state.

## Visual design
The source OBJ/MTL model is a detailed operator station rather than a primitive blockout. It is authored in centimeters and fits within the buildable placement envelope.

- Layered 164 cm-wide armoured floor plinth with orange perimeter accents and warning markings.
- Lower power cabinet with inset access doors, vents, fasteners and side port details.
- Central armoured pedestal with twin side-service towers.
- Wide operator console deck with a keypad, control buttons and front status strip.
- Three-display assembly: primary dispatch screen plus two auxiliary status screens.
- Upper FICSIT canopy, identity panel, mast and orange status beacon.
- Six material slots: Gunmetal, Steel, Black, Orange, Screen and Indicator.
- Cyan screen illumination and a separate orange beacon light.
- Fallback blockout components remain available if the imported mesh is missing.

## Implemented source
- Native `AFICSITDispatchTerminal : AFGBuildable` actor with buildable hologram, physical collision, placement-clearance data and customization-aware mesh proxy.
- Native `UFGInteractWidget` dashboard for reputation/rank, active operations, available orders, complete strategic-program routes and recent archive.
- Accept, Deliver and Cancel buttons dispatch requests through the requesting player's owner-bound SML Remote Call Object.
- The server revalidates the owning player/controller pair, terminal lifetime and 450-unit interaction radius before changing the shared board or the requesting player's inventory.
- Native building descriptor, build-gun recipe, recipe unlock and Tier 5 Logistics milestone registered by the root world module.
- Dispatch Network milestone requires Oil Processing before it becomes available and is included in public cooked builds.
- Unreal Editor utility imports the OBJ model and 256px icon and creates the required `UFGGameFeatureData` asset.

## Contract board capacity
- Five offers displayed at once, generated from a 21-template reputation-gated catalog.
- Up to four active orders per session.
- Multi-material manifests display every required material line.
- Strategic programs show every phase and distinguish COMPLETE, CURRENT and LOCKED phases.
- The full reputation reward is paid only after final program completion.

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
The terminal is a shared front-end, not a second contract database. All actions are sent through an RCO owned by the requesting player's controller. The server resolves that owner again and refuses requests from a player who is no longer the controller's character, from an invalid/destroyed terminal or from more than 450 Unreal units away. Delivery uses only that validated player's main inventory. Chat commands remain available as a fallback and all contract rules live in the authoritative world subsystem.

## Editor steps
1. Copy the plugin into `Mods/GameFeatures/FICSITContracts` in the SML 3.12 Starter Project.
2. Copy `Mods/FICSITContracts/ContentSource` and `Tools/Unreal` from this repository into the project root.
3. Run `Tools/Unreal/BuildFICSITContractsAssets.py` in Unreal Editor.
4. Open the `FICSITContracts` Game Feature Data asset and set Initial State to Active.
5. Regenerate project files, compile Development Editor and package with Alpakit.

The OBJ/MTL is source geometry, not a cooked `.uasset`. No UE compile, in-game placement test or packaged release is claimed by this source update.
