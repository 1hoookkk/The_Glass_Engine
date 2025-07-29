#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <atomic>
#include <random>
#include "GrainPool.h"
#include "WindowTable.h"

class GranularEngine
{
public:
    GranularEngine();
    ~GranularEngine();

    void prepare(double sampleRate, int samplesPerBlock);
    void reset();
    void process(juce::AudioBuffer<float>& buffer);

    // Audio parameters - wire these to your processor/GUI
    void setGrainSize(float ms)           { grainDurationMs.store(ms); }
    void setPitch(float semitones)        { pitchSemitones.store(semitones); }
    void setDensity(float grainsPerSec)   { grainRate.store(grainsPerSec); }
    void setRandomness(float r)           { randomness.store(r); }

    void setSourceBuffer(const juce::AudioBuffer<float>& source);

private:
    void triggerGrain();
    void processGrain(Grain& grain, float* outL, float* outR, int numSamples);

    GrainPool<512> grainPool;
    juce::AudioBuffer<float> sourceBuffer;

    // Parameters
    std::atomic<float> grainRate{ 30.0f };
    std::atomic<float> grainDurationMs{ 50.0f };
    std::atomic<float> pitchSemitones{ 0.0f };      // -24 to +24
    std::atomic<float> randomness{ 0.2f };          // 0-1

    // State
    double sampleRate{ 44100.0 };
    float grainPhase{ 0.0f };
    std::mt19937 rng{ 42 };
    std::uniform_real_distribution<float> rand{ 0.0f, 1.0f };
};
