#pragma once
#include <cstdint>

// What kind of modulation source is driving a route.
enum class ModSourceType : std::uint8_t {
    None = 0,
    LFO1,
    LFO2,
    Env1,
    Env2,
    Velocity,
    Random,
};

// A single modulation route: source -> target parameter, with depth.
struct ModRoute {
    ModSourceType source { ModSourceType::None };
    int targetParamID { -1 };   // Use your parameter indexing scheme
    float depth { 0.0f };       // bipolar depth (-1..+1 typically)
};