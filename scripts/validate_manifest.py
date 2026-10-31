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
        PLUGIN_DIR / "Source/FICSITContracts/Public/FICSITFerraviumContent.h",
        PLUGIN_DIR / "Source/FICSITContracts/Private/FICSITFerraviumContent.cpp",
        ROOT / "Source/FICSITContractsCore/EndgameProgression.h",
        ROOT / "Source/FICSITContractsCore/EndgameProgression.cpp",
        PLUGIN_DIR / "Source/FICSITContracts/Private/FICSITContractsCore.cpp",
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
        ROOT / "Mods/FICSITContracts/ContentSource/Resources/Ferravium/Mesh/Ferravium.mtl",
        ROOT / "Mods/FICSITContracts/ContentSource/Resources/Ferravium/Mesh/FerraviumOre.obj",
        ROOT / "Mods/FICSITContracts/ContentSource/Resources/Ferravium/Mesh/FerraviumConcentrate.obj",
        ROOT / "Mods/FICSITContracts/ContentSource/Resources/Ferravium/Mesh/FerraviumIngot.obj",
        ROOT / "Mods/FICSITContracts/ContentSource/Resources/Ferravium/Mesh/TitanAlloyIngot.obj",
        ROOT / "Mods/FICSITContracts/ContentSource/Resources/Ferravium/Mesh/TitanAlloyBeam.obj",
        ROOT / "Mods/FICSITContracts/ContentSource/Resources/Ferravium/Mesh/ReinforcedTitanFrame.obj",
        ROOT / "Mods/FICSITContracts/ContentSource/Buildables/FerraviumSmelter/Ferravium.mtl",
        ROOT / "Mods/FICSITContracts/ContentSource/Buildables/FerraviumSmelter/FerraviumSmelter.obj",
        ROOT / "Mods/FICSITContracts/ContentSource/Buildables/AdvancedAlloyFoundry/AdvancedAlloyFoundry.mtl",
        ROOT / "Mods/FICSITContracts/ContentSource/Buildables/AdvancedAlloyFoundry/AdvancedAlloyFoundry.obj",
        ROOT / "Tools/Unreal/BuildFerraviumAssets.py",
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
    chat_command_source = PLUGIN_DIR / "Source/FICSITContracts/Private/FICSITContractsChatCommand.cpp"
    if chat_command_source.is_file():
        chat_command_text = chat_command_source.read_text(encoding="utf-8")
        if "active %d/4" not in chat_command_text:
            errors.append("The /contracts summary must display the four active contract slots.")
        if "active %d/3" in chat_command_text:
            errors.append("The /contracts summary still displays the obsolete three-slot limit.")
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

    # Validate the complete endgame progression design catalog.
    progression_path = ROOT / "Data/Progression/EndgameProgression.json"
    if not progression_path.is_file():
        errors.append("Endgame progression catalog is missing.")
    else:
        try:
            progression = json.loads(progression_path.read_text(encoding="utf-8"))
        except (json.JSONDecodeError, OSError) as exc:
            progression = {}
            errors.append(f"Endgame progression catalog is invalid JSON: {exc}")

        if progression:
            if progression.get("schemaVersion") != 1:
                errors.append("Endgame progression catalog must use schemaVersion 1.")
            if progression.get("implementationStatus") != "design_only":
                errors.append("The design catalog must not claim unimplemented content is runtime-ready.")

            phase_list = progression.get("phases", [])
            phase_indices = [phase.get("index") for phase in phase_list]
            if len(phase_list) != 6 or phase_indices != [1, 2, 3, 4, 5, 6]:
                errors.append("Endgame progression must contain exactly six ordered terminal phases.")

            unlock_list = progression.get("unlocks", [])
            unlock_ids = [entry.get("id") for entry in unlock_list]
            if len(unlock_ids) != len(set(unlock_ids)) or any(not value for value in unlock_ids):
                errors.append("Endgame unlock IDs must be non-empty and globally unique.")
            unlock_by_id = {entry.get("id"): entry for entry in unlock_list if entry.get("id")}
            allowed_categories = {
                "building", "miner", "logistics", "power", "automation", "weapon", "material",
                "ammo", "equipment", "vehicle", "blueprint", "scanner", "defense",
            }
            referenced_unlocks: list[str] = []
            for phase in phase_list:
                phase_index = phase.get("index")
                for unlock_id in phase.get("unlockIds", []):
                    referenced_unlocks.append(unlock_id)
                    target = unlock_by_id.get(unlock_id)
                    if target is None:
                        errors.append(f"Terminal phase {phase_index} references unknown unlock: {unlock_id}")
                    elif target.get("phase") != phase_index:
                        errors.append(f"Unlock {unlock_id} is assigned to a different phase than its phase list.")
                if not phase.get("title") or not phase.get("requires"):
                    errors.append(f"Terminal phase {phase_index} is missing its title or gating requirement.")
            if sorted(referenced_unlocks) != sorted(unlock_ids):
                errors.append("Every endgame unlock must belong to exactly one terminal phase.")
            for entry in unlock_list:
                if entry.get("category") not in allowed_categories:
                    errors.append(f"Unlock {entry.get('id')} has an unsupported category.")
                if entry.get("status") not in {"planned", "native_source_prototype"}:
                    errors.append(f"Unlock {entry.get('id')} has an unsupported implementation status.")
                if entry.get("status") == "native_source_prototype" and not entry.get("sourceSliceId"):
                    errors.append(f"Prototype unlock {entry.get('id')} must reference an implementation slice.")

            recipe_list = progression.get("recipes", [])
            recipe_ids = [recipe.get("id") for recipe in recipe_list]
            if len(recipe_ids) != len(set(recipe_ids)) or any(not value for value in recipe_ids):
                errors.append("Endgame recipe IDs must be non-empty and globally unique.")
            recipe_by_id = {recipe.get("id"): recipe for recipe in recipe_list if recipe.get("id")}
            recipe_outputs: set[str] = set()
            for recipe in recipe_list:
                if recipe.get("phase") not in range(1, 7):
                    errors.append(f"Recipe {recipe.get('id')} references an invalid terminal phase.")
                duration = recipe.get("cycleSeconds")
                is_build_gun_recipe = recipe.get("recipeKind") == "build_gun"
                if (
                    not isinstance(duration, (int, float))
                    or duration < 0
                    or (duration == 0 and not is_build_gun_recipe)
                ):
                    errors.append(f"Recipe {recipe.get('id')} must have a positive cycle duration, except instant build-gun recipes.")
                for direction in ("inputs", "outputs"):
                    entries = recipe.get(direction, [])
                    if not entries:
                        errors.append(f"Recipe {recipe.get('id')} has no {direction}.")
                    for ingredient in entries:
                        item_id = ingredient.get("itemId")
                        amount = ingredient.get("amount")
                        if not item_id or not isinstance(amount, int) or amount <= 0:
                            errors.append(f"Recipe {recipe.get('id')} has an invalid {direction} entry.")
                        if direction == "outputs" and item_id:
                            recipe_outputs.add(item_id)
            for phase in phase_list:
                for recipe_id in phase.get("recipeIds", []):
                    recipe = recipe_by_id.get(recipe_id)
                    if recipe is None:
                        errors.append(f"Terminal phase {phase.get('index')} references unknown recipe: {recipe_id}")
                    elif recipe.get("phase") != phase.get("index"):
                        errors.append(f"Recipe {recipe_id} is assigned to a different phase than its phase list.")

            resource_ids = [resource.get("id") for resource in progression.get("resources", [])]
            if len(resource_ids) != 4 or len(resource_ids) != len(set(resource_ids)):
                errors.append("Endgame progression must define four unique new resource types.")

            elevator_phases = progression.get("progression", {}).get("elevatorExtensions", [])
            expected_elevator_unlocks = {6: [3, 4], 7: [5], 8: [6]}
            if [entry.get("phase") for entry in elevator_phases] != [6, 7, 8]:
                errors.append("Space Elevator extension must contain ordered phases VI, VII and VIII.")
            for entry in elevator_phases:
                phase_number = entry.get("phase")
                if entry.get("unlocksTerminalPhases") != expected_elevator_unlocks.get(phase_number):
                    errors.append(f"Space Elevator phase {phase_number} has an incorrect terminal unlock gate.")
                for requirement in entry.get("requirements", []):
                    if (
                        len(requirement) != 2
                        or not requirement[0]
                        or not isinstance(requirement[1], int)
                        or requirement[1] <= 0
                    ):
                        errors.append(f"Space Elevator phase {phase_number} has an invalid requirement.")
                    elif requirement[0] not in recipe_outputs:
                        errors.append(
                            f"Space Elevator phase {phase_number} requires {requirement[0]}, "
                            "but no endgame recipe produces it."
                        )

            blueprint_ids = {entry.get("id") for entry in unlock_list if entry.get("category") == "blueprint"}
            if not {"BLUEPRINT_DESIGNER_MK4", "BLUEPRINT_DESIGNER_MK5"}.issubset(blueprint_ids):
                errors.append("The expansion must continue from vanilla Blueprint Designer Mk3 with Mk4 and Mk5.")
            constraints = " ".join(progression.get("implementationConstraints", []))
            if "World Partition" not in constraints or "asset registry" not in constraints:
                errors.append("The endgame design must retain World Partition and vanilla asset-path implementation caveats.")

            ferravium_slice = next(
                (entry for entry in progression.get("implementationSlices", [])
                 if entry.get("id") == "FERRAVIUM_PROCESSING_BOOTSTRAP"),
                None,
            )
            if not ferravium_slice or ferravium_slice.get("status") != "native_source_prototype":
                errors.append("The Ferravium source prototype must be recorded in the design catalog.")
            if ferravium_slice and ferravium_slice.get("temporaryOreSource") != "RCP_FERRAVIUM_RECOVERY":
                errors.append("The Ferravium catalogue must clearly identify its temporary recovery source.")
            for required_recipe in ("RCP_FERRAVIUM_RECOVERY", "RCP_FERRAVIUM_CONCENTRATE",
                                    "RCP_FERRAVIUM_INGOT", "RCP_FERRAVIUM_SMELTER_BUILD"):
                if required_recipe not in recipe_by_id:
                    errors.append(f"Ferravium processing prototype is missing catalog recipe: {required_recipe}")

            titan_alloy_slice = next(
                (entry for entry in progression.get("implementationSlices", [])
                 if entry.get("id") == "ADVANCED_METALLURGY_TITAN_ALLOY_BOOTSTRAP"),
                None,
            )
            if not titan_alloy_slice or titan_alloy_slice.get("status") != "native_source_prototype":
                errors.append("The Titan Alloy bootstrap source prototype must be recorded in the design catalog.")
            titan_unlock = unlock_by_id.get("TITAN_ALLOY_INGOT")
            if not titan_unlock or titan_unlock.get("status") != "native_source_prototype":
                errors.append("Titan Alloy Ingot must be tracked as an implemented Phase II source unlock.")
            if titan_unlock and titan_unlock.get("sourceSliceId") != "ADVANCED_METALLURGY_TITAN_ALLOY_BOOTSTRAP":
                errors.append("Titan Alloy Ingot must reference its Phase II implementation slice.")
            titan_recipe = recipe_by_id.get("RCP_TITAN_ALLOY")
            if not titan_recipe or not any(
                output.get("itemId") == "TITAN_ALLOY_INGOT"
                for output in (titan_recipe or {}).get("outputs", [])
            ):
                errors.append("The Phase II Titan Alloy recipe must produce Titan Alloy Ingots.")
            if titan_alloy_slice and titan_alloy_slice.get("bootstrapMachine") != "Advanced Alloy Foundry":
                errors.append("Titan Alloy production must use the native Advanced Alloy Foundry.")
            foundry_unlock = unlock_by_id.get("ADVANCED_ALLOY_FOUNDRY")
            if not foundry_unlock or foundry_unlock.get("status") != "native_source_prototype":
                errors.append("Advanced Alloy Foundry must be tracked as an implemented Phase II buildable.")
            if foundry_unlock and foundry_unlock.get("sourceSliceId") != "ADVANCED_METALLURGY_TITAN_ALLOY_BOOTSTRAP":
                errors.append("Advanced Alloy Foundry must reference the Titan Alloy source implementation slice.")
            foundry_build_recipe = recipe_by_id.get("RCP_ADVANCED_ALLOY_FOUNDRY_BUILD")
            if not foundry_build_recipe or not any(
                output.get("itemId") == "ADVANCED_ALLOY_FOUNDRY"
                for output in (foundry_build_recipe or {}).get("outputs", [])
            ):
                errors.append("Advanced Alloy Foundry must have a Build Gun recipe that creates the building.")
            if foundry_build_recipe and (
                foundry_build_recipe.get("recipeKind") != "build_gun"
                or foundry_build_recipe.get("cycleSeconds") != 0
            ):
                errors.append("Advanced Alloy Foundry construction must be an instant Build Gun recipe.")
            if titan_recipe and titan_recipe.get("machine") != "Advanced Alloy Foundry":
                errors.append("Titan Alloy Ingot production must use the Advanced Alloy Foundry.")

            beam_unlock = unlock_by_id.get("TITAN_ALLOY_BEAM")
            if not beam_unlock or beam_unlock.get("status") != "native_source_prototype":
                errors.append("Titan Alloy Beam must be tracked as a native Phase II prototype unlock.")
            if beam_unlock and beam_unlock.get("sourceSliceId") != "ADVANCED_METALLURGY_TITAN_ALLOY_BOOTSTRAP":
                errors.append("Titan Alloy Beam must reference the Phase II prototype slice.")
            beam_recipe = recipe_by_id.get("RCP_TITAN_BEAM_BOOTSTRAP")
            if not beam_recipe or not any(
                output.get("itemId") == "TITAN_ALLOY_BEAM"
                for output in (beam_recipe or {}).get("outputs", [])
            ):
                errors.append("The Titan Alloy Beam bootstrap recipe must produce Titan Alloy Beams.")
            if beam_recipe and beam_recipe.get("designRecipeId") != "RCP_TITAN_BEAM":
                errors.append("The Titan Alloy Beam bootstrap must retain a link to the planned Industrial Press recipe.")
            if titan_alloy_slice and len(titan_alloy_slice.get("bootstrapRecipes", [])) < 2:
                errors.append("Both Titan Alloy Foundry bootstrap recipes must be documented.")
            if beam_recipe:
                if beam_recipe.get("machine") != "Advanced Alloy Foundry" or beam_recipe.get("cycleSeconds") != 12:
                    errors.append("Titan Alloy Beam must retain the 12-second Advanced Alloy Foundry process.")
                expected_beam_inputs = {"TITAN_ALLOY_INGOT": 30, "VANILLA_STEEL_PIPE": 60}
                actual_beam_inputs = {item.get("itemId"): item.get("amount") for item in beam_recipe.get("inputs", [])}
                if actual_beam_inputs != expected_beam_inputs:
                    errors.append("Titan Alloy Beam bootstrap inputs must be 30 Titan Alloy Ingots and 60 Steel Pipes.")
                actual_beam_outputs = {item.get("itemId"): item.get("amount") for item in beam_recipe.get("outputs", [])}
                if actual_beam_outputs != {"TITAN_ALLOY_BEAM": 30}:
                    errors.append("Titan Alloy Beam bootstrap output must be 30 beams per cycle.")

            frame_unlock = unlock_by_id.get("REINFORCED_TITAN_FRAME")
            if not frame_unlock or frame_unlock.get("status") != "native_source_prototype":
                errors.append("Reinforced Titan Frame must be tracked as a native Phase II prototype unlock.")
            if frame_unlock and frame_unlock.get("sourceSliceId") != "ADVANCED_METALLURGY_TITAN_ALLOY_BOOTSTRAP":
                errors.append("Reinforced Titan Frame must reference the Phase II prototype slice.")
            frame_recipe = recipe_by_id.get("RCP_REINFORCED_TITAN_FRAME_BOOTSTRAP")
            if not frame_recipe or not any(
                output.get("itemId") == "REINFORCED_TITAN_FRAME"
                for output in (frame_recipe or {}).get("outputs", [])
            ):
                errors.append("The Reinforced Titan Frame bootstrap recipe must produce Reinforced Titan Frames.")
            if frame_recipe and frame_recipe.get("designRecipeId") != "RCP_REINFORCED_TITAN_FRAME":
                errors.append("The frame bootstrap must remain separate from the planned Heavy-Duty Assembler recipe.")
            if frame_recipe:
                if frame_recipe.get("machine") != "Vanilla Manufacturer Mk1 (native prototype bootstrap)" or frame_recipe.get("cycleSeconds") != 24:
                    errors.append("Reinforced Titan Frame bootstrap must use the 24-second vanilla Manufacturer Mk1 process.")
                expected_frame_inputs = {"TITAN_ALLOY_BEAM": 8, "VANILLA_HEAVY_MODULAR_FRAME": 2}
                actual_frame_inputs = {item.get("itemId"): item.get("amount") for item in frame_recipe.get("inputs", [])}
                if actual_frame_inputs != expected_frame_inputs:
                    errors.append("Reinforced Titan Frame bootstrap inputs must be 8 Titan Alloy Beams and 2 Heavy Modular Frames.")
                actual_frame_outputs = {item.get("itemId"): item.get("amount") for item in frame_recipe.get("outputs", [])}
                if actual_frame_outputs != {"REINFORCED_TITAN_FRAME": 2}:
                    errors.append("Reinforced Titan Frame bootstrap output must be 2 frames per cycle.")
            if titan_alloy_slice and len(titan_alloy_slice.get("bootstrapRecipes", [])) < 3:
                errors.append("All three Titan Alloy Phase II bootstrap recipes must be documented.")

    # Verify the portable progression state machine matches every unlock ID in the catalog.
    progression_engine = ROOT / "Source/FICSITContractsCore/EndgameProgression.cpp"
    progression_tests = ROOT / "Tests/ContractEngineTests.cpp"
    cmake_file = ROOT / "CMakeLists.txt"
    if progression_engine.is_file():
        engine_text = progression_engine.read_text(encoding="utf-8")
        for unlock in (progression.get("unlocks", []) if "progression" in locals() and progression else []):
            expected_mapping = f'{{"{unlock.get("id")}", {unlock.get("phase")}}}'
            if expected_mapping not in engine_text:
                errors.append(f"Portable progression engine is missing unlock mapping: {unlock.get('id')}")
        for required in (
            "canResearchTerminalPhase",
            "completeTerminalPhase",
            "canCompleteElevatorPhase",
            "completeElevatorPhase",
            "availableTerminalPhase",
            "availableElevatorPhase",
            "normalize",
        ):
            if required not in engine_text:
                errors.append(f"Portable progression engine is missing required gate logic: {required}")
    if cmake_file.is_file():
        cmake_text = cmake_file.read_text(encoding="utf-8")
        if "Source/FICSITContractsCore/EndgameProgression.cpp" not in cmake_text:
            errors.append("CMake must compile the portable endgame progression engine.")
    plugin_core_wrapper = PLUGIN_DIR / "Source/FICSITContracts/Private/FICSITContractsCore.cpp"
    if plugin_core_wrapper.is_file():
        wrapper_text = plugin_core_wrapper.read_text(encoding="utf-8")
        for source_name in ("ContractEngine.cpp", "EndgameProgression.cpp"):
            if source_name not in wrapper_text:
                errors.append(f"SML runtime core wrapper does not compile {source_name}.")
    if cmake_file.is_file():
        cmake_text = cmake_file.read_text(encoding="utf-8")
        if "FICSITContractsPluginCoreCompileCheck" not in cmake_text:
            errors.append("CI must compile-check the SML runtime core wrapper.")
    if progression_tests.is_file():
        progression_test_text = progression_tests.read_text(encoding="utf-8")
        for required in (
            "EndgameProgression progression",
            "vanilla Phase V unlocks Terminal I",
            "Terminal II unlocks Elevator VI",
            "Elevator VII unlocks Terminal V",
            "Elevator VIII unlocks Terminal VI",
            "malformed endgame save",
        ):
            if required not in progression_test_text:
                errors.append(f"Endgame progression regression test is missing: {required}")

    # The live SML subsystem must persist and replicate research progression.
    subsystem_header = PLUGIN_DIR / "Source/FICSITContracts/Public/FICSITContractsSubsystem.h"
    subsystem_runtime = PLUGIN_DIR / "Source/FICSITContracts/Private/FICSITContractsSubsystem.cpp"
    chat_command = PLUGIN_DIR / "Source/FICSITContracts/Private/FICSITContractsChatCommand.cpp"
    terminal_widget = PLUGIN_DIR / "Source/FICSITContracts/Private/FICSITDispatchTerminalWidget.cpp"

    if subsystem_header.is_file():
        header_text = subsystem_header.read_text(encoding="utf-8")
        for required in (
            "UPROPERTY(SaveGame, ReplicatedUsing = OnRep_ContractBoard)",
            "bVanillaSpaceElevatorPhase5Complete",
            "TerminalResearchPhasesComplete",
            "ElevatorExtensionsComplete",
            "GetAvailableTerminalResearchPhase",
            "GetAvailableElevatorExtensionPhase",
            "RecordVanillaSpaceElevatorPhase5Complete",
            "CompleteTerminalResearchPhase",
            "CompleteElevatorExtensionPhase",
            "HandleVanillaGameCompleted",
            "HandleSchematicPurchased",
            "TryBindProgressionEvents",
            "virtual void EndPlay",
        ):
            if required not in header_text:
                errors.append(f"SML subsystem progression state is missing: {required}")

    if subsystem_runtime.is_file():
        runtime_text = subsystem_runtime.read_text(encoding="utf-8")
        for required in (
            '#include "EndgameProgression.h"',
            "BuildEndgameProgression",
            "ApplyEndgameProgression",
            "DOREPLIFETIME(AFICSITContractsSubsystem, bVanillaSpaceElevatorPhase5Complete)",
            "DOREPLIFETIME(AFICSITContractsSubsystem, TerminalResearchPhasesComplete)",
            "DOREPLIFETIME(AFICSITContractsSubsystem, ElevatorExtensionsComplete)",
            "Progression.completeTerminalPhase(Phase)",
            "Progression.completeElevatorPhase(Phase)",
            "if (!HasAuthority()) return false;",
            "GetEndgameProgressionSummary",
            '#include "FGGamePhaseManager.h"',
            '#include "FGSchematicManager.h"',
            '#include "FICSITFerraviumContent.h"',
            "mOnGameCompleted.AddUniqueDynamic",
            "PurchasedSchematicInstigatorDelegate.AddUniqueDynamic",
            "GetGameIsCompleted()",
            "IsSchematicPurchased(PurchasedSchematic)",
            "CompleteTerminalResearchPhase(ResearchPhase)",
            "RemoveDynamic",
            "ProgressionBindingRetryTimerHandle",
            "SchematicPurchaseRetryTimerHandle",
        ):
            if required not in runtime_text:
                errors.append(f"SML runtime progression integration is missing: {required}")

    if subsystem_runtime.is_file():
        for required in (
            "UFICSITAdvancedMetallurgySchematic::StaticClass()",
            "CompleteTerminalResearchPhase(ResearchPhase)",
            "IsSchematicPurchased(UFICSITFrontierSurveySchematic::StaticClass())",
            "IsSchematicPurchased(UFICSITAdvancedMetallurgySchematic::StaticClass())",
        ):
            if required not in runtime_text:
                errors.append(f"Advanced Metallurgy progression integration is missing: {required}")

    if chat_command.is_file():
        chat_text = chat_command.read_text(encoding="utf-8")
        if 'Subcommand == TEXT("progression")' not in chat_text or "GetEndgameProgressionSummary" not in chat_text:
            errors.append("The /contracts progression status command is missing.")

    if terminal_widget.is_file():
        terminal_widget_text = terminal_widget.read_text(encoding="utf-8")
        if "04  //  ENDGAME RESEARCH" not in terminal_widget_text or "GetEndgameProgressionSummary" not in terminal_widget_text:
            errors.append("Dispatch Terminal UI must display the endgame research progression.")

    # Parse Editor utilities without importing Unreal's Python module.
    editor_scripts = (
        ROOT / "Tools/Unreal/BuildFICSITContractsAssets.py",
        ROOT / "Tools/Unreal/BuildFerraviumAssets.py",
    )
    for asset_script in editor_scripts:
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

    # Validate all Ferravium OBJ/MTL pairs, including material sidecar coverage.
    ferravium_mesh_pairs = (
        (
            ROOT / "Mods/FICSITContracts/ContentSource/Resources/Ferravium/Mesh/FerraviumOre.obj",
            ROOT / "Mods/FICSITContracts/ContentSource/Resources/Ferravium/Mesh/Ferravium.mtl",
        ),
        (
            ROOT / "Mods/FICSITContracts/ContentSource/Resources/Ferravium/Mesh/FerraviumConcentrate.obj",
            ROOT / "Mods/FICSITContracts/ContentSource/Resources/Ferravium/Mesh/Ferravium.mtl",
        ),
        (
            ROOT / "Mods/FICSITContracts/ContentSource/Resources/Ferravium/Mesh/FerraviumIngot.obj",
            ROOT / "Mods/FICSITContracts/ContentSource/Resources/Ferravium/Mesh/Ferravium.mtl",
        ),
        (
            ROOT / "Mods/FICSITContracts/ContentSource/Resources/Ferravium/Mesh/TitanAlloyIngot.obj",
            ROOT / "Mods/FICSITContracts/ContentSource/Resources/Ferravium/Mesh/Ferravium.mtl",
        ),
        (
            ROOT / "Mods/FICSITContracts/ContentSource/Resources/Ferravium/Mesh/TitanAlloyBeam.obj",
            ROOT / "Mods/FICSITContracts/ContentSource/Resources/Ferravium/Mesh/Ferravium.mtl",
        ),
        (
            ROOT / "Mods/FICSITContracts/ContentSource/Buildables/FerraviumSmelter/FerraviumSmelter.obj",
            ROOT / "Mods/FICSITContracts/ContentSource/Buildables/FerraviumSmelter/Ferravium.mtl",
        ),
        (
            ROOT / "Mods/FICSITContracts/ContentSource/Buildables/AdvancedAlloyFoundry/AdvancedAlloyFoundry.obj",
            ROOT / "Mods/FICSITContracts/ContentSource/Buildables/AdvancedAlloyFoundry/AdvancedAlloyFoundry.mtl",
        ),
    )
    for obj_file, material_file in ferravium_mesh_pairs:
        if not obj_file.is_file() or not material_file.is_file():
            continue
        obj_lines = obj_file.read_text(encoding="utf-8").splitlines()
        vertex_count = sum(line.startswith("v ") for line in obj_lines)
        uv_count = sum(line.startswith("vt ") for line in obj_lines)
        faces = [line for line in obj_lines if line.startswith("f ")]
        used = {line[7:].strip() for line in obj_lines if line.startswith("usemtl ")}
        defined = {
            line[7:].strip()
            for line in material_file.read_text(encoding="utf-8").splitlines()
            if line.startswith("newmtl ")
        }
        if vertex_count < 6 or uv_count < 3 or not faces:
            errors.append(f"Ferravium mesh {obj_file.name} has incomplete geometry or UVs.")
        for face_number, face in enumerate(faces, start=1):
            for corner in face[2:].split():
                parts = corner.split("/")
                try:
                    vertex_index = int(parts[0])
                    uv_index = int(parts[1]) if len(parts) > 1 and parts[1] else 0
                except (ValueError, IndexError):
                    errors.append(f"Ferravium mesh {obj_file.name} face {face_number} has an invalid index.")
                    break
                if not (1 <= vertex_index <= vertex_count) or not (1 <= uv_index <= uv_count):
                    errors.append(f"Ferravium mesh {obj_file.name} face {face_number} points outside its vertex/UV arrays.")
                    break
        missing = sorted(used - defined)
        if missing:
            errors.append(f"Ferravium mesh {obj_file.name} references undefined materials: " + ", ".join(missing))
        if obj_file.name == "FerraviumSmelter.obj":
            positions = []
            for line in obj_lines:
                if line.startswith("v "):
                    try:
                        positions.append(tuple(float(value) for value in line.split()[1:4]))
                    except (ValueError, IndexError):
                        errors.append("Ferravium Smelter OBJ contains a malformed vertex.")
            if positions:
                bounds = [
                    (min(point[axis] for point in positions), max(point[axis] for point in positions))
                    for axis in range(3)
                ]
                if bounds[0][0] < -165.0 or bounds[0][1] > 165.0:
                    errors.append("Ferravium Smelter mesh exceeds the buildable X clearance.")
                if bounds[1][0] < -125.0 or bounds[1][1] > 125.0:
                    errors.append("Ferravium Smelter mesh exceeds the buildable Y clearance.")
                if bounds[2][0] < 0.0 or bounds[2][1] > 255.0:
                    errors.append("Ferravium Smelter mesh exceeds the buildable Z clearance.")
        elif obj_file.name == "AdvancedAlloyFoundry.obj":
            positions = []
            for line in obj_lines:
                if line.startswith("v "):
                    try:
                        positions.append(tuple(float(value) for value in line.split()[1:4]))
                    except (ValueError, IndexError):
                        errors.append("Advanced Alloy Foundry OBJ contains a malformed vertex.")
            if positions:
                bounds = [
                    (min(point[axis] for point in positions), max(point[axis] for point in positions))
                    for axis in range(3)
                ]
                expected_extents = ((-222.0, 222.0), (-165.0, 165.0), (0.0, 315.0))
                if bounds != list(expected_extents):
                    errors.append("Advanced Alloy Foundry mesh must retain its 444 x 316 x 315 cm clearance envelope.")
        elif obj_file.name == "TitanAlloyBeam.obj":
            positions = []
            for line in obj_lines:
                if line.startswith("v "):
                    try:
                        positions.append(tuple(float(value) for value in line.split()[1:4]))
                    except (ValueError, IndexError):
                        errors.append("Titan Alloy Beam OBJ contains a malformed vertex.")
            if positions:
                bounds = [
                    (min(point[axis] for point in positions), max(point[axis] for point in positions))
                    for axis in range(3)
                ]
                expected_extents = ((-70.0, 70.0), (-18.0, 18.0), (-15.0, 15.0))
                if bounds != list(expected_extents):
                    errors.append("Titan Alloy Beam mesh must retain its 140 x 36 x 30 cm I-section envelope.")
        elif obj_file.name == "ReinforcedTitanFrame.obj":
            positions = []
            for line in obj_lines:
                if line.startswith("v "):
                    try:
                        positions.append(tuple(float(value) for value in line.split()[1:4]))
                    except (ValueError, IndexError):
                        errors.append("Reinforced Titan Frame OBJ contains a malformed vertex.")
            if positions:
                bounds = [
                    (min(point[axis] for point in positions), max(point[axis] for point in positions))
                    for axis in range(3)
                ]
                expected_extents = ((-46.0, 46.0), (-32.0, 32.0), (-32.0, 32.0))
                if bounds != list(expected_extents):
                    errors.append("Reinforced Titan Frame mesh must retain its 92 x 64 x 64 cm envelope.")

    # The native Ferravium source slice must be registered and match its asset importer.
    ferravium_header = PLUGIN_DIR / "Source/FICSITContracts/Public/FICSITFerraviumContent.h"
    ferravium_source = PLUGIN_DIR / "Source/FICSITContracts/Private/FICSITFerraviumContent.cpp"
    world_module_source = PLUGIN_DIR / "Source/FICSITContracts/Private/FICSITContractsWorldModule.cpp"
    if ferravium_header.is_file():
        header_text = ferravium_header.read_text(encoding="utf-8")
        for required in (
            "UFGResourceDescriptor",
            "AFGBuildableManufacturer",
            "UFICSITFerraviumOreDescriptor",
            "UFICSITFerraviumConcentrateDescriptor",
            "UFICSITFerraviumIngotDescriptor",
            "UFICSITTitanAlloyIngotDescriptor",
            "UFICSITTitanAlloyBeamDescriptor",
            "UFICSITReinforcedTitanFrameDescriptor",
            "UFICSITTitanAlloyRecipe",
            "UFICSITTitanAlloyBeamRecipe",
            "UFICSITTitanAlloyRecipeUnlock",
            "AFICSITAdvancedAlloyFoundry",
            "UFICSITAdvancedAlloyFoundryDescriptor",
            "UFICSITAdvancedAlloyFoundryBuildRecipe",
            "UFICSITAdvancedMetallurgyPrerequisite",
            "UFICSITAdvancedMetallurgySchematic",
            "AFICSITFerraviumSmelter",
            "UFICSITFrontierSurveySchematic",
        ):
            if required not in header_text:
                errors.append(f"Ferravium content header is missing declaration: {required}")
    if ferravium_source.is_file():
        ferravium_text = ferravium_source.read_text(encoding="utf-8")
        for required in (
            'CreateDefaultSubobject<UFGFactoryConnectionComponent>(TEXT("Input0"))',
            'CreateDefaultSubobject<UFGFactoryConnectionComponent>(TEXT("Output0"))',
            "SetDirection(EFactoryConnectionDirection::FCD_INPUT)",
            "SetDirection(EFactoryConnectionDirection::FCD_OUTPUT)",
            "SetForwardPeekAndGrabToBuildable(true)",
            "mManufacturingSpeed = 1.0f",
            "mPowerConsumption",
            "mRecipes.Add(UFICSITFerraviumRecoveryRecipe::StaticClass())",
            "mRecipes.Add(UFICSITFerraviumConcentrateRecipe::StaticClass())",
            "mRecipes.Add(UFICSITFerraviumSmeltingRecipe::StaticClass())",
            "mRecipes.Add(UFICSITFerraviumSmelterBuildRecipe::StaticClass())",
            "GetAvailableTerminalResearchPhase() == 1",
            "UFICSITTitanAlloyRecipe::UFICSITTitanAlloyRecipe()",
            "mRecipes.Add(UFICSITTitanAlloyRecipe::StaticClass())",
            "UFICSITTitanAlloyBeamRecipe::UFICSITTitanAlloyBeamRecipe()",
            "UFICSITReinforcedTitanFrameDescriptor::UFICSITReinforcedTitanFrameDescriptor()",
            "UFICSITReinforcedTitanFrameRecipe::UFICSITReinforcedTitanFrameRecipe()",
            "mRecipes.Add(UFICSITReinforcedTitanFrameRecipe::StaticClass())",
            "mProduct.Emplace(UFICSITReinforcedTitanFrameDescriptor::StaticClass(), 2)",
            'FICSITTitanAlloyBeamDescriptor"), 8)',
            'Desc_ModularFrameHeavy.Desc_ModularFrameHeavy_C"), 2)',
            "Build_ManufacturerMk1.Build_ManufacturerMk1_C",

            "mRecipes.Add(UFICSITTitanAlloyBeamRecipe::StaticClass())",
            "mRecipes.Add(UFICSITAdvancedAlloyFoundryBuildRecipe::StaticClass())",
            "AFICSITAdvancedAlloyFoundry::AFICSITAdvancedAlloyFoundry()",
            "UFICSITAdvancedAlloyFoundryDescriptor::UFICSITAdvancedAlloyFoundryDescriptor()",
            "UFICSITAdvancedAlloyFoundryBuildRecipe::UFICSITAdvancedAlloyFoundryBuildRecipe()",
            "Power->SetPropertyValue_InContainer(this, 125.0f)",
            'CreateDefaultSubobject<UFGFactoryConnectionComponent>(TEXT("Input0"))',
            'CreateDefaultSubobject<UFGFactoryConnectionComponent>(TEXT("Output0"))',
            "mProducedIn.Add(TSoftClassPtr<UObject>(AFICSITAdvancedAlloyFoundry::StaticClass()))",
            "UFICSITAdvancedAlloyFoundryDescriptor::StaticClass()",
            "UFICSITTitanAlloyBeamDescriptor::StaticClass()",
            "mManufactoringDuration = 12.0f",
            "mProduct.Emplace(UFICSITTitanAlloyBeamDescriptor::StaticClass(), 30)",
            'FICSITTitanAlloyIngotDescriptor"), 30)',
            'Desc_SteelPipe.Desc_SteelPipe_C"), 60)',
            "SM_TitanAlloyBeam.SM_TitanAlloyBeam",
            "Desc_SteelIngot.Desc_SteelIngot_C",
            "SM_AdvancedAlloyFoundry.SM_AdvancedAlloyFoundry",
            "TX_AdvancedAlloyFoundry_256",
            "GetAvailableTerminalResearchPhase() == 2",
            "UFICSITFrontierSurveySchematic::StaticClass()",
            "UFICSITDispatchTerminalSchematic::StaticClass()",
        ):
            if required not in ferravium_text:
                errors.append(f"Ferravium source prototype is missing required integration: {required}")
    if world_module_source.is_file():
        world_text = world_module_source.read_text(encoding="utf-8")
        if (
            '#include "FICSITFerraviumContent.h"' not in world_text
            or "mSchematics.Add(UFICSITFrontierSurveySchematic::StaticClass())" not in world_text
            or "mSchematics.Add(UFICSITAdvancedMetallurgySchematic::StaticClass())" not in world_text
        ):
            errors.append("Frontier Survey and Advanced Metallurgy schematics must be registered in the SML world module.")

    ferravium_importer = ROOT / "Tools/Unreal/BuildFerraviumAssets.py"
    if ferravium_importer.is_file():
        importer_text = ferravium_importer.read_text(encoding="utf-8")
        for required in (
            "FerraviumOre.obj",
            "FerraviumConcentrate.obj",
            "FerraviumIngot.obj",
            "TitanAlloyIngot.obj",
            "TitanAlloyBeam.obj",
            "FerraviumSmelter.obj",
            "AdvancedAlloyFoundry.obj",
            "AdvancedAlloyFoundry.mtl",
            "TX_AdvancedAlloyFoundry_256",
            "TX_FerraviumOre_256",
            "TX_FerraviumConcentrate_256",
            "TX_FerraviumIngot_256",
            "TX_TitanAlloyIngot_256",
            "TX_TitanAlloyBeam_256",
            "TX_ReinforcedTitanFrame_256",
            "TX_FerraviumSmelter_256",
        ):
            if required not in importer_text:
                errors.append(f"Ferravium asset importer is missing source asset reference: {required}")

    if errors:
        for error in errors:
            print(f"ERROR: {error}")
        return 1
    print("Plugin descriptor, Ferravium machine wiring, asset source and progression checks are valid.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
