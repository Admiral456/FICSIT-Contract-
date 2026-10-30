# Dispatch Terminal source mesh

`DispatchTerminal.obj` is the low-poly source model in centimeters. `DispatchTerminal.mtl` separates Gunmetal, Steel, Black, Orange, Screen and Indicator material slots.

Run `Tools/Unreal/BuildFICSITContractsAssets.py` inside the Satisfactory 1.2 / SML 3.12 Starter Project's Unreal Editor. It imports the OBJ, generates/imports the 256px icon and creates the required `UFGGameFeatureData` asset.

Expected assets:
- `/FICSITContracts/Buildables/DispatchTerminal/SM_DispatchTerminal`
- `/FICSITContracts/UI/TX_DispatchTerminal_256`
- `/FICSITContracts/FICSITContracts` (FGGameFeatureData)

Verify scale 1.0, generated lightmap UVs and separate material slots. Set Game Feature Initial State to Active before packaging.
