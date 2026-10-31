#pragma once

#include <array>
#include <cstddef>
#include <string>

namespace FicsitContracts {

/**
 * Save-friendly value snapshot for the post-vanilla endgame research sequence.
 *
 * This portable state machine deliberately knows only phase completion. Unreal
 * schematics, item descriptors and Space Elevator callbacks are adapter concerns.
 */
struct EndgameProgressionSnapshot {
    bool vanillaSpaceElevatorPhase5Complete = false;
    std::array<bool, 6> terminalPhasesComplete{};
    std::array<bool, 3> elevatorExtensionsComplete{};
};

class EndgameProgression {
public:
    explicit EndgameProgression(EndgameProgressionSnapshot state = {});

    const EndgameProgressionSnapshot& state() const noexcept;

    /** Record the vanilla Space Elevator finale as completed. */
    void recordVanillaSpaceElevatorPhase5() noexcept;

    /** Terminal research chapters are numbered 1..6. */
    bool canResearchTerminalPhase(int phase) const noexcept;
    bool isTerminalPhaseComplete(int phase) const noexcept;
    bool completeTerminalPhase(int phase) noexcept;
    int availableTerminalPhase() const noexcept;

    /** Custom Space Elevator extensions are numbered 6..8. */
    bool canCompleteElevatorPhase(int phase) const noexcept;
    bool isElevatorPhaseComplete(int phase) const noexcept;
    bool completeElevatorPhase(int phase) noexcept;
    int availableElevatorPhase() const noexcept;

    /** Stable unlock-ID mapping used by UI and schematic adapters. */
    static int terminalPhaseForUnlock(const std::string& unlockId) noexcept;

private:
    EndgameProgressionSnapshot state_;

    void normalize() noexcept;
    static bool validTerminalPhase(int phase) noexcept;
    static bool validElevatorPhase(int phase) noexcept;
};

} // namespace FicsitContracts
