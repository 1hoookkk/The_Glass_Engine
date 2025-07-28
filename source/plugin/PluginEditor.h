#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class VisualGranularSynthAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                                 public juce::Button::Listener
{
public:
    VisualGranularSynthAudioProcessorEditor(VisualGranularSynthAudioProcessor&);
    ~VisualGranularSynthAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void buttonClicked(juce::Button*) override;

private:
    VisualGranularSynthAudioProcessor& audioProcessor;
    juce::TextButton loadButton { "Load Sample" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VisualGranularSynthAudioProcessorEditor)
};
