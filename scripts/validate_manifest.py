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
    errors.extend(
        f"Required Dispatch Terminal source asset missing: {path.relative_to(ROOT)}"
        for path in source_assets if not path.is_file()
    )

    # Assert the safety-critical terminal integration is present in source.
    terminal_source = PLUGIN_DIR / "Source/FICSITContracts/Private/FICSITDispatchTerminal.cpp"
    widget_source = PLUGIN_DIR / "Source/FICSITContracts/Private/FICSITDispatchTerminalWidget.cpp"
    rco_source = PLUGIN_DIR / "Source/FICSITContracts/Private/FICSITContractsRemoteCallObject.cpp"
    schematic_source = PLUGIN_DIR / "Source/FICSITContracts/Private/FICSITDispatchTerminalContent.cpp"
    if terminal_source.is_file():
        terminal_text = terminal_source.read_text(encoding="utf-8")
        for required in ("UFGColoredInstanceMeshProxy", "AFGBuildableHologram::StaticClass()", "mClearanceData.Add"):
            if required not in terminal_text:
                errors.append(f"Dispatch Terminal actor is missing required integration: {required}")
    if widget_source.is_file():
        widget_text = widget_source.read_text(encoding="utf-8")
        for required in ("Server_AcceptContract", "Server_DeliverContract", "Server_CancelContract", "SetPercent"):
            if required not in widget_text:
                errors.append(f"Dispatch Terminal UI is missing required feature: {required}")
    if rco_source.is_file():
        rco_text = rco_source.read_text(encoding="utf-8")
        for required in ("GetOwnerPlayerCharacter", "GetOwnerPlayerController", "HasAuthority", "DeliverFromInventory"):
            if required not in rco_text:
                errors.append(f"Remote Call Object is missing server validation: {required}")
    if schematic_source.is_file() and "mSchematicDependencies.Add" not in schematic_source.read_text(encoding="utf-8"):
        errors.append("Dispatch Network schematic is missing its Oil Processing prerequisite.")

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
        vertex_count = sum(line.startswith("v ") for line in obj_lines)
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
    print("Plugin descriptor and expected SML module files are valid.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
