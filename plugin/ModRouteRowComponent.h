#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "core/ModRoute.h" // The struct definition
#include "core/ModSource.h" // For ModSourceType enum

// Forward-declare the editor for potential circular dependencies
class VisualGranularSynthAudioProcessorEditor;

class ModRouteRowComponent : public juce::Component
{
public:
    ModRouteRowComponent(VisualGranularSynthAudioProcessor& p, int idx);
    ~ModRouteRowComponent() override = default;

    // Position child widgets
    void resized() override;

    // Refresh UI to match processor state (e.g., after preset load)
    void updateFromProcessor();

private:
    VisualGranularSynthAudioProcessor& proc;
    int routeIndex;

    juce::ComboBox   sourceBox, targetBox;
    juce::Slider     depthSlider;
    juce::ToggleButton enableButton;
    juce::TextButton deleteButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModRouteRowComponent)
};