#include "EndgameProgression.h"

#include <algorithm>
#include <iterator>
#include <array>

namespace FicsitContracts {
namespace {

struct UnlockPhase {
    const char* id;
    int phase;
};

// IDs mirror Data/Progression/EndgameProgression.json. Keep this lookup small
// and deterministic; the JSON catalog remains the content-design source of truth.
constexpr UnlockPhase kUnlockPhases[] = {
    {"GEO_SURVEY_STATION", 1}, {"DEEP_CORE_EXTRACTOR", 1}, {"ORE_WASHING_PLANT", 1},
    {"FERRAVIUM_SMELTER", 1}, {"FIELD_WORKSHOP", 1}, {"SURVEY_BEACON_MK2", 1},
    {"SCANNER_FERRAVIUM", 1}, {"SURVEY_VISOR", 1}, {"SCOUT_DRONE", 1},
    {"PORTABLE_MINER_MK2", 1}, {"PRECISION_REBAR_RIFLE", 1}, {"FERRAVIUM_DARTS", 1},
    {"BLUEPRINT_DESIGNER_MK4", 1}, {"DEEP_CORE_STORAGE", 1},

    {"TITAN_ALLOY_INGOT", 2},
    {"TITAN_ALLOY_BEAM", 2},
    {"ADVANCED_ALLOY_FOUNDRY", 2}, {"INDUSTRIAL_PRESS", 2}, {"HEAVY_DUTY_ASSEMBLER", 2},
    {"MINER_MK4", 2}, {"CONVEYOR_MK7", 2}, {"LIFT_MK7", 2}, {"SMART_SPLITTER_MK2", 2},
    {"INDUSTRIAL_STORAGE_MK2", 2}, {"PIPELINE_MK3", 2}, {"FLUID_BUFFER_MK2", 2},
    {"HV_SUBSTATION", 2}, {"POWER_STORAGE_MK2", 2}, {"HEAVY_REBAR_REPEATER", 2},
    {"HARDENED_REBAR_AMMO", 2}, {"EXOSUIT_MK1", 2}, {"INDUSTRIAL_GARAGE", 2},
    {"REINFORCED_BUILD_SET", 2},

    {"CRYOGENIC_PROCESSING_FACILITY", 3}, {"CRYOGENIC_REFINERY", 3},
    {"GAS_LIQUEFACTION_UNIT", 3}, {"SUPERCONDUCTIVE_ASSEMBLER", 3},
    {"INDUSTRIAL_COOLING_TOWER", 3}, {"CRYOGENIC_PUMP_MK3", 3},
    {"FLUID_BUFFER_MK3", 3}, {"THERMAL_ENERGY_STORAGE", 3}, {"CRYO_SUIT_MK1", 3},
    {"CRYO_PROJECTOR", 3}, {"CRYO_AMMO", 3}, {"CRYO_GRENADE", 3},
    {"JETPACK_MK2", 3}, {"COOLING_LOOP_CONTROLLER", 3},

    {"PHOTONIC_FABRICATOR", 4}, {"OPTICAL_GLASS_FURNACE", 4},
    {"QUANTUM_CIRCUIT_ASSEMBLER", 4}, {"PHOTONIC_COMMUNICATION_TOWER", 4},
    {"MINER_MK5", 4}, {"CONVEYOR_MK8", 4}, {"LIFT_MK8", 4},
    {"SMART_SPLITTER_MK3", 4}, {"DRONE_PORT_MK2", 4},
    {"FACTORY_MONITOR_TERMINAL", 4}, {"HOLOGRAPHIC_DISPLAY", 4},
    {"AUTOMATED_FACTORY_ALARM", 4}, {"PHOTONIC_RIFLE", 4},
    {"PHOTONIC_ENERGY_CELLS", 4}, {"BLUEPRINT_DESIGNER_MK5", 4},
    {"HOVERPACK_MK2", 4}, {"SCOUT_DRONE_MK2", 4}, {"LOGISTICS_CONTROLLER", 4},

    {"GRAVITIC_STABILIZATION_REACTOR", 5}, {"GRAVITIC_ASSEMBLER", 5},
    {"MASS_COMPRESSION_UNIT", 5}, {"MINER_MK6", 5}, {"CONVEYOR_MK9", 5},
    {"LIFT_MK9", 5}, {"GRAVITIC_FREIGHT_ELEVATOR", 5}, {"QUANTUM_MERGER", 5},
    {"QUANTUM_ACCUMULATOR", 5}, {"GRAVITIC_POWER_PLANT", 5}, {"GRAVITY_BOOTS", 5},
    {"PERSONAL_SHIELD_GENERATOR", 5}, {"GRAVITIC_LAUNCHER", 5},
    {"GRAVITY_GRENADE", 5}, {"REPULSION_GRENADE", 5}, {"AUTOMATED_SENTRY_MK2", 5},

    {"ORBITAL_FREIGHT_HUB", 6}, {"QUANTUM_CARGO_RELAY", 6},
    {"LOGISTICS_CONTROL_CENTER", 6}, {"AUTOMATED_LOADING_DOCK", 6},
    {"QUANTUM_STORAGE_CONTAINER", 6}, {"ORBITAL_ASSEMBLY_PLANT", 6},
    {"FICSIT_GLOBAL_DISPATCH", 6}, {"DRONE_PORT_MK3", 6},
    {"TRAIN_LOGISTICS_CONTROLLER", 6}, {"QUANTUM_POWER_RELAY", 6},
    {"ORBITAL_POWER_DISTRIBUTOR", 6}, {"FIELD_FABRICATOR_MK2", 6},
    {"PERSONAL_LOGISTICS_SUIT", 6}, {"ORBITAL_SUPPORT_BEACON", 6},
    {"FICSIT_SECURITY_DRONE", 6}, {"REMOTE_FACTORY_CONTROL", 6},
    {"BLUEPRINT_MANAGEMENT_TERMINAL", 6}
};

} // namespace

EndgameProgression::EndgameProgression(EndgameProgressionSnapshot state)
    : state_(state) {
    normalize();
}

const EndgameProgressionSnapshot& EndgameProgression::state() const noexcept {
    return state_;
}

void EndgameProgression::recordVanillaSpaceElevatorPhase5() noexcept {
    state_.vanillaSpaceElevatorPhase5Complete = true;
}

bool EndgameProgression::validTerminalPhase(int phase) noexcept {
    return phase >= 1 && phase <= 6;
}

bool EndgameProgression::validElevatorPhase(int phase) noexcept {
    return phase >= 6 && phase <= 8;
}

bool EndgameProgression::canResearchTerminalPhase(int phase) const noexcept {
    if (!validTerminalPhase(phase)) return false;
    if (state_.terminalPhasesComplete[static_cast<std::size_t>(phase - 1)]) return false;

    switch (phase) {
    case 1: return state_.vanillaSpaceElevatorPhase5Complete;
    case 2: return state_.terminalPhasesComplete[0];
    case 3: return state_.elevatorExtensionsComplete[0];
    case 4: return state_.terminalPhasesComplete[2];
    case 5: return state_.elevatorExtensionsComplete[1];
    case 6: return state_.elevatorExtensionsComplete[2];
    default: return false;
    }
}

bool EndgameProgression::isTerminalPhaseComplete(int phase) const noexcept {
    return validTerminalPhase(phase)
        && state_.terminalPhasesComplete[static_cast<std::size_t>(phase - 1)];
}

bool EndgameProgression::completeTerminalPhase(int phase) noexcept {
    if (!canResearchTerminalPhase(phase)) return false;
    state_.terminalPhasesComplete[static_cast<std::size_t>(phase - 1)] = true;
    return true;
}

int EndgameProgression::availableTerminalPhase() const noexcept {
    for (int phase = 1; phase <= 6; ++phase) {
        if (canResearchTerminalPhase(phase)) return phase;
    }
    return 0;
}

bool EndgameProgression::canCompleteElevatorPhase(int phase) const noexcept {
    if (!validElevatorPhase(phase)) return false;
    if (state_.elevatorExtensionsComplete[static_cast<std::size_t>(phase - 6)]) return false;

    switch (phase) {
    case 6: return state_.terminalPhasesComplete[1];
    case 7: return state_.elevatorExtensionsComplete[0] && state_.terminalPhasesComplete[3];
    case 8: return state_.elevatorExtensionsComplete[1] && state_.terminalPhasesComplete[4];
    default: return false;
    }
}

bool EndgameProgression::isElevatorPhaseComplete(int phase) const noexcept {
    return validElevatorPhase(phase)
        && state_.elevatorExtensionsComplete[static_cast<std::size_t>(phase - 6)];
}

bool EndgameProgression::completeElevatorPhase(int phase) noexcept {
    if (!canCompleteElevatorPhase(phase)) return false;
    state_.elevatorExtensionsComplete[static_cast<std::size_t>(phase - 6)] = true;
    return true;
}

int EndgameProgression::availableElevatorPhase() const noexcept {
    for (int phase = 6; phase <= 8; ++phase) {
        if (canCompleteElevatorPhase(phase)) return phase;
    }
    return 0;
}

int EndgameProgression::terminalPhaseForUnlock(const std::string& unlockId) noexcept {
    const auto iterator = std::find_if(
        std::begin(kUnlockPhases), std::end(kUnlockPhases),
        [&unlockId](const UnlockPhase& entry) { return unlockId == entry.id; });
    return iterator == std::end(kUnlockPhases) ? 0 : iterator->phase;
}

void EndgameProgression::normalize() noexcept {
    // Terminal phases form a strict sequence. A later saved phase without its
    // predecessor is discarded rather than silently granting missing content.
    for (std::size_t index = 1; index < state_.terminalPhasesComplete.size(); ++index) {
        if (!state_.terminalPhasesComplete[index - 1]) {
            state_.terminalPhasesComplete[index] = false;
        }
    }

    if (!state_.vanillaSpaceElevatorPhase5Complete) {
        state_.terminalPhasesComplete.fill(false);
        state_.elevatorExtensionsComplete.fill(false);
        return;
    }

    if (!state_.terminalPhasesComplete[1]) {
        state_.elevatorExtensionsComplete[0] = false;
    }
    if (!state_.elevatorExtensionsComplete[0]) {
        state_.terminalPhasesComplete[2] = false;
        state_.terminalPhasesComplete[3] = false;
        state_.elevatorExtensionsComplete[1] = false;
    }
    if (!state_.terminalPhasesComplete[3]) {
        state_.elevatorExtensionsComplete[1] = false;
    }
    if (!state_.elevatorExtensionsComplete[1]) {
        state_.terminalPhasesComplete[4] = false;
        state_.elevatorExtensionsComplete[2] = false;
    }
    if (!state_.terminalPhasesComplete[4]) {
        state_.elevatorExtensionsComplete[2] = false;
    }
    if (!state_.elevatorExtensionsComplete[2]) {
        state_.terminalPhasesComplete[5] = false;
    }
}

} // namespace FicsitContracts
