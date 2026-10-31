#!/usr/bin/env python3
"""Validate the source plugin descriptor and expected C++ module files."""

import ast
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PLUGIN_DIR = ROOT / "Mods" / "GameFeatures" / "FICSITContracts"
DESCRIPTOR = PLUGIN_DIR / "FICSITContracts.uplugin"


def main() -> int:
    descriptor = json.loads(DESCRIPTOR.read_text(encoding="utf-8"))
    errors: list[str] = []
    sem_version = descriptor.get("SemVersion", "")
    if sem_version != descriptor.get("VersionName"):
        errors.append("SemVersion and VersionName must be identical.")
    if not isinstance(descriptor.get("Version"), int) or descriptor["Version"] < 1:
        errors.append("Version must be a positive, monotonically incremented plugin revision.")
    if not sem_version.startswith("0.2.0"):
        errors.append("The source descriptor is expected to describe the 0.2.0 alpha.")
    if descriptor.get("GameFeature") is not True:
        errors.append("The SML Game Feature plugin must set GameFeature=true.")
    if descriptor.get("CanContainContent") is not True:
        errors.append("The content-bearing Dispatch Terminal plugin must set CanContainContent=true.")
    if descriptor.get("GameVersion") != ">=491125":
        errors.append("GameVersion must retain the SML 3.12.0 minimum CL#491125.")
    if not any(
        plugin.get("Name") == "SML"
        and plugin.get("Enabled") is True
        and plugin.get("SemVersion") == "^3.12.0"
        for plugin in descriptor.get("Plugins", [])
    ):
        errors.append("A required SML ^3.12.0 dependency is missing.")
    if not any(
        module.get("Name") == "FICSITContracts" and module.get("Type") == "Runtime"
        for module in descriptor.get("Modules", [])
    ):
        errors.append("The FICSITContracts Runtime module is missing.")

    required_files = [
        PLUGIN_DIR / "Source/FICSITContracts/FICSITContracts.Build.cs",
        PLUGIN_DIR / "Source/FICSITContracts/Public/FICSITContractsSubsystem.h",
        PLUGIN_DIR / "Source/FICSITContracts/Private/FICSITContractsSubsystem.cpp",
        PLUGIN_DIR / "Source/FICSITContracts/Private/FICSITContractsWorldModule.cpp",
        PLUGIN_DIR / "Source/FICSITContracts/Private/FICSITContractsChatCommand.cpp",
        PLUGIN_DIR / "Source/FICSITContracts/Public/FICSITDispatchTerminal.h",
        PLUGIN_DIR / "Source/FICSITContracts/Private/FICSITDispatchTerminal.cpp",
        PLUGIN_DIR / "Source/FICSITContracts/Public/FICSITDispatchTerminalWidget.h",
        PLUGIN_DIR / "Source/FICSITContracts/Private/FICSITDispatchTerminalWidget.cpp",
        PLUGIN_DIR / "Source/FICSITContracts/Private/FICSITDispatchTerminalContent.cpp",
        PLUGIN_DIR / "Source/FICSITContracts/Public/FICSITContractsRemoteCallObject.h",
        PLUGIN_DIR / "Source/FICSITContracts/Private/FICSITContractsRemoteCallObject.cpp",
        PLUGIN_DIR / "Source/FICSITContracts/Public/FICSITContractsGameInstanceModule.h",
        PLUGIN_DIR / "Source/FICSITContracts/Private/FICSITContractsGameInstanceModule.cpp",
        PLUGIN_DIR / "Source/FICSITContracts/Public/FICSITDispatchTerminalContent.h",
    ]
    errors.extend(
        f"Required source file missing: {path.relative_to(ROOT)}"
        for path in required_files
        if not path.is_file()
    )
    source_assets = [
        ROOT / "Mods/FICSITContracts/ContentSource/Buildables/DispatchTerminal/DispatchTerminal.obj",
        ROOT / "Mods/FICSITContracts/ContentSource/Buildables/DispatchTerminal/DispatchTerminal.mtl",
        ROOT / "Tools/Unreal/BuildFICSITContractsAssets.py",
    ]

    # Keep the expanded gameplay catalog and board capacities from silently shrinking.
    core_source = ROOT / "Source/FICSITContractsCore/ContractEngine.cpp"
    subsystem_source = PLUGIN_DIR / "Source/FICSITContracts/Private/FICSITContractsSubsystem.cpp"
    if core_source.is_file():
        core_text = core_source.read_text(encoding="utf-8")
        for template_id in (
            "SUP-COPPER-CIRCUIT",
            "SUP-POWER-DISTRIBUTION",
            "URGENT-GRID-RECOVERY",
            "SPECIAL-FACTORY-BACKBONE",
            "SPECIAL-FOUNDRY-EXPANSION",
            "SPECIAL-AUTOMATION-ROLLOUT",
            "SPECIAL-POWER-GRID",
            "SPECIAL-PLANETARY-LOGISTICS",
            "SPECIAL-ORBITAL-LOGISTICS",
        ):
            if template_id not in core_text:
                errors.append(f"Expanded contract catalog is missing template: {template_id}")
        if "int ContractEngine::activeContractLimit() noexcept { return 4; }" not in core_text:
            errors.append("The expanded board must retain four active contract slots.")
    if subsystem_source.is_file():
        subsystem_text = subsystem_source.read_text(encoding="utf-8")
        if subsystem_text.count("ensureOffers(5,") < 4:
            errors.append("All board refresh paths must maintain five available offers.")
        if "PROGRAM ROUTE" not in subsystem_text or "LOCKED" not in subsystem_text:
            errors.append("Terminal/command output must expose the complete locked multi-phase route.")
    errors.extend(
        f"Required Dispatch Terminal source asset missing: {path.relative_to(ROOT)}"
        for path in source_assets if not path.is_file()
    )

    # Assert that the native buildable, UI and network action path stay connected.
    terminal_source = PLUGIN_DIR / "Source/FICSITContracts/Private/FICSITDispatchTerminal.cpp"
    widget_source = PLUGIN_DIR / "Source/FICSITContracts/Private/FICSITDispatchTerminalWidget.cpp"
    widget_header = PLUGIN_DIR / "Source/FICSITContracts/Public/FICSITDispatchTerminalWidget.h"
    rco_source = PLUGIN_DIR / "Source/FICSITContracts/Private/FICSITContractsRemoteCallObject.cpp"
    rco_header = PLUGIN_DIR / "Source/FICSITContracts/Public/FICSITContractsRemoteCallObject.h"
    schematic_source = PLUGIN_DIR / "Source/FICSITContracts/Private/FICSITDispatchTerminalContent.cpp"

    if terminal_source.is_file():
        terminal_text = terminal_source.read_text(encoding="utf-8")
        for required in (
            "UFGColoredInstanceMeshProxy",
            "AFGBuildableHologram::StaticClass()",
            "mClearanceData.Add",
            "HUD->OpenInteractUI",
        ):
            if required not in terminal_text:
                errors.append(f"Dispatch Terminal actor is missing required integration: {required}")
        if "mInteractWidgetSoftClass" in terminal_text:
            errors.append("Dispatch Terminal must not access AFGBuildable's private interaction-widget property.")

    if widget_source.is_file():
        widget_text = widget_source.read_text(encoding="utf-8")
        for required in (
            "Server_AcceptContract",
            "Server_DeliverContract",
            "Server_CancelContract",
            "SetPercent",
            "OnClicked.AddDynamic",
            "ActionHandlers.Reset",
            "mInteractObject",
        ):
            if required not in widget_text:
                errors.append(f"Dispatch Terminal UI is missing required feature: {required}")
        if "OnClicked.AddLambda" in widget_text:
            errors.append("UButton dynamic OnClicked delegates must not use AddLambda.")

    if widget_header.is_file():
        widget_header_text = widget_header.read_text(encoding="utf-8")
        if "UFICSITDispatchTerminalActionHandler" not in widget_header_text:
            errors.append("Dispatch Terminal UI is missing its reflected button-action handler.")

    if rco_source.is_file():
        rco_text = rco_source.read_text(encoding="utf-8")
        for required in (
            "GetOwnerPlayerCharacter",
            "GetOwnerPlayerController",
            "HasAuthority",
            "DeliverFromInventory",
            "GetDistanceTo",
            "IsActorBeingDestroyed",
            "GetLifetimeReplicatedProps",
            "DOREPLIFETIME",
        ):
            if required not in rco_text:
                errors.append(f"Remote Call Object is missing server validation: {required}")

    if rco_header.is_file():
        rco_header_text = rco_header.read_text(encoding="utf-8")
        for required in ("AFICSITDispatchTerminal* Terminal", "UPROPERTY(Replicated"):
            if required not in rco_header_text:
                errors.append(f"Remote Call Object header is missing required declaration: {required}")

    if schematic_source.is_file():
        schematic_text = schematic_source.read_text(encoding="utf-8")
        if "mSchematicDependencies.Add" not in schematic_text:
            errors.append("Dispatch Network schematic is missing its Oil Processing prerequisite.")
        for required in ("IIB_PublicBuilds", "mIncludeInBuilds", "GetUnderlyingProperty"):
            if required not in schematic_text:
                errors.append(f"Dispatch Network schematic is missing cook inclusion configuration: {required}.")

    # Parse the Editor utility without importing Unreal's Python module.
    asset_script = source_assets[-1]
    if asset_script.is_file():
        try:
            ast.parse(asset_script.read_text(encoding="utf-8"), filename=str(asset_script))
        except SyntaxError as exc:
            errors.append(f"Unreal asset utility has invalid Python syntax: {exc}")

    # Validate OBJ topology references, UV coordinates and MTL slot declarations.
    obj_path, mtl_path = source_assets[0], source_assets[1]
    if obj_path.is_file() and mtl_path.is_file():
        obj_lines = obj_path.read_text(encoding="utf-8").splitlines()
        vertex_positions: list[tuple[float, float, float]] = []
        for line in obj_lines:
            if line.startswith("v "):
                try:
                    x, y, z = (float(value) for value in line.split()[1:4])
                    vertex_positions.append((x, y, z))
                except (ValueError, IndexError):
                    errors.append("Dispatch Terminal OBJ contains a malformed vertex.")
        vertex_count = len(vertex_positions)
        uv_count = sum(line.startswith("vt ") for line in obj_lines)
        face_lines = [line for line in obj_lines if line.startswith("f ")]
        used_materials = {line[7:].strip() for line in obj_lines if line.startswith("usemtl ")}
        defined_materials = {
            line[7:].strip()
            for line in mtl_path.read_text(encoding="utf-8").splitlines()
            if line.startswith("newmtl ")
        }
        if vertex_count == 0 or uv_count == 0 or not face_lines:
            errors.append("Dispatch Terminal OBJ must contain vertices, UVs and faces.")
        if vertex_positions:
            bounds = [
                (min(point[axis] for point in vertex_positions), max(point[axis] for point in vertex_positions))
                for axis in range(3)
            ]
            if bounds[0][0] < -85.0 or bounds[0][1] > 85.0:
                errors.append("Dispatch Terminal OBJ exceeds the buildable X clearance.")
            if bounds[1][0] < -66.0 or bounds[1][1] > 60.0:
                errors.append("Dispatch Terminal OBJ exceeds the buildable Y clearance.")
            if bounds[2][0] < 0.0 or bounds[2][1] > 235.0:
                errors.append("Dispatch Terminal OBJ exceeds the buildable Z clearance.")
        for face_number, face in enumerate(face_lines, start=1):
            for corner in face[2:].split():
                parts = corner.split("/")
                try:
                    vertex_index = int(parts[0])
                    uv_index = int(parts[1]) if len(parts) > 1 and parts[1] else 0
                except (ValueError, IndexError):
                    errors.append(f"Dispatch Terminal OBJ face {face_number} has an invalid vertex/UV reference.")
                    break
                if not (1 <= vertex_index <= vertex_count) or not (1 <= uv_index <= uv_count):
                    errors.append(f"Dispatch Terminal OBJ face {face_number} references an out-of-range vertex or UV.")
                    break
        missing_materials = sorted(used_materials - defined_materials)
        if missing_materials:
            errors.append("Dispatch Terminal OBJ references undefined MTL slots: " + ", ".join(missing_materials))

    if errors:
        for error in errors:
            print(f"ERROR: {error}")
        return 1
    print("Plugin descriptor and Dispatch Terminal source checks are valid.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
