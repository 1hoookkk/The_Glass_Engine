// source/core/StateManager.h
#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

class StateManager
{
public:
    explicit StateManager (juce::AudioProcessorValueTreeState& stateRef) noexcept;

    void saveTo      (juce::MemoryBlock& destData) const;
    void loadFrom    (const void* data, int sizeInBytes);

    juce::MemoryBlock serialise  () const;
    void              deserialise(const void* data, int sizeInBytes);

    juce::ValueTree&       getState() noexcept;
    const juce::ValueTree& getState() const noexcept;

private:
    juce::AudioProcessorValueTreeState& apvts;
};
