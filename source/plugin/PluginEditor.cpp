#include "PluginEditor.h"
#include "PluginProcessor.h"

VisualGranularSynthAudioProcessorEditor::VisualGranularSynthAudioProcessorEditor(VisualGranularSynthAudioProcessor& p)
    : juce::AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(400, 200);

    addAndMakeVisible(loadButton);
    loadButton.addListener(this);
    loadButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    loadButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
}

VisualGranularSynthAudioProcessorEditor::~VisualGranularSynthAudioProcessorEditor()
{
    loadButton.removeListener(this);
}

void VisualGranularSynthAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font("Courier New", 16.0f, juce::Font::bold));
    g.drawFittedText("Visual Granular Synth", getLocalBounds(), juce::Justification::centredTop, 1);
}

void VisualGranularSynthAudioProcessorEditor::resized()
{
    loadButton.setBounds(getWidth() / 2 - 50, getHeight() / 2 - 20, 100, 40);
}

void VisualGranularSynthAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        juce::FileChooser chooser("Choose Sample", {}, "*.wav;*.aif;*.mp3");
        if (chooser.browseForFileToOpen())
            audioProcessor.loadSample(chooser.getResult());
    }
}
