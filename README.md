# Tether_Free_LLM

**Tier-0 Zero-Load speculation engine** for on-device LLM inference (2026).

This repo contains a production-shaped **Suffix Automaton (SAM)** optimized to live in **L3 cache**:

- **64-byte cacheline-packed nodes** (`alignas(64)`) to match L3 physics
- **In-node open-addressing transitions** (no pointer-chasing)
- **Bounded-memory 128k token window** using a deterministic sliding-window rebuild strategy
- "Boilerplate Laser" gates:
  - `min_occ = 2` (must be seen twice)
  - `max_deg = 1` (only speculate deterministic continuations)

## Build (VS Code / CMake Tools)

### Requirements
- CMake >= 3.16
- C++17 compiler (clang++ or g++)

### Command line

```bash
mkdir -p build
cmake -S . -B build
cmake --build build -j
ctest --test-dir build --output-on-failure
./build/tether_demo
```

## What to open in VS Code

Open the folder **Tether_Free_LLM**.

If you have the **CMake Tools** extension installed:
- `CMake: Configure`
- `CMake: Build`
- Run `tether_demo`

## Files

- `include/sam_l3_bounded.hpp` : the Tier-0 SAM engine
- `src/main.cpp` : demo
- `tests/test_smoke.cpp` : smoke test

## Integration point

In your on-device inference loop:

1) After target verifies tokens, call:

```cpp
sam.extend_verified(tok);
```

2) Before waking the draft model, attempt Tier-0 speculation:

```cpp
auto tier0 = sam.propose(k_max, 2, 1);
```

3) Verify Tier-0 tokens in one target pass.

---

This is the Tier-0 “Zero-Load” substrate for tether-free LLMs.
