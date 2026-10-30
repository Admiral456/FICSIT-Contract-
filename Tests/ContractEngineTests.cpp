#include "ContractEngine.h"

#include <cstdlib>
#include <iostream>
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
    const auto first = engine.state().offers.front();
    expect(engine.accept(first.id, now), "valid offer can be accepted");
    expect(engine.state().active.size() == 1, "accepted contract becomes active");
    expect(engine.state().offers.size() == 2, "accepted contract is removed from offers");
    expect(!engine.accept("FC-DOES-NOT-EXIST", now), "unknown offer cannot be accepted");

    const auto active = engine.state().active.front();
    const auto requirement = active.requirements.front();
    const int half = requirement.requiredAmount / 2;
    auto partial = engine.deliver(active.id, requirement.itemClassPath, half, now + 1);
    expect(partial.contractFound && partial.requirementFound, "delivery locates contract and item requirement");
    expect(partial.acceptedAmount == half, "partial delivery accepts only supplied amount");
    expect(!partial.completed, "partial delivery does not complete contract");
    expect(ContractEngine::progressPercent(engine.state().active.front()) < 100, "partial delivery progress remains below 100 percent");
    auto rest = engine.deliver(active.id, requirement.itemClassPath, requirement.requiredAmount * 10, now + 2);
    expect(rest.acceptedAmount == requirement.requiredAmount - half, "excess is clamped to remaining amount");
    expect(rest.completed, "fully supplied contract completes");
    expect(rest.reputationAwarded == active.rewardReputation, "completion pays configured reputation exactly once");
    expect(engine.state().active.empty(), "completed contract leaves active list");
    expect(engine.state().reputation == active.rewardReputation, "reputation is added on completion");
    expect(engine.state().history.size() == 1 && engine.state().history.back().completed, "completed contract is recorded in history");
    expect(!engine.deliver(active.id, requirement.itemClassPath, requirement.requiredAmount, now + 3).contractFound, "completed contract cannot be paid twice");
    expect(engine.state().reputation == active.rewardReputation, "duplicate delivery cannot award reputation twice");
    const auto wrongItem = engine.deliver("FC-NOT-ACTIVE", "not-an-item", 20, now + 4);
    expect(!wrongItem.contractFound && wrongItem.acceptedAmount == 0, "unknown contract and item cannot change state");

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

    Snapshot historyState;
    for (int index = 0; index < 55; ++index) {
        Contract oldContract; oldContract.id = "FC-OLD-" + std::to_string(index);
        oldContract.expiresAtUnix = now; oldContract.requirements.push_back({"item", "Test Item", 1, 0});
        historyState.offers.push_back(oldContract);
    }
    ContractEngine historyEngine(historyState); historyEngine.expire(now);
    expect(historyEngine.state().history.size() == 50, "history retains no more than 50 entries");
    expect(historyEngine.state().offers.empty(), "all elapsed offers are removed");

    ContractEngine capEngine; capEngine.ensureOffers(3, now, 9981);
    const auto offers = capEngine.state().offers;
    expect(capEngine.accept(offers[0].id, now), "first slot can be used");
    expect(capEngine.accept(offers[1].id, now), "second slot can be used");
    expect(capEngine.accept(offers[2].id, now), "third slot can be used");
    capEngine.ensureOffers(3, now, 9982);
    expect(capEngine.state().active.size() == 3, "active contract cap is three");

    Snapshot expiryState; Contract expiring; expiring.id = "FC-EXPIRING"; expiring.title = "Expiry test";
    expiring.timeLimited = true; expiring.expiresAtUnix = now + 30;
    expiring.requirements.push_back({"item", "Test Item", 10, 0}); expiryState.active.push_back(expiring);
    ContractEngine expiryEngine(expiryState); expiryEngine.expire(now + 30);
    expect(expiryEngine.state().active.empty(), "expired active contract is removed");
    expect(expiryEngine.state().history.size() == 1 && expiryEngine.state().history.front().expired, "expired contract is recorded as expired");
    expect(expiryEngine.state().reputation == 0, "expired contract grants no reputation");

    expect(std::string(ContractEngine::rankName(0)) == "FICSIT Trainee", "entry rank");
    expect(std::string(ContractEngine::rankName(150)) == "Logistics Associate", "associate rank threshold");
    expect(std::string(ContractEngine::rankName(500)) == "Logistics Specialist", "specialist rank threshold");
    expect(std::string(ContractEngine::rankName(1000)) == "FICSIT Coordinator", "coordinator rank threshold");
    expect(std::string(ContractEngine::rankName(2000)) == "Supply Chain Director", "director rank threshold");
    if (failures == 0) { std::cout << "All FICSIT Contracts core tests passed.\n"; return EXIT_SUCCESS; }
    std::cerr << failures << " test(s) failed.\n"; return EXIT_FAILURE;
}
