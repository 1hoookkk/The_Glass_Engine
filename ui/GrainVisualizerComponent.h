#pragma once

#include <JuceHeader.h>
#include <mutex>

class GrainVisualizerComponent : public juce::Component, private juce::Timer
{
public:
    GrainVisualizerComponent();
    void paint(juce::Graphics&) override;
    void resized() override {}
    void setGrainStates(const std::vector<juce::Point<float>>& grainPositions);

private:
    std::vector<juce::Point<float>> grains;
    std::mutex grainMutex;

    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GrainVisualizerComponent)
};
