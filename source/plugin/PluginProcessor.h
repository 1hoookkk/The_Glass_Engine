#pragma once

#include <JuceHeader.h>
#include "../dsp/granular/GranularEngine.h"
#include <atomic>

class VisualGranularSynthAudioProcessor  : public juce::AudioProcessor
{
public:
    VisualGranularSynthAudioProcessor();
    ~VisualGranularSynthAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override                      { return "Visual Granular Synth"; }
    bool acceptsMidi() const override                                { return true; }
    bool producesMidi() const override                               { return false; }
    bool isMidiEffect() const override                               { return false; }
    double getTailLengthSeconds() const override                     { return 0.0; }
    int getNumPrograms() override                                    { return 1; }
    int getCurrentProgram() override                                 { return 0; }
    void setCurrentProgram(int) override                             {}
    const juce::String getProgramName(int) override                  { return {}; }
    void changeProgramName(int, const juce::String&) override        {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // sample loading
    void loadSample(const juce::File& file);

private:
    GranularEngine granularEngine;

    juce::AudioBuffer<float> fileBuffer;
    double                 fileSampleRate { 44100.0 };
    juce::CriticalSection  bufferLock;
    std::atomic<bool>      sampleLoaded   { false };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VisualGranularSynthAudioProcessor)
};
