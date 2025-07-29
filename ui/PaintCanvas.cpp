#include "PaintCanvas.h"

PaintCanvas::PaintCanvas()
{
    setSize(300, 150);
    canvasImage = juce::Image(juce::Image::RGB, getWidth(), getHeight(), true);
}

void PaintCanvas::resized()
{
    canvasImage = juce::Image(juce::Image::RGB, getWidth(), getHeight(), true);
    repaint();
}

void PaintCanvas::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    g.drawImageAt(canvasImage, 0, 0);

    // Optional: draw grid
    g.setColour(juce::Colours::darkgrey);
    for (int x = 0; x < getWidth(); x += 10)
        g.drawVerticalLine(x, 0.0f, (float)getHeight());
    for (int y = 0; y < getHeight(); y += 10)
        g.drawHorizontalLine(y, 0.0f, (float)getWidth());
}

void PaintCanvas::mouseDown(const juce::MouseEvent& event)
{
    drawAt(event.getPosition(), event.mods.isRightButtonDown());
}

void PaintCanvas::mouseDrag(const juce::MouseEvent& event)
{
    drawAt(event.getPosition(), event.mods.isRightButtonDown());
}

void PaintCanvas::drawAt(const juce::Point<int>& point, bool erase)
{
    juce::Graphics g(canvasImage);
    g.setColour(erase ? juce::Colours::black : drawColour);
    g.fillEllipse((float)point.x - 2.5f, (float)point.y - 2.5f, 5.0f, 5.0f);
    repaint();
}

void PaintCanvas::clearCanvas()
{
    canvasImage.clear(canvasImage.getBounds(), juce::Colours::black);
    repaint();
}
