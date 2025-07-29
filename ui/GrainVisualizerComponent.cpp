#include "GrainVisualizerComponent.h"

GrainVisualizerComponent::GrainVisualizerComponent()
{
    startTimerHz(30); // 30 FPS
}

void GrainVisualizerComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

    std::lock_guard lock(grainMutex);
    for (const auto& grain : grains)
    {
        float x = grain.x * (float)getWidth();
        float y = (1.0f - grain.y) * (float)getHeight();

        g.setColour(juce::Colours::aqua);
        g.fillEllipse(x - 2.5f, y - 2.5f, 5.0f, 5.0f);
    }
}

void GrainVisualizerComponent::setGrainStates(const std::vector<juce::Point<float>>& grainPositions)
{
    std::lock_guard lock(grainMutex);
    grains = grainPositions;
}

void GrainVisualizerComponent::timerCallback()
{
    repaint();
}
