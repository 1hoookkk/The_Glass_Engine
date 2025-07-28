// perf/BlockProfiler.h
#pragma once
#include <chrono>
#include <vector>

struct BlockProfiler
{
    using clock = std::chrono::high_resolution_clock;

    struct Sample
    {
        double micros = 0.0;
    };

    struct Scoped
    {
        BlockProfiler& owner;
        clock::time_point t0;
        explicit Scoped (BlockProfiler& o) : owner(o), t0(clock::now()) {}
        ~Scoped()
        {
            const auto ns =
                std::chrono::duration_cast<std::chrono::nanoseconds>(clock::now() - t0).count();
            owner.samples.push_back({ ns / 1000.0 });
        }
    };

    const std::vector<Sample>& get() const { return samples; }

private:
    std::vector<Sample> samples;
};