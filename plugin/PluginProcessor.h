// PluginProcessor.h
#pragma once

#include <JuceHeader.h>
#include "dsp/granular/GranularEngine.h"

class VisualGranularSynthAudioProcessor  : public juce::AudioProcessor
{
public:
    VisualGranularSynthAudioProcessor();
    ~VisualGranularSynthAudioProcessor() override;

    // AudioProcessor overrides
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    // Editor
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    // State I/O
    void getStateInformation (juce::MemoryBlock& destData) override {}
    void setStateInformation (const void* data, int sizeInBytes) override {}

    // Sample loading (drag-drop)
    void loadSample (const juce::File& file);

    // Expose engine for any direct calls
    GranularEngine& getEngine() noexcept    { return granularEngine; }

private:
    GranularEngine           granularEngine;
    juce::AudioBuffer<float> fileBuffer;
    double                   fileSampleRate { 44100.0 };
    std::atomic<bool>        sampleLoaded   { false };
    juce::CriticalSection    bufferLock;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VisualGranularSynthAudioProcessor)
};
