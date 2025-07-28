// source/dsp/granular/GranularEngine.cpp
#include "GranularEngine.h"

GranularEngine::GranularEngine() {
    // Pre-allocate source buffer with reasonable size
    sourceBuffer.setSize(2, 48000 * 10); // 10 seconds at 48kHz
    sourceBuffer.clear();
}

void GranularEngine::prepare(double sr, int samplesPerBlock) {
    sampleRate = sr;
    reset();
    
    // Pre-warm the grain pool to avoid first-use allocations
    grainPool.reset();
}

void GranularEngine::reset() {
    grainPool.reset();
    samplesSinceLastGrain = 0.0f;
}

void GranularEngine::setSourceBuffer(const juce::AudioBuffer<float>& source) {
    if (source.getNumSamples() == 0) return;
    
    // Copy to internal buffer (this happens on message thread)
    const int numSamples = source.getNumSamples();
    const int numChannels = source.getNumChannels();
    
    sourceBuffer.setSize(numChannels, numSamples, false, false, true);
    sourceBuffer.makeCopyOf(source, true);
    
    // Update length atomically for audio thread
    sourceLength.store(numSamples);
}

void GranularEngine::spawnGrain() {
    const int srcLen = sourceLength.load(std::memory_order_acquire);
    if (srcLen < 1000) return; // Need at least some samples
    
    // Get parameter values once
    const float pos = position.load(std::memory_order_relaxed);
    const float posSpread = positionSpread.load(std::memory_order_relaxed);
    const float pitchVal = pitch.load(std::memory_order_relaxed);
    const float pitchSpr = pitchSpread.load(std::memory_order_relaxed);
    const float durMs = grainDurationMs.load(std::memory_order_relaxed);
    
    // Calculate grain parameters
    const float normalizedPos = pos + (posSpread * getRandomSpread());
    const float startPos = juce::jlimit(0.0f, static_cast<float>(srcLen - 1), 
                                       normalizedPos * srcLen);
    
    const float grainPitch = pitchVal * std::pow(2.0f, pitchSpr * getRandomSpread() / 12.0f);
    const float duration = (durMs * 0.001f) * static_cast<float>(sampleRate);
    const float pan = getRandomSpread(); // Random pan for now
    const float amplitude = 0.5f; // Fixed amplitude for now
    
    // Try to allocate a grain
    if (Grain* grain = grainPool.allocate()) {
        grain->spawn(startPos, duration, grainPitch, amplitude, pan);
    }
}

void GranularEngine::process(juce::AudioBuffer<float>& buffer) {
    const int numSamples = buffer.getNumSamples();
    const int srcLen = sourceLength.load(std::memory_order_acquire);
    
    // Clear output buffer
    buffer.clear();
    
    // Early out if no source
    if (srcLen == 0 || sourceBuffer.getNumChannels() == 0) return;
    
    // Get write pointers
    float* outL = buffer.getWritePointer(0);
    float* outR = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr;
    
    // Get source read pointers
    const float* srcL = sourceBuffer.getReadPointer(0);
    const float* srcR = sourceBuffer.getNumChannels() > 1 ? 
                       sourceBuffer.getReadPointer(1) : srcL;
    
    // Process sample by sample
    for (int i = 0; i < numSamples; ++i) {
        // Check if we need to spawn a new grain
        const float grainRateHz = grainRate.load(std::memory_order_relaxed);
        const float samplesPerGrain = sampleRate / std::max(0.1f, grainRateHz);
        
        samplesSinceLastGrain += 1.0f;
        if (samplesSinceLastGrain >= samplesPerGrain) {
            spawnGrain();
            samplesSinceLastGrain -= samplesPerGrain;
        }
        
        // Mix all active grains
        float mixL = 0.0f;
        float mixR = 0.0f;
        
        for (auto& grain : grainPool) {
            if (!grain.active.load(std::memory_order_relaxed)) continue;
            
            // Get grain sample with interpolation
            const float pos = grain.position;
            const int pos0 = static_cast<int>(pos);
            const int pos1 = std::min(pos0 + 1, srcLen - 1);
            const float frac = pos - pos0;
            
            // Bounds check
            if (pos0 >= srcLen) {
                grain.reset();
                continue;
            }
            
            // Linear interpolation
            const float sampleL = srcL[pos0] + frac * (srcL[pos1] - srcL[pos0]);
            const float sampleR = srcR[pos0] + frac * (srcR[pos1] - srcR[pos0]);
            
            // Get window envelope
            const float window = g_windowTable.lookup(grain.windowPhase);
            
            // Apply amplitude and window
            const float grainOut = grain.amplitude * window;
            const float grainL = sampleL * grainOut;
            const float grainR = sampleR * grainOut;
            
            // Apply panning
            float panL, panR;
            calculatePanGains(grain.pan, panL, panR);
            
            mixL += grainL * panL;
            mixR += grainR * panR;
            
            // Update grain state
            grain.tick();
        }
        
        // Write output
        outL[i] = mixL;
        if (outR) outR[i] = mixR;
    }
}
