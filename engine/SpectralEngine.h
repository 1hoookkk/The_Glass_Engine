// source/engine/SpectralEngine.h
#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <vector>
#include <memory>

class SpectralEngine
{
public:
    SpectralEngine();
    ~SpectralEngine();

    // Lifecycle methods for audio processing
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void releaseResources();

    // Processing methods
    void process(juce::AudioBuffer<float>& buffer);
    void process(const float* inputChannelData, float* outputChannelData, int numSamples);

    // Processing modes
    enum class ProcessingMode
    {
        Bypass,
        FrequencyMask,
        SpectralBlur,
        SpectralFreeze,
        Convolution
    };
    
    void setMode(ProcessingMode mode);
    
    // Spectral effects controls
    void applyMask(const std::vector<float>& maskData, int width, int height);
    void setSpectralBlur(float amount);
    void setFreezeEnabled(bool enabled);
    void setPitchShift(float semitones);
    void setFormantShift(float amount);

private:
    class Impl;
    std::unique_ptr<Impl> impl;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectralEngine)
};