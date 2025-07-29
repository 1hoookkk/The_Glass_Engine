#pragma once

#include <JuceHeader.h>

class PaintCanvas : public juce::Component
{
public:
    PaintCanvas();

    void paint(juce::Graphics&) override;
    void resized() override;

    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseDown(const juce::MouseEvent& event) override;

    void clearCanvas();

private:
    juce::Image canvasImage;
    juce::Colour drawColour = juce::Colours::white;

    void drawAt(const juce::Point<int>& point, bool erase);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PaintCanvas)
};
