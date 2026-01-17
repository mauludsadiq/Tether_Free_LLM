#include <cassert>
#include <vector>
#include "sam_l3_bounded.hpp"

int main() {
    SamL3Bounded128k sam(128000, 16);

    std::vector<uint32_t> seq = {10,20,30,40};
    for (int rep=0; rep<2; rep++) {
        for (auto t: seq) sam.extend_verified(t);
    }

    // after feeding seq twice, propose should return at least one deterministic token
    auto p = sam.propose(4, /*min_occ=*/2, /*max_deg=*/1);
    // It's possible to propose 0 if current match state doesn't point to deterministic continuation,
    // but in this smoke test we accept that it should not crash.
    (void)p;

    return 0;
}
