#!/usr/bin/env python3
"""Validate the source plugin descriptor and expected C++ module files."""

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PLUGIN_DIR = ROOT / "Mods" / "FICSITContracts"
DESCRIPTOR = PLUGIN_DIR / "FICSITContracts.uplugin"


def main() -> int:
    descriptor = json.loads(DESCRIPTOR.read_text(encoding="utf-8"))
    errors: list[str] = []
    if descriptor.get("SemVersion") != descriptor.get("VersionName"):
        errors.append("SemVersion and VersionName must be identical.")
    if descriptor.get("Version") != 0:
        errors.append("Version must match the major version component of SemVersion (0).")
    if not descriptor.get("SemVersion", "").startswith("0.1.0"):
        errors.append("The source descriptor is expected to describe the 0.1.0 alpha.")
    if descriptor.get("GameVersion") != ">=491125":
        errors.append("GameVersion must retain the SML 3.12.0 minimum CL#491125.")
    if not any(plugin.get("Name") == "SML" and plugin.get("Enabled") is True and plugin.get("SemVersion") == "^3.12.0" for plugin in descriptor.get("Plugins", [])):
        errors.append("A required SML ^3.12.0 dependency is missing.")
    if not any(module.get("Name") == "FICSITContracts" and module.get("Type") == "Runtime" for module in descriptor.get("Modules", [])):
        errors.append("The FICSITContracts Runtime module is missing.")

    required_files = [
        PLUGIN_DIR / "Source/FICSITContracts/FICSITContracts.Build.cs",
        PLUGIN_DIR / "Source/FICSITContracts/Public/FICSITContractsSubsystem.h",
        PLUGIN_DIR / "Source/FICSITContracts/Private/FICSITContractsSubsystem.cpp",
        PLUGIN_DIR / "Source/FICSITContracts/Private/FICSITContractsWorldModule.cpp",
        PLUGIN_DIR / "Source/FICSITContracts/Private/FICSITContractsChatCommand.cpp",
    ]
    errors.extend(f"Required source file missing: {path.relative_to(ROOT)}" for path in required_files if not path.is_file())
    if errors:
        for error in errors:
            print(f"ERROR: {error}")
        return 1
    print("Plugin descriptor and expected SML module files are valid.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
