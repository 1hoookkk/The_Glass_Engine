#include "Parameters.h"

juce::AudioProcessorValueTreeState::ParameterLayout Params::createLayout()
{
    using APF = juce::AudioParameterFloat;
    using Rng = juce::NormalisableRange<float>;

    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<APF>(gainOut,   "Output Gain (dB)",   Rng(-60.0f, 6.0f, 0.01f), 0.0f));
    params.push_back(std::make_unique<APF>(scanX,     "Scan X",             Rng(0.0f, 1.0f, 0.0001f), 0.5f));
    params.push_back(std::make_unique<APF>(scanY,     "Scan Y",             Rng(0.0f, 1.0f, 0.0001f), 0.5f));
    params.push_back(std::make_unique<APF>(engineMix, "Engine Mix",         Rng(0.0f, 1.0f, 0.0001f), 1.0f));
    params.push_back(std::make_unique<APF>(grainDur,  "Grain Duration (ms)",Rng(1.0f, 200.0f, 0.1f),  50.0f));
    params.push_back(std::make_unique<APF>(grainPitch,"Grain Pitch (st)",   Rng(-24.0f, 24.0f, 0.01f),0.0f));
    params.push_back(std::make_unique<APF>(grainGain, "Grain Gain (dB)",    Rng(-60.0f, 6.0f, 0.01f), 0.0f));
    params.push_back(std::make_unique<APF>(grainPan,  "Grain Pan",          Rng(-1.0f, 1.0f, 0.0001f),0.0f));

    return { params.begin(), params.end() };
}