DESIGN.md — VisualGranularSynth M0
Vision
Experimental, modular granular synthesizer

Radical MetaSynth/CDP-inspired workflow

Real-time-safe, high-performance core DSP

Strict no-allocation, no-locks, no-exceptions in audio thread

Architecture (M0 Milestone)
Grain: Atomic playback unit (see Grain.h)

GrainPool: Fixed pool, allocation-free (see GrainPool.h)

WindowTable: Precomputed window shapes (see WindowTable.h)

GranularEngine: Manages grain playback, SIMD-based mixdown, static resource usage only

Perf Harness: Measures block latency, enforces real-time constraints

Real-Time Safety
No heap allocation or locking in audio thread

Exception safety (no throw/catch in callback)

SIMD paths for SSE2 and AVX2

Block-level profiling (BlockProfiler)

File Structure
source/dsp/granular/Grain.h

source/dsp/granular/GrainPool.h

source/dsp/granular/WindowTable.h

source/dsp/granular/GranularEngine.h/.cpp

perf/BlockProfiler.h

perf/PerfHarness.cpp

plugin/PluginProcessor.*

Modularity
Grain triggering logic externally controlled

Block-based processing (JUCE buffer size)

Future: spectral/image path, flexible mod matrix

