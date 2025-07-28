// source/ui/ImageCanvas.h
#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class ImageScanner; // Forward declaration only

class ImageCanvas : public juce::Component,
                   public juce::FileDragAndDropTarget
{
public:
    ImageCanvas();
    ~ImageCanvas() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    
    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;
    
    void setImageScanner(ImageScanner* scanner);
    void loadImage(const juce::File& imageFile);
    void clearImage();
    void setScanPosition(float x, float y);
    void setShowCrosshairs(bool show) { showCrosshairs = show; repaint(); }
    
    std::function<void(juce::Point<float>)> onPositionClicked;
    std::function<void(const juce::File&)> onImageLoaded;

private:
    ImageScanner* imageScanner = nullptr;
    float scanX = 0.5f;
    float scanY = 0.5f;
    bool showCrosshairs = true;
    bool isPainting = false;
    
    juce::Point<float> screenToImageCoords(juce::Point<int> screenPos) const;
    juce::Rectangle<float> getImageDisplayBounds() const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ImageCanvas)
};