// PluginEditor.h
#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class VisualGranularSynthAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                                 public juce::FileDragAndDropTarget
{
public:
    VisualGranularSynthAudioProcessorEditor (VisualGranularSynthAudioProcessor&);
    ~VisualGranularSynthAudioProcessorEditor() override;

    void paint  (juce::Graphics& g) override;
    void resized() override;

    // File-drag support
    bool isInterestedInFileDrag (const juce::StringArray& files) override;
    void filesDropped          (const juce::StringArray& files, int x, int y) override;

private:
    VisualGranularSynthAudioProcessor& processorRef;

    // Simple 4-knob layout
    juce::Slider                     grainSizeSlider, pitchSlider, densitySlider, randomnessSlider;
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment>      grainSizeAttachment, pitchAttachment, densityAttachment, randomnessAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VisualGranularSynthAudioProcessorEditor)
};
