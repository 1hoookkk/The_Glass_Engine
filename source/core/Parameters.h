#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

namespace Params
{
    static constexpr const char* gainOut    = "gainOut";
    static constexpr const char* scanX      = "scanX";
    static constexpr const char* scanY      = "scanY";
    static constexpr const char* engineMix  = "engineMix";
    static constexpr const char* grainDur   = "grainDur";
    static constexpr const char* grainPitch = "grainPitch";
    static constexpr const char* grainGain  = "grainGain";
    static constexpr const char* grainPan   = "grainPan";

    juce::AudioProcessorValueTreeState::ParameterLayout createLayout();
}