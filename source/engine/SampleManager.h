// source/engine/SampleManager.h
#pragma once
#include <juce_audio_formats/juce_audio_formats.h>
#include <vector>
#include <memory>

class SampleManager
{
public:
    static constexpr int MAX_SAMPLES = 16;
    
    struct Sample
    {
        juce::String name;
        juce::AudioBuffer<float> buffer;
        double sampleRate = 44100.0;
        bool isLoaded = false;
    };
    
    SampleManager();
    ~SampleManager();
    
    // Lifecycle
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void releaseResources();
    
    // Sample loading
    bool loadSample(int slot, const juce::File& file);
    bool loadSample(int slot, const juce::AudioBuffer<float>& buffer, double sourceSampleRate);
    void clearSample(int slot);
    void clearAllSamples();
    
    // Sample access
    const juce::AudioBuffer<float>* getSample(int slot) const;
    juce::AudioBuffer<float>* getSample(int slot);
    const Sample* getSampleInfo(int slot) const;
    
    // Query
    int getNumSamples() const;
    int getNumLoadedSamples() const;
    bool isSampleLoaded(int slot) const;
    
    // Sample mixing/layering
    void mixSamples(const std::vector<int>& slots, const std::vector<float>& gains, 
                    juce::AudioBuffer<float>& output);
    
private:
    std::array<std::unique_ptr<Sample>, MAX_SAMPLES> samples;
    juce::AudioFormatManager formatManager;
    double currentSampleRate = 44100.0;
    
    // Resampling
    std::unique_ptr<juce::LagrangeInterpolator> resampler;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleManager)
};