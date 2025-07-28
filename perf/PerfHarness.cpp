// perf/PerfHarness.cpp
#include <juce_core/juce_core.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "../source/plugin/PluginProcessor.h"
#include "../source/dsp/granular/GranularEngine.h"
#include "BlockProfiler.h"
#include <vector>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace juce;

// Generate a test waveform
void generateTestBuffer(AudioBuffer<float>& buffer, double sampleRate) {
    const int numSamples = static_cast<int>(sampleRate * 2.0); // 2 seconds
    buffer.setSize(2, numSamples);
    
    for (int ch = 0; ch < 2; ++ch) {
        float* data = buffer.getWritePointer(ch);
        for (int i = 0; i < numSamples; ++i) {
            // Simple sine wave with some harmonics
            const float phase = (float)i / sampleRate;
            data[i] = 0.5f * std::sin(2.0f * M_PI * 440.0f * phase) +
                     0.3f * std::sin(2.0f * M_PI * 880.0f * phase) +
                     0.2f * std::sin(2.0f * M_PI * 1320.0f * phase);
        }
    }
}

void testConfiguration(GranularEngine& engine, double sampleRate, int blockSize, 
                      const AudioBuffer<float>& sourceBuffer, DynamicObject* results) {
    engine.prepare(sampleRate, blockSize);
    engine.setSourceBuffer(sourceBuffer);
    
    // Configure for ≥16 grains test patch
    engine.setGrainRate(100.0f);      // 100 grains/sec
    engine.setGrainDuration(200.0f);  // 200ms grains
    engine.setPosition(0.5f);
    engine.setPositionSpread(0.3f);
    engine.setPitch(1.0f);
    engine.setPitchSpread(0.2f);
    
    AudioBuffer<float> buffer(2, blockSize);
    MidiBuffer midi;
    
    // Warm up
    for (int i = 0; i < 50; ++i) {
        engine.process(buffer);
    }
    
    // Measure
    constexpr int iterations = 1000;
    std::vector<double> timings;
    timings.reserve(iterations);
    
    BlockProfiler profiler;
    
    for (int i = 0; i < iterations; ++i) {
        buffer.clear();
        
        BlockProfiler::Scoped scope(profiler);
        engine.process(buffer);
        
        // Record grain count periodically
        if (i % 100 == 0) {
            const size_t grainCount = engine.getActiveGrainCount();
            if (grainCount < 16) {
                // Keep running until we have enough grains
                i--;
            }
        }
    }
    
    // Extract timings
    const auto& samples = profiler.get();
    timings.reserve(samples.size());
    for (const auto& sample : samples) {
        timings.push_back(sample.micros);
    }
    
    // Calculate statistics
    std::sort(timings.begin(), timings.end());
    
    const double avgUs = std::accumulate(timings.begin(), timings.end(), 0.0) / timings.size();
    const double maxUs = timings.back();
    const double p99Us = timings[static_cast<size_t>(timings.size() * 0.99)];
    
    // Convert to milliseconds
    const double avgMs = avgUs / 1000.0;
    const double maxMs = maxUs / 1000.0;
    const double p99Ms = p99Us / 1000.0;
    
    // Get final grain count
    const size_t finalGrainCount = engine.getActiveGrainCount();
    
    // Store results
    String key = String(static_cast<int>(sampleRate / 1000)) + "k_" + String(blockSize);
    
    DynamicObject::Ptr config = new DynamicObject();
    config->setProperty("p99_ms", p99Ms);
    config->setProperty("max_ms", maxMs);
    config->setProperty("avg_ms", avgMs);
    config->setProperty("grains_avg", static_cast<int>(finalGrainCount));
    config->setProperty("grains_p99", static_cast<int>(finalGrainCount)); // Simplified for now
    
    results->setProperty(key, var(config.get()));
    
    // Print for debugging
    std::cout << "Config " << key << ": "
              << "p99=" << p99Ms << "ms, "
              << "max=" << maxMs << "ms, "
              << "grains=" << finalGrainCount << std::endl;
}

int main() {
    ConsoleApplication app;
    
    // Generate test source buffer
    AudioBuffer<float> sourceBuffer;
    generateTestBuffer(sourceBuffer, 48000.0);
    
    // Create results object
    DynamicObject::Ptr root = new DynamicObject();
    DynamicObject::Ptr audio = new DynamicObject();
    
    // Test each configuration
    GranularEngine engine;
    
    testConfiguration(engine, 48000.0, 64, sourceBuffer, audio.get());
    testConfiguration(engine, 48000.0, 128, sourceBuffer, audio.get());
    testConfiguration(engine, 48000.0, 256, sourceBuffer, audio.get());
    
    root->setProperty("audio", var(audio.get()));
    
    // Add environment info
    DynamicObject::Ptr env = new DynamicObject();
    env->setProperty("cpu", SystemStats::getCpuModel());
    #ifdef _MSC_VER
        env->setProperty("compiler", "MSVC " + String(_MSC_VER));
    #else
        env->setProperty("compiler", "Unknown");
    #endif
    env->setProperty("juce", "7.0.x"); // Update as needed
    root->setProperty("env", var(env.get()));
    
    // Write results
    var v(root.get());
    const auto json = JSON::toString(v, true);
    File("perf_results.json").replaceWithText(json);
    
    std::cout << "\nPerformance Results:\n" << json << std::endl;
    
    // Check against budgets
    bool allPassed = true;
    
    // M0 Budgets
    const double budget_48k_64_p99 = 0.25;
    const double budget_48k_64_max = 0.35;
    const double budget_48k_128_p99 = 0.50;
    const double budget_48k_128_max = 0.70;
    const double budget_48k_256_p99 = 1.00;
    const double budget_48k_256_max = 1.40;
    
    auto checkBudget = [&](const String& config, double p99, double max, 
                          double p99Budget, double maxBudget) {
        bool passed = (p99 <= p99Budget) && (max <= maxBudget);
        allPassed &= passed;
        std::cout << config << " " << (passed ? "PASS" : "FAIL") 
                  << " (p99: " << p99 << "/" << p99Budget 
                  << ", max: " << max << "/" << maxBudget << ")" << std::endl;
        return passed;
    };
    
    // Get results and check
    if (auto* cfg = audio->getProperty("48k_64").getDynamicObject()) {
        checkBudget("48k/64", 
                   cfg->getProperty("p99_ms"), 
                   cfg->getProperty("max_ms"),
                   budget_48k_64_p99, 
                   budget_48k_64_max);
    }
    
    if (auto* cfg = audio->getProperty("48k_128").getDynamicObject()) {
        checkBudget("48k/128", 
                   cfg->getProperty("p99_ms"), 
                   cfg->getProperty("max_ms"),
                   budget_48k_128_p99, 
                   budget_48k_128_max);
    }
    
    if (auto* cfg = audio->getProperty("48k_256").getDynamicObject()) {
        checkBudget("48k/256", 
                   cfg->getProperty("p99_ms"), 
                   cfg->getProperty("max_ms"),
                   budget_48k_256_p99, 
                   budget_48k_256_max);
    }
    
    return allPassed ? 0 : 1;
}