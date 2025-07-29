// PluginEditor.cpp
#include "PluginEditor.h"
#include "PluginProcessor.h"

VisualGranularSynthAudioProcessorEditor::VisualGranularSynthAudioProcessorEditor (VisualGranularSynthAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    auto& params = processorRef.getParameters();

    // Grain Size
    grainSizeSlider.setSliderStyle (juce::Slider::Rotary);
    grainSizeSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible (grainSizeSlider);
    grainSizeAttachment.reset (new Attachment (processorRef.parameters, "grainSize", grainSizeSlider));

    // Pitch
    pitchSlider.setSliderStyle (juce::Slider::Rotary);
    pitchSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible (pitchSlider);
    pitchAttachment.reset (new Attachment (processorRef.parameters, "pitch", pitchSlider));

    // Density
    densitySlider.setSliderStyle (juce::Slider::Rotary);
    densitySlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible (densitySlider);
    densityAttachment.reset (new Attachment (processorRef.parameters, "density", densitySlider));

    // Randomness
    randomnessSlider.setSliderStyle (juce::Slider::Rotary);
    randomnessSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible (randomnessSlider);
    randomnessAttachment.reset (new Attachment (processorRef.parameters, "randomness", randomnessSlider));

    setSize (400, 260);
}

VisualGranularSynthAudioProcessorEditor::~VisualGranularSynthAudioProcessorEditor() = default;

void VisualGranularSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::darkslategrey);
}

void VisualGranularSynthAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (20);
    auto knobW = 60;
    auto spacing = (area.getWidth() - 4 * knobW) / 3;

    grainSizeSlider .setBounds (area.removeFromLeft (knobW));
    area.removeFromLeft (spacing);
    pitchSlider     .setBounds (area.removeFromLeft (knobW));
    area.removeFromLeft (spacing);
    densitySlider   .setBounds (area.removeFromLeft (knobW));
    area.removeFromLeft (spacing);
    randomnessSlider.setBounds (area.removeFromLeft (knobW));
}

bool VisualGranularSynthAudioProcessorEditor::isInterestedInFileDrag (const juce::StringArray& files)
{
    for (auto& f : files)
        if (f.endsWithIgnoreCase (".wav") || f.endsWithIgnoreCase (".aiff") || f.endsWithIgnoreCase (".mp3"))
            return true;
    return false;
}

void VisualGranularSynthAudioProcessorEditor::filesDropped (const juce::StringArray& files, int, int)
{
    if (files.size() > 0)
        processorRef.loadSample (juce::File { files[0] });
}
