#pragma once
#include <juce_audio_basics/juce_audio_basics.h>

class GranularEngine
{
public:
    GranularEngine() = default;
    ~GranularEngine() = default;

    void prepare (double /*sampleRate*/, int /*samplesPerBlock*/) {}
    void reset() {}

    void process (juce::AudioBuffer<float>& buffer)
    {
        buffer.clear();
    }
};
