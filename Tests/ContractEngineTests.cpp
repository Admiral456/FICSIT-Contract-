#include "ContractEngine.h"

#include <cstdlib>
#include <iostream>
#include <set>
#include <string>

using namespace FicsitContracts;
namespace {
int failures = 0;
void expect(bool condition, const char* message) {
    if (!condition) { ++failures; std::cerr << "FAIL: " << message << '\n'; }
}
}

int main() {
    constexpr std::int64_t now = 1'800'000'000;
    ContractEngine engine;
    engine.ensureOffers(3, now, 12345);
    expect(engine.state().offers.size() == 3, "new board has three offers");
    expect(engine.state().reputation == 0, "new board starts at zero reputation");

    std::set<std::string> initialTemplates;
    for (const auto& offer : engine.state().offers) initialTemplates.insert(offer.templateId);
    expect(initialTemplates.size() == engine.state().offers.size(), "the same template is not listed twice");

    const auto first = engine.state().offers.front();
    expect(engine.accept(first.id, now), "valid offer can be accepted");
    expect(engine.state().active.size() == 1, "accepted contract becomes active");
    expect(engine.state().offers.size() == 2, "accepted contract is removed from offers");
    expect(!engine.accept("FC-DOES-NOT-EXIST", now), "unknown offer cannot be accepted");

    const auto active = engine.state().active.front();
    const auto requirement = active.requirements.front();
    const int half = requirement.requiredAmount / 2;
    const auto zeroDelivery = engine.deliver(active.id, requirement.itemClassPath, 0, now + 1);
    const auto negativeDelivery = engine.deliver(active.id, requirement.itemClassPath, -10, now + 1);
    const auto wrongItemDelivery = engine.deliver(active.id, "/not/a/required/item", 10, now + 1);
    expect(zeroDelivery.acceptedAmount == 0 && negativeDelivery.acceptedAmount == 0, "zero and negative deliveries are rejected");
    expect(wrongItemDelivery.acceptedAmount == 0 && !wrongItemDelivery.requirementFound, "unrequested items are rejected");
    expect(engine.state().active.front().requirements.front().deliveredAmount == 0, "invalid deliveries do not mutate progress");

    auto partial = engine.deliver(active.id, requirement.itemClassPath, half, now + 2);
    expect(partial.contractFound && partial.requirementFound, "delivery locates contract and item requirement");
    expect(partial.acceptedAmount == half, "partial delivery accepts only supplied amount");
    expect(!partial.completed, "partial delivery does not complete contract");
    expect(ContractEngine::progressPercent(engine.state().active.front()) < 100, "partial delivery progress remains below 100 percent");

    // A save/load round trip is represented by serializing the value snapshot and rebuilding the engine.
    ContractEngine restoredEngine(engine.state());
    expect(restoredEngine.state().active.size() == 1, "active contracts survive a snapshot restore");
    expect(restoredEngine.state().active.front().requirements.front().deliveredAmount == half, "partial progress survives a snapshot restore");
    expect(restoredEngine.state().nextSerial == engine.state().nextSerial, "contract serial survives a snapshot restore");

    auto rest = restoredEngine.deliver(active.id, requirement.itemClassPath, requirement.requiredAmount * 10, now + 3);
    expect(rest.acceptedAmount == requirement.requiredAmount - half, "excess is clamped to remaining amount");
    expect(rest.completed, "fully supplied contract completes");
    expect(rest.reputationAwarded == active.rewardReputation, "completion pays configured reputation exactly once");
    expect(restoredEngine.state().active.empty(), "completed contract leaves active list");
    expect(restoredEngine.state().reputation == active.rewardReputation, "reputation is added on completion");
    expect(restoredEngine.state().history.size() == 1 && restoredEngine.state().history.back().completed, "completed contract is recorded in history");
    expect(!restoredEngine.deliver(active.id, requirement.itemClassPath, requirement.requiredAmount, now + 4).contractFound, "completed contract cannot be paid twice");
    expect(restoredEngine.state().reputation == active.rewardReputation, "duplicate delivery cannot award reputation twice");

    Snapshot advancedState; advancedState.reputation = 1000;
    ContractEngine advancedEngine(advancedState);
    advancedEngine.ensureOffers(10, now, 555);
    expect(advancedEngine.state().offers.size() == 10, "five-offer advanced board can fill ten test slots");
    std::set<std::string> advancedTemplates;
    for (const auto& offer : advancedEngine.state().offers) advancedTemplates.insert(offer.templateId);
    expect(advancedTemplates.size() == advancedEngine.state().offers.size(), "expanded catalog never duplicates a template on the board");

    const auto findProgram = [&](int reputation, const std::string& templateId, Snapshot* outBoard = nullptr) {
        Snapshot startingState;
        startingState.reputation = reputation;
        for (std::uint32_t seed = 1; seed <= 400; ++seed) {
            ContractEngine probe(startingState);
            probe.ensureOffers(10, now, seed);
            for (const Contract& offer : probe.state().offers) {
                if (offer.templateId == templateId) {
                    if (outBoard) *outBoard = probe.state();
                    return offer;
                }
            }
        }
        return Contract{};
    };

    Snapshot backboneBoard;
    const Contract stagedOffer = findProgram(1000, "SPECIAL-FACTORY-BACKBONE", &backboneBoard);
    expect(!stagedOffer.id.empty(), "Factory Backbone is generated at its reputation gate");
    expect(stagedOffer.stages.size() == 4, "Factory Backbone is expanded into four sequential phases");
    expect(stagedOffer.stages.front().requirements.size() == 2, "each phase can require several different materials");
    expect(findProgram(1000, "SPECIAL-FOUNDRY-EXPANSION").stages.size() == 4,
        "Foundry Expansion is a four-phase industrial megaproject");
    expect(findProgram(1500, "SPECIAL-AUTOMATION-ROLLOUT").stages.size() == 5,
        "Automation Rollout is a five-phase production program");
    expect(findProgram(1500, "SPECIAL-POWER-GRID").stages.size() == 5,
        "Planetary Power Grid is a five-phase infrastructure program");
    expect(findProgram(2000, "SPECIAL-PLANETARY-LOGISTICS").stages.size() == 6,
        "Planetary Logistics is a six-phase endgame megaproject");
    expect(findProgram(3000, "SPECIAL-ORBITAL-LOGISTICS").stages.size() == 7,
        "Orbital Logistics Authority is a seven-phase executive megaproject");

    ContractEngine chainEngine(backboneBoard);
    expect(chainEngine.accept(stagedOffer.id, now), "multi-stage industrial program can be accepted");
    const std::string ironPlatePath = "/Game/FactoryGame/Resource/Parts/IronPlate/Desc_IronPlate.Desc_IronPlate_C";
    const std::string concretePath = "/Game/FactoryGame/Resource/Parts/Concrete/Desc_Concrete.Desc_Concrete_C";
    const std::string reinforcedPath = "/Game/FactoryGame/Resource/Parts/IronPlateReinforced/Desc_IronPlateReinforced.Desc_IronPlateReinforced_C";
    const std::string modularFramePath = "/Game/FactoryGame/Resource/Parts/ModularFrame/Desc_ModularFrame.Desc_ModularFrame_C";
    const std::string motorPath = "/Game/FactoryGame/Resource/Parts/Motor/Desc_Motor.Desc_Motor_C";
    const std::string rotorPath = "/Game/FactoryGame/Resource/Parts/Rotor/Desc_Rotor.Desc_Rotor_C";
    const std::string statorPath = "/Game/FactoryGame/Resource/Parts/Stator/Desc_Stator.Desc_Stator_C";
    const std::string computerPath = "/Game/FactoryGame/Resource/Parts/Computer/Desc_Computer.Desc_Computer_C";
    const std::string circuitBoardPath = "/Game/FactoryGame/Resource/Parts/CircuitBoard/Desc_CircuitBoard.Desc_CircuitBoard_C";

    const auto partialStageDelivery = chainEngine.deliver(stagedOffer.id, ironPlatePath, 2000, now + 1);
    expect(partialStageDelivery.acceptedAmount == 2000 && !partialStageDelivery.stageCompleted,
        "delivering one required material does not bypass the remaining stage manifest");
    expect(chainEngine.state().active.front().currentStageIndex == 0,
        "the next phase remains locked while one current-phase material is outstanding");
    ContractEngine partialChainRestore(chainEngine.state());
    expect(partialChainRestore.state().active.front().stages.front().requirements.front().deliveredAmount == 2000,
        "partial multi-material phase progress survives a snapshot restore");
    const auto firstStageDelivery = partialChainRestore.deliver(stagedOffer.id, concretePath, 800, now + 2);
    expect(firstStageDelivery.stageCompleted && !firstStageDelivery.completed,
        "all materials complete phase one without completing the full program");
    expect(partialChainRestore.state().active.front().currentStageIndex == 1,
        "completing phase one unlocks phase two");
    expect(ContractEngine::progressPercent(partialChainRestore.state().active.front()) >= 25,
        "program progress includes completed earlier phases");

    const auto futureStageAttempt = partialChainRestore.deliver(stagedOffer.id, motorPath, 150, now + 3);
    expect(!futureStageAttempt.requirementFound && futureStageAttempt.acceptedAmount == 0,
        "future-phase products cannot be delivered early");
    ContractEngine restoredChain(partialChainRestore.state());
    expect(restoredChain.state().active.front().currentStageIndex == 1,
        "current phase survives a save snapshot restore");
    expect(ContractEngine::currentStageTitle(restoredChain.state().active.front()) == "02 - Reinforced Superstructure",
        "restored program identifies its current phase");
    const auto incompleteSecondMaterial = restoredChain.deliver(stagedOffer.id, reinforcedPath, 500, now + 4);
    expect(!incompleteSecondMaterial.stageCompleted,
        "a phase with multiple requirements remains open until every line is supplied");
    const auto secondStageDelivery = restoredChain.deliver(stagedOffer.id, modularFramePath, 150, now + 5);
    expect(secondStageDelivery.stageCompleted && !secondStageDelivery.completed,
        "phase two advances only after both reinforcement materials arrive");
    expect(restoredChain.state().active.front().currentStageIndex == 2,
        "phase two unlocks the powertrain phase");

    const auto motors = restoredChain.deliver(stagedOffer.id, motorPath, 150, now + 6);
    expect(!motors.stageCompleted, "powertrain phase waits for its secondary components");
    const auto rotors = restoredChain.deliver(stagedOffer.id, rotorPath, 120, now + 7);
    expect(!rotors.stageCompleted, "powertrain phase still waits for its stator shipment");
    const auto stators = restoredChain.deliver(stagedOffer.id, statorPath, 80, now + 8);
    expect(stators.stageCompleted && !stators.completed, "all three powertrain lines complete phase three");

    const auto computers = restoredChain.deliver(stagedOffer.id, computerPath, 80, now + 9);
    expect(!computers.completed, "final commissioning waits for its circuit-board shipment");
    const auto finalStageDelivery = restoredChain.deliver(stagedOffer.id, circuitBoardPath, 100, now + 10);
    expect(finalStageDelivery.stageCompleted && finalStageDelivery.completed,
        "the final delivery completes the entire four-phase program");
    expect(finalStageDelivery.reputationAwarded == 1100 && restoredChain.state().reputation == 2100,
        "the expanded program awards its full reputation reward exactly once at final completion");
    expect(restoredChain.state().active.empty(), "completed industrial program leaves active operations");
    expect(restoredChain.state().history.back().stages.size() == 4,
        "completed history retains the complete four-phase program route");
    expect(ContractEngine::progressPercent(restoredChain.state().history.back()) == 100,
        "completed industrial program reports full progress");

    Snapshot malformedStages;
    Contract malformedChain;
    malformedChain.id = "FC-MALFORMED-STAGES";
    malformedChain.currentStageIndex = 999;
    malformedChain.stages = {
        {"Stage 1", {{"item-a", "Item A", -5, 10}}},
        {"Stage 2", {{"item-b", "Item B", 3, 1}}}
    };
    malformedStages.active.push_back(malformedChain);
    ContractEngine sanitizedStages(malformedStages);
    expect(sanitizedStages.state().active.front().currentStageIndex == 1,
        "restored stage index is clamped to the final valid stage");
    expect(ContractEngine::currentRequirements(sanitizedStages.state().active.front()).front().deliveredAmount == 1,
        "current stage delivery amount remains bounded after restore");


    // The active-contract cap must be tested with more than three distinct offers available.
    ContractEngine capEngine(advancedState);
    capEngine.ensureOffers(10, now, 9981);
    expect(capEngine.state().offers.size() == 9, "cap test has at least four available offers");
    const auto capOffers = capEngine.state().offers;
    expect(capEngine.accept(capOffers[0].id, now), "first active slot can be used");
    expect(capEngine.accept(capOffers[1].id, now), "second active slot can be used");
    expect(capEngine.accept(capOffers[2].id, now), "third active slot can be used");
    expect(capEngine.accept(capOffers[3].id, now), "fourth active slot can be used");
    expect(!capEngine.accept(capOffers[4].id, now), "fifth active contract is rejected");
    expect(capEngine.state().active.size() == 4, "expanded active contract cap remains four");

    Snapshot historyState;
    for (int index = 0; index < 55; ++index) {
        Contract oldContract; oldContract.id = "FC-OLD-" + std::to_string(index);
        oldContract.expiresAtUnix = now; oldContract.requirements.push_back({"item", "Test Item", 1, 0});
        historyState.offers.push_back(oldContract);
    }
    ContractEngine historyEngine(historyState); historyEngine.expire(now);
    expect(historyEngine.state().history.size() == 50, "history retains no more than 50 entries");
    expect(historyEngine.state().offers.empty(), "all elapsed offers are removed");

    Snapshot oversizedHistory;
    for (int index = 0; index < 60; ++index) {
        Contract oldContract; oldContract.id = "FC-SAVED-" + std::to_string(index);
        oversizedHistory.history.push_back(oldContract);
    }
    ContractEngine loadedHistoryEngine(oversizedHistory);
    expect(loadedHistoryEngine.state().history.size() == 50, "restoring an oversized save trims history to the limit");

    Snapshot expiryState;
    Contract expiring; expiring.id = "FC-EXPIRING"; expiring.title = "Expiry test";
    expiring.timeLimited = true; expiring.expiresAtUnix = now + 30;
    expiring.requirements.push_back({"item", "Test Item", 10, 0});
    expiryState.active.push_back(expiring);
    ContractEngine expiryEngine(expiryState); expiryEngine.expire(now + 30);
    expect(expiryEngine.state().active.empty(), "expired active contract is removed");
    expect(expiryEngine.state().history.size() == 1 && expiryEngine.state().history.front().expired, "expired contract is recorded as expired");
    expect(expiryEngine.state().reputation == 0, "expired contract grants no reputation");

    Snapshot offerExpiryState;
    Contract expiringOffer; expiringOffer.id = "FC-OFFER-EXPIRING"; expiringOffer.expiresAtUnix = now + 5;
    offerExpiryState.offers.push_back(expiringOffer);
    ContractEngine offerExpiryEngine(offerExpiryState); offerExpiryEngine.expire(now + 5);
    expect(offerExpiryEngine.state().offers.empty(), "expired offer is removed from the board");
    expect(offerExpiryEngine.state().history.size() == 1 && offerExpiryEngine.state().history.front().expired, "expired offer is recorded in history");

    Snapshot malformedState;
    malformedState.reputation = -25;
    malformedState.nextSerial = 0;
    Contract malformed; malformed.id = "FC-MALFORMED";
    malformed.requirements.push_back({"item", "Test Item", -4, 90});
    malformedState.active.push_back(malformed);
    ContractEngine sanitizedEngine(malformedState);
    expect(sanitizedEngine.state().reputation == 0, "negative reputation is sanitized");
    expect(sanitizedEngine.state().nextSerial == 1, "zero contract serial is sanitized");
    expect(sanitizedEngine.state().active.front().requirements.front().requiredAmount == 0, "negative requirement amount is sanitized");
    expect(sanitizedEngine.state().active.front().requirements.front().deliveredAmount == 0, "delivered amount is clamped to its requirement");

    Snapshot abandonState;
    abandonState.reputation = 50;
    Contract abandonTarget;
    abandonTarget.id = "FC-ABANDON";
    abandonTarget.title = "Cancellation test";
    abandonTarget.rewardReputation = 42;
    abandonTarget.requirements.push_back({"item", "Test Item", 20, 7});
    abandonState.active.push_back(abandonTarget);
    ContractEngine abandonEngine(abandonState);
    const AbandonResult abandoned = abandonEngine.abandon("FC-ABANDON", now + 10);
    expect(abandoned.contractFound, "an active contract can be abandoned");
    expect(abandoned.reputationPenalty == 11, "abandon penalty rounds 25 percent up");
    expect(abandonEngine.state().reputation == 39, "abandon penalty is deducted from reputation");
    expect(abandonEngine.state().active.empty(), "abandoned contract leaves active list");
    expect(abandonEngine.state().history.size() == 1 && abandonEngine.state().history.front().abandoned,
        "abandoned contract is retained in history");
    expect(abandonEngine.state().history.front().requirements.front().deliveredAmount == 7,
        "abandon history retains previously delivered progress");
    expect(!abandonEngine.abandon("FC-ABANDON", now + 11).contractFound,
        "the same contract cannot be abandoned twice");

    Snapshot lowRepState;
    lowRepState.reputation = 4;
    Contract costly; costly.id = "FC-COSTLY"; costly.rewardReputation = 100;
    costly.requirements.push_back({"item", "Test Item", 1, 0});
    lowRepState.active.push_back(costly);
    ContractEngine lowRepEngine(lowRepState);
    const AbandonResult lowRepAbandoned = lowRepEngine.abandon("FC-COSTLY", now + 12);
    expect(lowRepAbandoned.reputationPenalty == 4 && lowRepEngine.state().reputation == 0,
        "abandon fee cannot push reputation below zero");

    Snapshot minimumFeeState;
    minimumFeeState.reputation = 10;
    Contract lowReward; lowReward.id = "FC-LOW-REWARD"; lowReward.rewardReputation = 1;
    lowReward.requirements.push_back({"item", "Test Item", 1, 0});
    minimumFeeState.active.push_back(lowReward);
    ContractEngine minimumFeeEngine(minimumFeeState);
    expect(minimumFeeEngine.abandon("FC-LOW-REWARD", now + 13).reputationPenalty == 5,
        "abandon fee has a five-reputation minimum when reward is positive");

    Snapshot expiredAbandonState;
    Contract alreadyExpired; alreadyExpired.id = "FC-ALREADY-EXPIRED";
    alreadyExpired.expiresAtUnix = now + 1;
    alreadyExpired.requirements.push_back({"item", "Test Item", 1, 0});
    expiredAbandonState.active.push_back(alreadyExpired);
    ContractEngine expiredAbandonEngine(expiredAbandonState);
    expect(!expiredAbandonEngine.abandon("FC-ALREADY-EXPIRED", now + 1).contractFound,
        "expired contract cannot be manually abandoned");
    expect(expiredAbandonEngine.state().history.size() == 1 && expiredAbandonEngine.state().history.front().expired,
        "expiry takes precedence over manual abandonment");

    expect(std::string(ContractEngine::rankName(0)) == "FICSIT Trainee", "entry rank");
    expect(std::string(ContractEngine::rankName(150)) == "Logistics Associate", "associate rank threshold");
    expect(std::string(ContractEngine::rankName(500)) == "Logistics Specialist", "specialist rank threshold");
    expect(std::string(ContractEngine::rankName(1000)) == "FICSIT Coordinator", "coordinator rank threshold");
    expect(std::string(ContractEngine::rankName(2000)) == "Supply Chain Director", "director rank threshold");
    expect(std::string(ContractEngine::rankName(1500)) == "Regional Logistics Director", "regional director rank threshold");
    expect(std::string(ContractEngine::rankName(3000)) == "FICSIT Executive", "executive rank threshold");

    if (failures == 0) { std::cout << "All FICSIT Contracts core tests passed.\n"; return EXIT_SUCCESS; }
    std::cerr << failures << " test(s) failed.\n"; return EXIT_FAILURE;
}
