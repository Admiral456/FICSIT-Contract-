#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace FicsitContracts {

enum class Difficulty : std::uint8_t { Small = 0, Medium = 1, Large = 2, Special = 3 };

struct Requirement {
    std::string itemClassPath;
    std::string displayName;
    int requiredAmount = 0;
    int deliveredAmount = 0;
};

struct ContractStage {
    std::string title;
    std::vector<Requirement> requirements;
};

struct Contract {
    std::string id;
    std::string templateId;
    std::string title;
    Difficulty difficulty = Difficulty::Small;
    int rewardReputation = 0;
    bool timeLimited = false;
    std::int64_t expiresAtUnix = 0; // UTC Unix time; zero means no expiry.
    bool longTerm = false;
    bool completed = false;
    bool expired = false;
    bool abandoned = false;
    std::int64_t finishedAtUnix = 0;
    // Legacy single-stage requirements stay supported for existing saves.
    std::vector<Requirement> requirements;
    std::vector<ContractStage> stages;
    std::size_t currentStageIndex = 0;
};

struct Snapshot {
    int reputation = 0;
    std::uint64_t nextSerial = 1;
    std::vector<Contract> offers;
    std::vector<Contract> active;
    std::vector<Contract> history;
};

struct DeliveryResult {
    bool contractFound = false;
    bool requirementFound = false;
    bool completed = false;
    bool stageCompleted = false;
    int completedStageIndex = -1;
    int nextStageIndex = -1;
    int stageCount = 1;
    int acceptedAmount = 0;
    int reputationAwarded = 0;
};

struct AbandonResult {
    bool contractFound = false;
    int reputationPenalty = 0;
};

class ContractEngine {
public:
    explicit ContractEngine(Snapshot state = {});

    const Snapshot& state() const noexcept;
    Snapshot& state() noexcept;

    void ensureOffers(std::size_t targetCount, std::int64_t nowUnix, std::uint32_t seed);
    bool accept(const std::string& contractId, std::int64_t nowUnix);
    DeliveryResult deliver(const std::string& contractId, const std::string& itemClassPath,
                           int amount, std::int64_t nowUnix);
    AbandonResult abandon(const std::string& contractId, std::int64_t nowUnix);
    void expire(std::int64_t nowUnix);

    static int progressPercent(const Contract& contract) noexcept;
    static std::vector<Requirement>& currentRequirements(Contract& contract) noexcept;
    static const std::vector<Requirement>& currentRequirements(const Contract& contract) noexcept;
    static const std::string& currentStageTitle(const Contract& contract) noexcept;
    static std::size_t stageCount(const Contract& contract) noexcept;
    static const char* rankName(int reputation) noexcept;
    static int activeContractLimit() noexcept;

private:
    Snapshot state_;
    std::string makeId();
    void pushHistory(Contract contract);
};

} // namespace FicsitContracts
