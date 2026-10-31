# Dispatch Terminal source mesh

`DispatchTerminal.obj` is the detailed source model for the FICSIT Dispatch Terminal, authored in centimeters. The armoured logistics workstation includes a layered plinth, power cabinet, side-service towers, operator deck, keypad, triple-screen console, overhead canopy, rear service pack, warning markings and status beacon. Its approximate envelope is 168 cm wide, 120 cm deep and 235 cm high; the operator-facing side points toward negative Y.

The OBJ separates six material slots: Gunmetal, Steel, Black, Orange, Screen and Indicator. The UV coordinates are authored per face so the material slots remain stable during import.

Run `Tools/Unreal/BuildFICSITContractsAssets.py` inside the Satisfactory 1.2 / SML 3.12 Starter Project's Unreal Editor. It imports the OBJ, generates/imports the 256px icon and creates the required `UFGGameFeatureData` asset.

Expected assets:
- `/FICSITContracts/Buildables/DispatchTerminal/SM_DispatchTerminal`
- `/FICSITContracts/UI/TX_DispatchTerminal_256`
- `/FICSITContracts/FICSITContracts` (`FGGameFeatureData`)

Verify scale 1.0, generated lightmap UVs and all six material slots. The editor utility assigns the game's `MI_Factory_Base_01` to Gunmetal and Steel so the main surfaces respond to the Customizer; Orange, Black, Screen and Indicator retain their dedicated source materials. Set Game Feature Initial State to Active before packaging. The target Unreal Editor import and in-game appearance still require manual verification.
