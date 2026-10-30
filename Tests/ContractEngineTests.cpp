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
    expect(advancedEngine.state().offers.size() == 9, "advanced reputation unlocks the full catalog");
    bool foundLongTerm = false;
    for (const auto& offer : advancedEngine.state().offers) if (offer.longTerm) {
        foundLongTerm = true;
        expect(offer.requirements.size() == 3, "long-term backbone order has three product requirements");
    }
    expect(foundLongTerm, "1000 reputation unlocks the long-term factory contract");

    // The active-contract cap must be tested with more than three distinct offers available.
    ContractEngine capEngine(advancedState);
    capEngine.ensureOffers(10, now, 9981);
    expect(capEngine.state().offers.size() == 9, "cap test has at least four available offers");
    const auto capOffers = capEngine.state().offers;
    expect(capEngine.accept(capOffers[0].id, now), "first active slot can be used");
    expect(capEngine.accept(capOffers[1].id, now), "second active slot can be used");
    expect(capEngine.accept(capOffers[2].id, now), "third active slot can be used");
    expect(!capEngine.accept(capOffers[3].id, now), "fourth active contract is rejected");
    expect(capEngine.state().active.size() == 3, "active contract cap remains three");

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

    if (failures == 0) { std::cout << "All FICSIT Contracts core tests passed.\n"; return EXIT_SUCCESS; }
    std::cerr << failures << " test(s) failed.\n"; return EXIT_FAILURE;
}
