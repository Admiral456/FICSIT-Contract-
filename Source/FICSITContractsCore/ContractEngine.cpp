#include "ContractEngine.h"

#include <algorithm>
#include <cstddef>
#include <iomanip>
#include <initializer_list>
#include <random>
#include <sstream>
#include <utility>

namespace FicsitContracts {
namespace {

struct TemplateRequirement { const char* path; const char* label; int amount; };
struct TemplateStage {
    const char* title;
    std::vector<TemplateRequirement> requirements;
};
struct ContractTemplate {
    const char* id;
    const char* title;
    Difficulty difficulty;
    int reward;
    int minimumReputation;
    bool longTerm;
    int timedWindowSeconds;
    std::vector<TemplateRequirement> requirements;
    std::vector<TemplateStage> stages;
};

TemplateRequirement item(const char* path, const char* label, int amount) {
    return {path, label, amount};
}
TemplateStage stage(const char* title, std::initializer_list<TemplateRequirement> requirements) {
    return {title, std::vector<TemplateRequirement>(requirements)};
}
ContractTemplate order(const char* id, const char* title, Difficulty difficulty, int reward,
                      int minimumReputation, int timedWindowSeconds,
                      std::initializer_list<TemplateRequirement> requirements) {
    return {id, title, difficulty, reward, minimumReputation, false, timedWindowSeconds,
            std::vector<TemplateRequirement>(requirements), {}};
}
ContractTemplate program(const char* id, const char* title, int reward, int minimumReputation,
                         std::initializer_list<TemplateStage> stages) {
    return {id, title, Difficulty::Special, reward, minimumReputation, true, 0, {}, std::vector<TemplateStage>(stages)};
}

const std::vector<ContractTemplate>& templates() {
    // A deliberately broader catalog than the original one-item delivery board:
    // basic stock requests, multi-item production batches, urgent recovery orders
    // and long multi-phase factory programs. All item paths are vanilla descriptors.
    static const std::vector<ContractTemplate> value = {
        order("SUP-IRON-PLATE", "Iron Plate Reserve", Difficulty::Small, 35, 0, 1200, {
            item("/Game/FactoryGame/Resource/Parts/IronPlate/Desc_IronPlate.Desc_IronPlate_C", "Iron Plates", 500)
        }),
        order("SUP-IRON-ROD", "Rod Supply", Difficulty::Small, 30, 0, 1200, {
            item("/Game/FactoryGame/Resource/Parts/IronRod/Desc_IronRod.Desc_IronRod_C", "Iron Rods", 400)
        }),
        order("SUP-WIRE", "Wire Shipment", Difficulty::Small, 30, 0, 1200, {
            item("/Game/FactoryGame/Resource/Parts/Wire/Desc_Wire.Desc_Wire_C", "Wire", 600)
        }),
        order("SUP-CABLE", "Cable Shipment", Difficulty::Small, 45, 0, 1200, {
            item("/Game/FactoryGame/Resource/Parts/Cable/Desc_Cable.Desc_Cable_C", "Cable", 250)
        }),
        order("SUP-CONCRETE", "Foundation Concrete Batch", Difficulty::Small, 50, 0, 1500, {
            item("/Game/FactoryGame/Resource/Parts/Concrete/Desc_Concrete.Desc_Concrete_C", "Concrete", 500)
        }),

        order("SUP-RIP", "Reinforced Plate Order", Difficulty::Medium, 130, 150, 1800, {
            item("/Game/FactoryGame/Resource/Parts/IronPlateReinforced/Desc_IronPlateReinforced.Desc_IronPlateReinforced_C", "Reinforced Iron Plates", 120)
        }),
        order("SUP-ROTOR", "Rotor Replacement Stock", Difficulty::Medium, 150, 150, 1800, {
            item("/Game/FactoryGame/Resource/Parts/Rotor/Desc_Rotor.Desc_Rotor_C", "Rotors", 60)
        }),
        order("SUP-COPPER-CIRCUIT", "Copper Circuit Batch", Difficulty::Medium, 165, 150, 2100, {
            item("/Game/FactoryGame/Resource/Parts/Wire/Desc_Wire.Desc_Wire_C", "Wire", 900),
            item("/Game/FactoryGame/Resource/Parts/Cable/Desc_Cable.Desc_Cable_C", "Cable", 180)
        }),
        order("SUP-FOUNDATION-KIT", "Foundation Starter Kit", Difficulty::Medium, 175, 150, 2100, {
            item("/Game/FactoryGame/Resource/Parts/Concrete/Desc_Concrete.Desc_Concrete_C", "Concrete", 700),
            item("/Game/FactoryGame/Resource/Parts/IronPlate/Desc_IronPlate.Desc_IronPlate_C", "Iron Plates", 500)
        }),

        order("SUP-MOTOR", "Motor Delivery", Difficulty::Large, 350, 500, 2400, {
            item("/Game/FactoryGame/Resource/Parts/Motor/Desc_Motor.Desc_Motor_C", "Motors", 150)
        }),
        order("SUP-MODULAR-FRAME", "Structural Frame Order", Difficulty::Large, 280, 500, 2400, {
            item("/Game/FactoryGame/Resource/Parts/ModularFrame/Desc_ModularFrame.Desc_ModularFrame_C", "Modular Frames", 100)
        }),
        order("SUP-STEELWORKS", "Steelworks Pipeline", Difficulty::Large, 360, 500, 2700, {
            item("/Game/FactoryGame/Resource/Parts/SteelPipe/Desc_SteelPipe.Desc_SteelPipe_C", "Steel Pipes", 450),
            item("/Game/FactoryGame/Resource/Parts/SteelPlate/Desc_SteelPlate.Desc_SteelPlate_C", "Steel Beams", 180)
        }),
        order("SUP-POWER-DISTRIBUTION", "Power Distribution Package", Difficulty::Large, 420, 500, 2700, {
            item("/Game/FactoryGame/Resource/Parts/Cable/Desc_Cable.Desc_Cable_C", "Cable", 250),
            item("/Game/FactoryGame/Resource/Parts/Stator/Desc_Stator.Desc_Stator_C", "Stators", 100),
            item("/Game/FactoryGame/Resource/Parts/Motor/Desc_Motor.Desc_Motor_C", "Motors", 45)
        }),
        order("SUP-ENCASED-BEAMS", "Heavy Foundation Beam Shipment", Difficulty::Large, 390, 500, 2700, {
            item("/Game/FactoryGame/Resource/Parts/EncasedIndustrialBeam/Desc_EncasedIndustrialBeam.Desc_EncasedIndustrialBeam_C", "Encased Industrial Beams", 180),
            item("/Game/FactoryGame/Resource/Parts/SteelPipe/Desc_SteelPipe.Desc_SteelPipe_C", "Steel Pipes", 250)
        }),
        order("URGENT-GRID-RECOVERY", "Emergency Grid Recovery", Difficulty::Large, 520, 750, 1200, {
            item("/Game/FactoryGame/Resource/Parts/Cable/Desc_Cable.Desc_Cable_C", "Cable", 180),
            item("/Game/FactoryGame/Resource/Parts/Motor/Desc_Motor.Desc_Motor_C", "Motors", 35),
            item("/Game/FactoryGame/Resource/Parts/SteelPipe/Desc_SteelPipe.Desc_SteelPipe_C", "Steel Pipes", 120)
        }),

        program("SPECIAL-FACTORY-BACKBONE", "Factory Backbone Program", 1100, 1000, {
            stage("01 - Structural Base", {
                item("/Game/FactoryGame/Resource/Parts/IronPlate/Desc_IronPlate.Desc_IronPlate_C", "Iron Plates", 2000),
                item("/Game/FactoryGame/Resource/Parts/Concrete/Desc_Concrete.Desc_Concrete_C", "Concrete", 800)
            }),
            stage("02 - Reinforced Superstructure", {
                item("/Game/FactoryGame/Resource/Parts/IronPlateReinforced/Desc_IronPlateReinforced.Desc_IronPlateReinforced_C", "Reinforced Iron Plates", 500),
                item("/Game/FactoryGame/Resource/Parts/ModularFrame/Desc_ModularFrame.Desc_ModularFrame_C", "Modular Frames", 150)
            }),
            stage("03 - Powertrain Assembly", {
                item("/Game/FactoryGame/Resource/Parts/Motor/Desc_Motor.Desc_Motor_C", "Motors", 150),
                item("/Game/FactoryGame/Resource/Parts/Rotor/Desc_Rotor.Desc_Rotor_C", "Rotors", 120),
                item("/Game/FactoryGame/Resource/Parts/Stator/Desc_Stator.Desc_Stator_C", "Stators", 80)
            }),
            stage("04 - Control and Commissioning", {
                item("/Game/FactoryGame/Resource/Parts/Computer/Desc_Computer.Desc_Computer_C", "Computers", 80),
                item("/Game/FactoryGame/Resource/Parts/CircuitBoard/Desc_CircuitBoard.Desc_CircuitBoard_C", "Circuit Boards", 100)
            })
        }),
        program("SPECIAL-FOUNDRY-EXPANSION", "Foundry Expansion Initiative", 1350, 1000, {
            stage("01 - Site Preparation", {
                item("/Game/FactoryGame/Resource/Parts/Concrete/Desc_Concrete.Desc_Concrete_C", "Concrete", 1400),
                item("/Game/FactoryGame/Resource/Parts/IronPlate/Desc_IronPlate.Desc_IronPlate_C", "Iron Plates", 1200)
            }),
            stage("02 - Steel Production Lines", {
                item("/Game/FactoryGame/Resource/Parts/SteelPipe/Desc_SteelPipe.Desc_SteelPipe_C", "Steel Pipes", 650),
                item("/Game/FactoryGame/Resource/Parts/SteelPlate/Desc_SteelPlate.Desc_SteelPlate_C", "Steel Beams", 350)
            }),
            stage("03 - Reinforced Assembly", {
                item("/Game/FactoryGame/Resource/Parts/IronPlateReinforced/Desc_IronPlateReinforced.Desc_IronPlateReinforced_C", "Reinforced Iron Plates", 450),
                item("/Game/FactoryGame/Resource/Parts/EncasedIndustrialBeam/Desc_EncasedIndustrialBeam.Desc_EncasedIndustrialBeam_C", "Encased Industrial Beams", 180)
            }),
            stage("04 - Foundry Powertrain", {
                item("/Game/FactoryGame/Resource/Parts/Motor/Desc_Motor.Desc_Motor_C", "Motors", 150),
                item("/Game/FactoryGame/Resource/Parts/Stator/Desc_Stator.Desc_Stator_C", "Stators", 120)
            })
        }),
        program("SPECIAL-AUTOMATION-ROLLOUT", "Automated Assembly Rollout", 1650, 1500, {
            stage("01 - Machine Chassis", {
                item("/Game/FactoryGame/Resource/Parts/ModularFrame/Desc_ModularFrame.Desc_ModularFrame_C", "Modular Frames", 300),
                item("/Game/FactoryGame/Resource/Parts/IronPlateReinforced/Desc_IronPlateReinforced.Desc_IronPlateReinforced_C", "Reinforced Iron Plates", 500)
            }),
            stage("02 - Motion Systems", {
                item("/Game/FactoryGame/Resource/Parts/Rotor/Desc_Rotor.Desc_Rotor_C", "Rotors", 250),
                item("/Game/FactoryGame/Resource/Parts/Stator/Desc_Stator.Desc_Stator_C", "Stators", 250)
            }),
            stage("03 - Drive Assembly", {
                item("/Game/FactoryGame/Resource/Parts/Motor/Desc_Motor.Desc_Motor_C", "Motors", 180)
            }),
            stage("04 - Control Electronics", {
                item("/Game/FactoryGame/Resource/Parts/CircuitBoard/Desc_CircuitBoard.Desc_CircuitBoard_C", "Circuit Boards", 180),
                item("/Game/FactoryGame/Resource/Parts/Computer/Desc_Computer.Desc_Computer_C", "Computers", 90),
                item("/Game/FactoryGame/Resource/Parts/Cable/Desc_Cable.Desc_Cable_C", "Cable", 500)
            }),
            stage("05 - Heavy Commissioning", {
                item("/Game/FactoryGame/Resource/Parts/ModularFrameHeavy/Desc_ModularFrameHeavy.Desc_ModularFrameHeavy_C", "Heavy Modular Frames", 70),
                item("/Game/FactoryGame/Resource/Parts/EncasedIndustrialBeam/Desc_EncasedIndustrialBeam.Desc_EncasedIndustrialBeam_C", "Encased Industrial Beams", 150)
            })
        }),
        program("SPECIAL-POWER-GRID", "Planetary Power Grid Program", 1850, 1500, {
            stage("01 - Cable Trunk", {
                item("/Game/FactoryGame/Resource/Parts/Cable/Desc_Cable.Desc_Cable_C", "Cable", 1200),
                item("/Game/FactoryGame/Resource/Parts/Wire/Desc_Wire.Desc_Wire_C", "Wire", 2400)
            }),
            stage("02 - Transformer Frames", {
                item("/Game/FactoryGame/Resource/Parts/SteelPipe/Desc_SteelPipe.Desc_SteelPipe_C", "Steel Pipes", 600),
                item("/Game/FactoryGame/Resource/Parts/ModularFrame/Desc_ModularFrame.Desc_ModularFrame_C", "Modular Frames", 220)
            }),
            stage("03 - Generation Hardware", {
                item("/Game/FactoryGame/Resource/Parts/Stator/Desc_Stator.Desc_Stator_C", "Stators", 220),
                item("/Game/FactoryGame/Resource/Parts/Motor/Desc_Motor.Desc_Motor_C", "Motors", 140)
            }),
            stage("04 - Grid Control", {
                item("/Game/FactoryGame/Resource/Parts/CircuitBoard/Desc_CircuitBoard.Desc_CircuitBoard_C", "Circuit Boards", 220),
                item("/Game/FactoryGame/Resource/Parts/Computer/Desc_Computer.Desc_Computer_C", "Computers", 120)
            }),
            stage("05 - Final Load Test", {
                item("/Game/FactoryGame/Resource/Parts/ModularFrameHeavy/Desc_ModularFrameHeavy.Desc_ModularFrameHeavy_C", "Heavy Modular Frames", 80),
                item("/Game/FactoryGame/Resource/Parts/EncasedIndustrialBeam/Desc_EncasedIndustrialBeam.Desc_EncasedIndustrialBeam_C", "Encased Industrial Beams", 220)
            })
        }),
        program("SPECIAL-PLANETARY-LOGISTICS", "Planetary Logistics Network", 2600, 2000, {
            stage("01 - Foundation Corridor", {
                item("/Game/FactoryGame/Resource/Parts/Concrete/Desc_Concrete.Desc_Concrete_C", "Concrete", 2500),
                item("/Game/FactoryGame/Resource/Parts/IronPlate/Desc_IronPlate.Desc_IronPlate_C", "Iron Plates", 2200)
            }),
            stage("02 - Freight Structures", {
                item("/Game/FactoryGame/Resource/Parts/SteelPipe/Desc_SteelPipe.Desc_SteelPipe_C", "Steel Pipes", 1200),
                item("/Game/FactoryGame/Resource/Parts/SteelPlate/Desc_SteelPlate.Desc_SteelPlate_C", "Steel Beams", 700),
                item("/Game/FactoryGame/Resource/Parts/EncasedIndustrialBeam/Desc_EncasedIndustrialBeam.Desc_EncasedIndustrialBeam_C", "Encased Industrial Beams", 350)
            }),
            stage("03 - Reinforced Platforms", {
                item("/Game/FactoryGame/Resource/Parts/IronPlateReinforced/Desc_IronPlateReinforced.Desc_IronPlateReinforced_C", "Reinforced Iron Plates", 900),
                item("/Game/FactoryGame/Resource/Parts/ModularFrame/Desc_ModularFrame.Desc_ModularFrame_C", "Modular Frames", 400)
            }),
            stage("04 - Mechanical Handling", {
                item("/Game/FactoryGame/Resource/Parts/Rotor/Desc_Rotor.Desc_Rotor_C", "Rotors", 450),
                item("/Game/FactoryGame/Resource/Parts/Stator/Desc_Stator.Desc_Stator_C", "Stators", 350),
                item("/Game/FactoryGame/Resource/Parts/Motor/Desc_Motor.Desc_Motor_C", "Motors", 250)
            }),
            stage("05 - Dispatch Electronics", {
                item("/Game/FactoryGame/Resource/Parts/CircuitBoard/Desc_CircuitBoard.Desc_CircuitBoard_C", "Circuit Boards", 350),
                item("/Game/FactoryGame/Resource/Parts/Computer/Desc_Computer.Desc_Computer_C", "Computers", 220),
                item("/Game/FactoryGame/Resource/Parts/Cable/Desc_Cable.Desc_Cable_C", "Cable", 1000)
            }),
            stage("06 - Heavy Freight Commissioning", {
                item("/Game/FactoryGame/Resource/Parts/ModularFrameHeavy/Desc_ModularFrameHeavy.Desc_ModularFrameHeavy_C", "Heavy Modular Frames", 160)
            })
        }),
        program("SPECIAL-ORBITAL-LOGISTICS", "Orbital Logistics Authority Program", 4200, 3000, {
            stage("01 - Planetary Foundations", {
                item("/Game/FactoryGame/Resource/Parts/Concrete/Desc_Concrete.Desc_Concrete_C", "Concrete", 3000),
                item("/Game/FactoryGame/Resource/Parts/IronPlate/Desc_IronPlate.Desc_IronPlate_C", "Iron Plates", 3000)
            }),
            stage("02 - Long-Haul Steelworks", {
                item("/Game/FactoryGame/Resource/Parts/SteelPipe/Desc_SteelPipe.Desc_SteelPipe_C", "Steel Pipes", 1800),
                item("/Game/FactoryGame/Resource/Parts/SteelPlate/Desc_SteelPlate.Desc_SteelPlate_C", "Steel Beams", 1200)
            }),
            stage("03 - Reinforced Launch Structures", {
                item("/Game/FactoryGame/Resource/Parts/EncasedIndustrialBeam/Desc_EncasedIndustrialBeam.Desc_EncasedIndustrialBeam_C", "Encased Industrial Beams", 650),
                item("/Game/FactoryGame/Resource/Parts/IronPlateReinforced/Desc_IronPlateReinforced.Desc_IronPlateReinforced_C", "Reinforced Iron Plates", 1200)
            }),
            stage("04 - Heavy Structural Modules", {
                item("/Game/FactoryGame/Resource/Parts/ModularFrame/Desc_ModularFrame.Desc_ModularFrame_C", "Modular Frames", 800),
                item("/Game/FactoryGame/Resource/Parts/ModularFrameHeavy/Desc_ModularFrameHeavy.Desc_ModularFrameHeavy_C", "Heavy Modular Frames", 250)
            }),
            stage("05 - Precision Mechanical Systems", {
                item("/Game/FactoryGame/Resource/Parts/Rotor/Desc_Rotor.Desc_Rotor_C", "Rotors", 650),
                item("/Game/FactoryGame/Resource/Parts/Stator/Desc_Stator.Desc_Stator_C", "Stators", 550),
                item("/Game/FactoryGame/Resource/Parts/Motor/Desc_Motor.Desc_Motor_C", "Motors", 450)
            }),
            stage("06 - Advanced Dispatch Electronics", {
                item("/Game/FactoryGame/Resource/Parts/CircuitBoard/Desc_CircuitBoard.Desc_CircuitBoard_C", "Circuit Boards", 750),
                item("/Game/FactoryGame/Resource/Parts/Computer/Desc_Computer.Desc_Computer_C", "Computers", 550),
                item("/Game/FactoryGame/Resource/Parts/Cable/Desc_Cable.Desc_Cable_C", "Cable", 1800)
            }),
            stage("07 - Authority Handover", {
                item("/Game/FactoryGame/Resource/Parts/ModularFrameHeavy/Desc_ModularFrameHeavy.Desc_ModularFrameHeavy_C", "Heavy Modular Frames", 180),
                item("/Game/FactoryGame/Resource/Parts/Motor/Desc_Motor.Desc_Motor_C", "Motors", 200)
            })
        })
    };
    return value;
}

bool requirementsComplete(const std::vector<Requirement>& requirements) {
    if (requirements.empty()) return false;
    return std::all_of(requirements.begin(), requirements.end(),
        [](const Requirement& requirement) {
            return requirement.requiredAmount > 0 && requirement.deliveredAmount >= requirement.requiredAmount;
        });
}
} // namespace

ContractEngine::ContractEngine(Snapshot state) : state_(std::move(state)) {
    if (state_.nextSerial == 0) state_.nextSerial = 1;
    if (state_.reputation < 0) state_.reputation = 0;

    // Be defensive when loading data written by an older or damaged save.
    const auto normalize = [](std::vector<Contract>& contracts) {
        for (Contract& contract : contracts) {
            const auto normalizeRequirements = [](std::vector<Requirement>& requirements) {
                for (Requirement& requirement : requirements) {
                    requirement.requiredAmount = std::max(0, requirement.requiredAmount);
                    requirement.deliveredAmount = std::clamp(requirement.deliveredAmount, 0, requirement.requiredAmount);
                }
            };
            normalizeRequirements(contract.requirements);
            for (ContractStage& stage : contract.stages) normalizeRequirements(stage.requirements);
            if (contract.stages.empty()) contract.currentStageIndex = 0;
            else contract.currentStageIndex = std::min(contract.currentStageIndex, contract.stages.size() - 1);
        }
    };
    normalize(state_.offers);
    normalize(state_.active);
    normalize(state_.history);

    constexpr std::size_t HistoryLimit = 50;
    if (state_.history.size() > HistoryLimit) {
        state_.history.erase(state_.history.begin(),
            state_.history.begin() + static_cast<std::ptrdiff_t>(state_.history.size() - HistoryLimit));
    }
}
const Snapshot& ContractEngine::state() const noexcept { return state_; }
Snapshot& ContractEngine::state() noexcept { return state_; }

std::string ContractEngine::makeId() {
    std::ostringstream stream;
    stream << "FC-" << std::setw(6) << std::setfill('0') << state_.nextSerial++;
    return stream.str();
}
void ContractEngine::pushHistory(Contract contract) {
    state_.history.push_back(std::move(contract));
    constexpr std::size_t HistoryLimit = 50;
    if (state_.history.size() > HistoryLimit)
        state_.history.erase(state_.history.begin(), state_.history.begin() + static_cast<std::ptrdiff_t>(state_.history.size() - HistoryLimit));
}

void ContractEngine::ensureOffers(std::size_t targetCount, std::int64_t nowUnix, std::uint32_t seed) {
    expire(nowUnix);
    if (targetCount > 10) targetCount = 10;
    std::mt19937 random(seed);
    const auto& catalog = templates();
    std::vector<const ContractTemplate*> candidates;
    candidates.reserve(catalog.size());
    for (const auto& entry : catalog) if (entry.minimumReputation <= state_.reputation) candidates.push_back(&entry);
    std::shuffle(candidates.begin(), candidates.end(), random);

    std::size_t candidateIndex = 0;
    while (state_.offers.size() < targetCount && candidateIndex < candidates.size()) {
        const ContractTemplate& entry = *candidates[candidateIndex++];
        const auto alreadyPresent = [&entry](const Contract& contract) { return contract.templateId == entry.id; };
        if (std::any_of(state_.offers.begin(), state_.offers.end(), alreadyPresent) ||
            std::any_of(state_.active.begin(), state_.active.end(), alreadyPresent)) continue;

        Contract contract;
        contract.id = makeId(); contract.templateId = entry.id; contract.title = entry.title;
        contract.difficulty = entry.difficulty; contract.rewardReputation = entry.reward; contract.longTerm = entry.longTerm;
        contract.timeLimited = !entry.longTerm && entry.timedWindowSeconds > 0 && (random() % 4U == 0U);
        contract.expiresAtUnix = contract.timeLimited ? nowUnix + entry.timedWindowSeconds : 0;
        if (!entry.stages.empty()) {
            for (const TemplateStage& sourceStage : entry.stages) {
                ContractStage targetStage;
                targetStage.title = sourceStage.title;
                for (const auto& requirement : sourceStage.requirements)
                    targetStage.requirements.push_back({requirement.path, requirement.label, requirement.amount, 0});
                contract.stages.push_back(std::move(targetStage));
            }
        } else {
            for (const auto& requirement : entry.requirements)
                contract.requirements.push_back({requirement.path, requirement.label, requirement.amount, 0});
        }
        state_.offers.push_back(std::move(contract));
    }
}

void ContractEngine::expire(std::int64_t nowUnix) {
    const auto expireList = [this, nowUnix](std::vector<Contract>& contracts) {
        auto iterator = contracts.begin();
        while (iterator != contracts.end()) {
            if (iterator->expiresAtUnix > 0 && nowUnix >= iterator->expiresAtUnix) {
                iterator->expired = true; iterator->completed = false; iterator->finishedAtUnix = nowUnix;
                pushHistory(std::move(*iterator)); iterator = contracts.erase(iterator);
            } else ++iterator;
        }
    };
    expireList(state_.offers); expireList(state_.active);
}

bool ContractEngine::accept(const std::string& contractId, std::int64_t nowUnix) {
    expire(nowUnix);
    if (state_.active.size() >= static_cast<std::size_t>(activeContractLimit())) return false;
    const auto iterator = std::find_if(state_.offers.begin(), state_.offers.end(),
        [&contractId](const Contract& contract) { return contract.id == contractId; });
    if (iterator == state_.offers.end()) return false;
    state_.active.push_back(std::move(*iterator)); state_.offers.erase(iterator); return true;
}

DeliveryResult ContractEngine::deliver(const std::string& contractId, const std::string& itemClassPath, int amount, std::int64_t nowUnix) {
    DeliveryResult result;
    if (amount <= 0 || itemClassPath.empty()) return result;
    expire(nowUnix);
    auto contractIterator = std::find_if(state_.active.begin(), state_.active.end(),
        [&contractId](const Contract& contract) { return contract.id == contractId; });
    if (contractIterator == state_.active.end()) return result;
    result.contractFound = true;
    std::vector<Requirement>& requirements = currentRequirements(*contractIterator);
    auto requirementIterator = std::find_if(requirements.begin(), requirements.end(),
        [&itemClassPath](const Requirement& requirement) { return requirement.itemClassPath == itemClassPath; });
    if (requirementIterator == requirements.end()) return result;
    result.requirementFound = true;
    const int remaining = std::max(0, requirementIterator->requiredAmount - requirementIterator->deliveredAmount);
    result.acceptedAmount = std::min(amount, remaining);
    requirementIterator->deliveredAmount += result.acceptedAmount;

    if (requirementsComplete(requirements)) {
        result.stageCompleted = true;
        result.completedStageIndex = static_cast<int>(contractIterator->currentStageIndex);
        result.stageCount = static_cast<int>(stageCount(*contractIterator));

        if (!contractIterator->stages.empty() &&
            contractIterator->currentStageIndex + 1 < contractIterator->stages.size()) {
            ++contractIterator->currentStageIndex;
            result.nextStageIndex = static_cast<int>(contractIterator->currentStageIndex);
        } else {
            Contract completed = std::move(*contractIterator);
            completed.completed = true; completed.expired = false; completed.finishedAtUnix = nowUnix;
            result.reputationAwarded = completed.rewardReputation;
            state_.reputation += completed.rewardReputation; result.completed = true;
            result.nextStageIndex = result.completedStageIndex;
            state_.active.erase(contractIterator); pushHistory(std::move(completed));
        }
    }
    return result;
}

AbandonResult ContractEngine::abandon(const std::string& contractId, std::int64_t nowUnix) {
    AbandonResult result;
    expire(nowUnix);

    auto iterator = std::find_if(state_.active.begin(), state_.active.end(),
        [&contractId](const Contract& contract) { return contract.id == contractId; });
    if (iterator == state_.active.end()) return result;

    result.contractFound = true;
    const int reward = std::max(0, iterator->rewardReputation);
    // Cancelling is deliberately costly enough to discourage offer rerolling.
    // Round the 25% fee up and never charge more reputation than the player has.
    const int requestedPenalty = reward > 0 ? std::max(5, reward / 4 + (reward % 4 != 0 ? 1 : 0)) : 0;
    result.reputationPenalty = std::min(state_.reputation, requestedPenalty);
    state_.reputation -= result.reputationPenalty;

    Contract abandoned = std::move(*iterator);
    abandoned.completed = false;
    abandoned.expired = false;
    abandoned.abandoned = true;
    abandoned.finishedAtUnix = nowUnix;
    state_.active.erase(iterator);
    pushHistory(std::move(abandoned));
    return result;
}

int ContractEngine::progressPercent(const Contract& contract) noexcept {
    const auto requirementsPercent = [](const std::vector<Requirement>& requirements) {
        std::int64_t required = 0;
        std::int64_t delivered = 0;
        for (const Requirement& requirement : requirements) {
            required += std::max(0, requirement.requiredAmount);
            delivered += std::min(std::max(0, requirement.deliveredAmount), std::max(0, requirement.requiredAmount));
        }
        return required > 0 ? static_cast<int>((delivered * 100) / required) : 0;
    };
    if (contract.stages.empty()) return requirementsPercent(contract.requirements);

    std::int64_t stagePercentTotal = 0;
    for (const ContractStage& stage : contract.stages)
        stagePercentTotal += requirementsPercent(stage.requirements);
    return static_cast<int>(stagePercentTotal / static_cast<std::int64_t>(contract.stages.size()));
}

std::vector<Requirement>& ContractEngine::currentRequirements(Contract& contract) noexcept {
    static std::vector<Requirement> empty;
    if (contract.stages.empty()) return contract.requirements;
    if (contract.currentStageIndex >= contract.stages.size()) return empty;
    return contract.stages[contract.currentStageIndex].requirements;
}

const std::vector<Requirement>& ContractEngine::currentRequirements(const Contract& contract) noexcept {
    static const std::vector<Requirement> empty;
    if (contract.stages.empty()) return contract.requirements;
    if (contract.currentStageIndex >= contract.stages.size()) return empty;
    return contract.stages[contract.currentStageIndex].requirements;
}

const std::string& ContractEngine::currentStageTitle(const Contract& contract) noexcept {
    static const std::string empty;
    if (contract.stages.empty() || contract.currentStageIndex >= contract.stages.size()) return empty;
    return contract.stages[contract.currentStageIndex].title;
}

std::size_t ContractEngine::stageCount(const Contract& contract) noexcept {
    return contract.stages.empty() ? 1U : contract.stages.size();
}
const char* ContractEngine::rankName(int reputation) noexcept {
    if (reputation >= 3000) return "FICSIT Executive";
    if (reputation >= 2000) return "Supply Chain Director";
    if (reputation >= 1500) return "Regional Logistics Director";
    if (reputation >= 1000) return "FICSIT Coordinator";
    if (reputation >= 500) return "Logistics Specialist";
    if (reputation >= 150) return "Logistics Associate";
    return "FICSIT Trainee";
}
int ContractEngine::activeContractLimit() noexcept { return 4; }

} // namespace FicsitContracts
