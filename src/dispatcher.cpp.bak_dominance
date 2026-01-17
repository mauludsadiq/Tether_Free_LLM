#include "dispatcher.hpp"
#include <algorithm>
#include <cmath>

static inline float safe_log(float x) {
    if (x <= 1e-12f) return -27.631021f;
    return std::log(x);
}

SamDecision sam_branch_decision(const SamL3Core& sam, uint32_t max_options) {
    SamDecision d;
    d.mode = TierMode::TARGET_ONLY;
    d.degree = 0;
    d.H_norm = 1.0f;
    d.best_prob = 0.0f;

    std::vector<SamL3Core::OutEdge> edges;
    sam.match_out_edges(edges, 64);
    if (edges.empty()) return d;

    uint64_t sum_w = 0;
    for (auto &e : edges) sum_w += (uint64_t)e.weight;
    if (sum_w == 0) {
        sum_w = edges.size();
        for (auto &e : edges) e.weight = 1;
    }

    std::vector<SamBranchOption> opts;
    opts.reserve(edges.size());
    for (auto &e : edges) {
        SamBranchOption o;
        o.token = e.token;
        o.to_state = e.to_state;
        o.weight = e.weight;
        o.prob = (float)((double)e.weight / (double)sum_w);
        opts.push_back(o);
    }

    std::sort(opts.begin(), opts.end(),
              [](const SamBranchOption& a, const SamBranchOption& b) {
                  if (a.prob != b.prob) return a.prob > b.prob;
                  return a.weight > b.weight;
              });

    if (opts.size() > max_options) opts.resize(max_options);

    d.options = opts;
    d.degree = (uint32_t)opts.size();
    d.best_prob = opts[0].prob;

    float H = 0.0f;
    for (auto &o : opts) H += -o.prob * safe_log(o.prob);

    float denom = safe_log((float)opts.size());
    if (opts.size() <= 1 || denom <= 0.0f) d.H_norm = 0.0f;
    else {
        d.H_norm = H / denom;
        if (d.H_norm < 0.0f) d.H_norm = 0.0f;
        if (d.H_norm > 1.0f) d.H_norm = 1.0f;
    }

    return d;
}

Proposal dispatch_propose(const SamL3Core& sam, const DispatchCfg& cfg) {
    Proposal p;
    p.mode = TierMode::TARGET_ONLY;
    p.confidence = 0.0f;

    SamDecision d = sam_branch_decision(sam, cfg.max_branch_options);
    if (d.options.empty()) return p;

    if (d.degree == 1) {
        p.mode = TierMode::SAM_DETERMINISTIC;
        p.confidence = 1.0f;
        p.tokens = sam.propose(cfg.k_max, cfg.min_occ, 1);
        return p;
    }

    if (d.degree > 1 && d.best_prob >= cfg.p_dom && d.H_norm <= cfg.H_max) {
        p.mode = TierMode::SAM_DOMINATED;
        p.confidence = d.best_prob;
        p.tokens = sam.propose(cfg.k_max, cfg.min_occ, 2);
        return p;
    }

    p.mode = TierMode::DRAFT_ARBITRATE;
    return p;
}

Proposal draft_arbitrate_next(const SamL3Core& sam,
                              const DispatchCfg& cfg,
                              DraftModel& draft,
                              const std::vector<uint32_t>& ctx) {
    Proposal p;
    p.mode = TierMode::DRAFT_ARBITRATE;
    p.confidence = 0.0f;

    SamDecision d = sam_branch_decision(sam, cfg.max_branch_options);
    if (d.options.size() <= 1) {
        p.mode = TierMode::TARGET_ONLY;
        return p;
    }

    std::vector<uint32_t> candidates;
    for (auto &o : d.options) {
        if (o.weight >= cfg.min_occ) candidates.push_back(o.token);
    }
    if (candidates.empty()) {
        for (size_t i = 0; i < d.options.size() && i < 2; i++) {
            candidates.push_back(d.options[i].token);
        }
    }

    uint32_t chosen = draft.choose_next(ctx, candidates);
    p.tokens = { chosen };
    return p;
}
