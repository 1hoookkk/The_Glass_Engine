#include "GranularEngine.h"
#include <cmath>

GranularEngine::GranularEngine() {
    sourceBuffer.setSize(2, 44100); // default size
    sourceBuffer.clear();
}
GranularEngine::~GranularEngine() {}

void GranularEngine::prepare(double sr, int /*samplesPerBlock*/) {
    sampleRate = sr;
    reset();
}

void GranularEngine::reset() {
    grainPool.reset();
    grainPhase = 0.0f;
}

void GranularEngine::setSourceBuffer(const juce::AudioBuffer<float>& source) {
    sourceBuffer = source;
}

void GranularEngine::process(juce::AudioBuffer<float>& buffer) {
    const float rate = grainRate.load();
    const float durationMs = grainDurationMs.load();
    const float pitch = pitchSemitones.load();
    const float randSpread = randomness.load();

    const float grainInterval = sampleRate / std::max(rate, 0.1f);
    const int numSamples = buffer.getNumSamples();
    buffer.clear();

    // Grain trigger logic
    grainPhase += numSamples;
    while (grainPhase >= grainInterval) {
        grainPhase -= grainInterval;
        triggerGrain();
    }

    // Process grains
    float* outL = buffer.getWritePointer(0);
    float* outR = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : outL;
    for (auto& grain : grainPool) {
        if (grain.active.load(std::memory_order_relaxed)) {
            processGrain(grain, outL, outR, numSamples);
        }
    }
}

void GranularEngine::triggerGrain() {
    auto* grain = grainPool.allocate();
    if (!grain) return;

    // Position: center (0.5) with random spread
    const float posCenter = 0.5f;
    const float spread = randomness.load() * 0.5f;
    const float pos = posCenter + (rand(rng) - 0.5f) * 2.0f * spread;

    grain->startPos = std::clamp(pos, 0.0f, 1.0f) * sourceBuffer.getNumSamples();
    grain->position = grain->startPos;
    grain->duration = grainDurationMs.load() * sampleRate / 1000.0f;

    // Convert semitones to pitch ratio
    const float semitones = pitchSemitones.load();
    grain->pitch = std::pow(2.0f, semitones / 12.0f);

    grain->amplitude = 0.7f;
    grain->pan = rand(rng) * 2.0f - 1.0f; // random pan
    grain->age = 0.0f;
}

void GranularEngine::processGrain(Grain& grain, float* outL, float* outR, int numSamples) {
    if (sourceBuffer.getNumSamples() == 0) return;
    const float* srcL = sourceBuffer.getReadPointer(0);
    const float* srcR = sourceBuffer.getNumChannels() > 1 ? sourceBuffer.getReadPointer(1) : srcL;
    const int srcLen = sourceBuffer.getNumSamples();

    for (int i = 0; i < numSamples; ++i) {
        if (grain.age >= grain.duration) {
            grain.reset();
            return;
        }

        const int idx = static_cast<int>(grain.position) % srcLen;
        float sample = 0.5f * (srcL[idx] + srcR[idx]);
        // (windowing can be added here)
        float panL = 0.5f * (1.0f - grain.pan);
        float panR = 0.5f * (1.0f + grain.pan);

        outL[i] += sample * panL * grain.amplitude;
        outR[i] += sample * panR * grain.amplitude;

        grain.position += grain.pitch;
        grain.age += 1.0f;
    }
}
