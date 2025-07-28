// source/plugin/ImageScanner.h
#pragma once
#include <juce_graphics/juce_graphics.h>
#include <atomic>
#include <vector>

class ImageScanner
{
public:
    ImageScanner();
    ~ImageScanner();
    
    // Image loading
    bool loadImage(const juce::File& file);
    bool loadImage(const juce::Image& image);
    void clearImage();
    
    // Image data access
    const juce::Image& getImage() const { return currentImage; }
    bool hasImage() const { return currentImage.isValid(); }
    
    // Scanning functions
    float getPixelBrightness(float x, float y) const;  // x,y in 0-1 range
    juce::Colour getPixelColour(float x, float y) const;
    
    // Get scan line data for spectral processing
    std::vector<float> getScanLine(float y, int numSamples) const;
    std::vector<float> getVerticalScanLine(float x, int numSamples) const;
    
    // Image analysis
    void analyzeImage();
    float getAverageBrightness() const { return avgBrightness; }
    float getContrast() const { return contrast; }
    
    // Image dimensions
    int getWidth() const { return currentImage.getWidth(); }
    int getHeight() const { return currentImage.getHeight(); }
    
private:
    juce::Image currentImage;
    std::atomic<float> avgBrightness{0.5f};
    std::atomic<float> contrast{0.5f};
    
    // Helper to get interpolated pixel value
    juce::Colour getInterpolatedPixel(float x, float y) const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ImageScanner)
};