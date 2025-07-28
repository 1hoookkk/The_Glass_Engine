// source/core/ParamMirror.h
#pragma once
#include <JuceHeader.h>

class ParamMirror
{
public:
    ParamMirror() = default;

    void init (juce::AudioProcessorValueTreeState& s, const juce::String& paramID)
    {
        param  = s.getParameter (paramID);
        jassert (param != nullptr);
        cached.store (param->getValue(), std::memory_order_relaxed);
    }

    void set (float newValue) noexcept
    {
        cached.store (newValue, std::memory_order_relaxed);
    }

    float get() const noexcept
    {
        return cached.load (std::memory_order_relaxed);
    }

private:
    juce::RangedAudioParameter* param = nullptr;
    std::atomic<float>          cached { 0.0f };
};
