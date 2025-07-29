#pragma once
#include "PluginProcessor.h"

// A custom LookAndFeel class to get the exact "Null-OS" style
class ArtefactLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ArtefactLookAndFeel()
    {
        // Set our custom colors
        setColour(juce::Slider::thumbColourId, juce::Colours::cyan);
        setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::cyan);
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff1c1c1c));
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                          const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override
    {
        auto bounds = juce::Rectangle<float>(x, y, width, height).reduced(2.0f);
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        auto centre = bounds.getCentre();
        
        g.setColour(juce::Colours::cyan);
        g.drawEllipse(centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f, 1.0f);
        
        juce::Path p;
        auto pointerLength = radius * 0.8f;
        p.addRectangle(-2.0f, -radius, 2.0f, pointerLength);
        p.applyTransform(juce::AffineTransform::rotation(toAngle).translated(centre));
        g.fillPath(p);
    }
};

class ArtefactAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    explicit ArtefactAudioProcessorEditor (ArtefactAudioProcessor&);
    ~ArtefactAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    ArtefactAudioProcessor& processorRef;

    // --- OUR UI COMPONENTS ---
    std::unique_ptr<juce::ImageComponent> canvas;
    juce::Array<std::unique_ptr<juce::TextButton>> samplerSlots;
    
    std::unique_ptr<juce::Slider> driveKnob, crushKnob, filterKnob, jitterKnob;
    std::unique_ptr<juce::Label> driveLabel, crushLabel, filterLabel, jitterLabel;
    
    // This object links our sliders to the processor parameters
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<SliderAttachment> driveAttachment, crushAttachment, filterAttachment, jitterAttachment;

    juce::Font pixelFont;
    ArtefactLookAndFeel lookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ArtefactAudioProcessorEditor)
};