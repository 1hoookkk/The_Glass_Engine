// source/plugin/ImageScanner.cpp
#include "ImageScanner.h"
#include <cstdint>

bool ImageScanner::loadImage(const juce::File& file)
{
    auto image = juce::ImageFileFormat::loadFrom(file);
    if (image.isValid())
    {
        currentImage = image;
        analyzeImage();
        return true;
    }
    return false;
}

// … other methods unchanged …

juce::Colour ImageScanner::getInterpolatedPixel(float x, float y) const
{
    if (!currentImage.isValid()) return juce::Colours::black;

    const float xPos = juce::jlimit(0.0f, 1.0f, x);
    const float yPos = juce::jlimit(0.0f, 1.0f, y);

    const float fX = xPos * (currentImage.getWidth() - 1);
    const float fY = yPos * (currentImage.getHeight() - 1);

    const int x0 = static_cast<int>(fX);
    const int y0 = static_cast<int>(fY);
    const int x1 = std::min(x0 + 1, currentImage.getWidth() - 1);
    const int y1 = std::min(y0 + 1, currentImage.getHeight() - 1);

    const float fx = fX - x0;
    const float fy = fY - y0;

    auto p00 = currentImage.getPixelAt(x0, y0);
    auto p10 = currentImage.getPixelAt(x1, y0);
    auto p01 = currentImage.getPixelAt(x0, y1);
    auto p11 = currentImage.getPixelAt(x1, y1);

    auto lerp = [](const juce::Colour& a, const juce::Colour& b, float t)
    {
        return juce::Colour(
            static_cast<uint8_t>((1.0f - t) * a.getRed()   + t * b.getRed()),
            static_cast<uint8_t>((1.0f - t) * a.getGreen() + t * b.getGreen()),
            static_cast<uint8_t>((1.0f - t) * a.getBlue()  + t * b.getBlue()),
            static_cast<uint8_t>((1.0f - t) * a.getAlpha() + t * b.getAlpha())
        );
    };

    auto px0 = lerp(p00, p10, fx);
    auto px1 = lerp(p01, p11, fx);
    return lerp(px0, px1, fy);
}
