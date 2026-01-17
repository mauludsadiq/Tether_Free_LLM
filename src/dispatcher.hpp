#pragma once
#include <cstdint>
#include <vector>

#include "sam_l3_bounded.hpp"

enum class TierMode {
    SAM_DETERMINISTIC,
    SAM_DOMINATED,
    DRAFT_ARBITRATE,
    TARGET_ONLY
};

struct SamBranchOption {
    uint32_t token;
    uint32_t to_state;
    uint32_t weight;
    float prob;
};

struct SamDecision {
    TierMode mode;
    uint32_t degree;
    float H_norm;
    float best_prob;
    std::vector<SamBranchOption> options;
};

struct DispatchCfg {
    uint32_t k_max = 6;
    uint32_t min_occ = 2;
    float p_dom = 0.95f;
    float H_max = 0.20f;
    uint32_t max_branch_options = 6;
};

struct Proposal {
    TierMode mode;
    float confidence;
    std::vector<uint32_t> tokens;
};

SamDecision sam_branch_decision(const SamL3Core& sam, uint32_t max_options);
Proposal dispatch_propose(const SamL3Core& sam, const DispatchCfg& cfg);

struct DraftModel {
    virtual ~DraftModel() = default;
    virtual uint32_t choose_next(const std::vector<uint32_t>& ctx,
                                 const std::vector<uint32_t>& candidates) = 0;
};

Proposal draft_arbitrate_next(const SamL3Core& sam,
                              const DispatchCfg& cfg,
                              DraftModel& draft,
                              const std::vector<uint32_t>& ctx);
