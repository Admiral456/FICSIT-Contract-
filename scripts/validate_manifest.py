#!/usr/bin/env python3
"""Validate the source plugin descriptor and expected C++ module files."""

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
    if errors:
        for error in errors:
            print(f"ERROR: {error}")
        return 1
    print("Plugin descriptor and expected SML module files are valid.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
