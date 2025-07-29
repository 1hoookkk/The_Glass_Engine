// source/ui/ImageCanvas.cpp
#include "ImageCanvas.h"
#include "../plugin/ImageScanner.h"

ImageCanvas::ImageCanvas()
{
    setWantsKeyboardFocus(true);
}

ImageCanvas::~ImageCanvas() = default;

void ImageCanvas::paint(juce::Graphics& g)
{
    // Colors defined here instead of as static constexpr
    const juce::Colour backgroundColor{0xff1a1a1a};
    const juce::Colour crosshairColor{0xff00ff00};
    const juce::Colour borderColor{0xff404040};
    
    // Clear background
    g.fillAll(backgroundColor);
    
    // Draw border
    g.setColour(borderColor);
    g.drawRect(getLocalBounds(), 1);
    
    if (!imageScanner || !imageScanner->hasImage()) {
        // Draw placeholder text
        g.setColour(juce::Colours::grey);
        g.drawText("Drop image here", getLocalBounds(), juce::Justification::centred);
        return;
    }
    
    // Get image display bounds
    auto imageBounds = getImageDisplayBounds();
    
    // Draw the image
    const auto& image = imageScanner->getImage();
    g.drawImage(image, imageBounds, juce::RectanglePlacement::stretchToFit);
    
    // Draw crosshairs for scan position
    if (showCrosshairs) {
        g.setColour(crosshairColor);
        
        float crossX = imageBounds.getX() + scanX * imageBounds.getWidth();
        float crossY = imageBounds.getY() + scanY * imageBounds.getHeight();
        
        // Vertical line
        g.drawLine(crossX, imageBounds.getY(), crossX, imageBounds.getBottom(), 2.0f);
        
        // Horizontal line  
        g.drawLine(imageBounds.getX(), crossY, imageBounds.getRight(), crossY, 2.0f);
        
        // Center circle
        g.drawEllipse(crossX - 3, crossY - 3, 6, 6, 1.0f);
    }
}

void ImageCanvas::resized()
{
    // Nothing specific needed for resize
}

void ImageCanvas::mouseDown(const juce::MouseEvent& e)
{
    if (!imageScanner || !imageScanner->hasImage()) return;
    
    auto imagePos = screenToImageCoords(e.getPosition());
    
    if (imagePos.x >= 0.0f && imagePos.x <= 1.0f && 
        imagePos.y >= 0.0f && imagePos.y <= 1.0f) {
        
        isPainting = true;
        
        // Notify position click
        if (onPositionClicked) {
            onPositionClicked(imagePos);
        }
    }
}

void ImageCanvas::mouseDrag(const juce::MouseEvent& e)
{
    if (!isPainting || !imageScanner) return;
    
    auto imagePos = screenToImageCoords(e.getPosition());
    
    if (imagePos.x >= 0.0f && imagePos.x <= 1.0f && 
        imagePos.y >= 0.0f && imagePos.y <= 1.0f) {
        
        // Simple painting - just repaint for now
        repaint();
    }
}

void ImageCanvas::mouseUp(const juce::MouseEvent&)
{
    isPainting = false;
}

bool ImageCanvas::isInterestedInFileDrag(const juce::StringArray& files)
{
    for (const auto& file : files) {
        if (juce::File(file).hasFileExtension("png;jpg;jpeg;bmp;gif;tiff")) {
            return true;
        }
    }
    return false;
}

void ImageCanvas::filesDropped(const juce::StringArray& files, int, int)
{
    for (const auto& filepath : files) {
        juce::File file(filepath);
        if (file.hasFileExtension("png;jpg;jpeg;bmp;gif;tiff")) {
            loadImage(file);
            break; // Only load first valid image
        }
    }
}

void ImageCanvas::setImageScanner(ImageScanner* scanner)
{
    imageScanner = scanner;
    repaint();
}

void ImageCanvas::loadImage(const juce::File& imageFile)
{
    if (imageScanner && imageScanner->loadImage(imageFile)) {
        repaint();
        
        if (onImageLoaded) {
            onImageLoaded(imageFile);
        }
    }
}

void ImageCanvas::clearImage()
{
    if (imageScanner) {
        imageScanner->clearImage();
        repaint();
    }
}

void ImageCanvas::setScanPosition(float x, float y)
{
    scanX = juce::jlimit(0.0f, 1.0f, x);
    scanY = juce::jlimit(0.0f, 1.0f, y);
    repaint();
}

juce::Point<float> ImageCanvas::screenToImageCoords(juce::Point<int> screenPos) const
{
    auto imageBounds = getImageDisplayBounds();
    
    if (imageBounds.isEmpty()) {
        return {-1.0f, -1.0f}; // Invalid coordinates
    }
    
    float x = (screenPos.x - imageBounds.getX()) / imageBounds.getWidth();
    float y = (screenPos.y - imageBounds.getY()) / imageBounds.getHeight();
    
    return {x, y};
}

juce::Rectangle<float> ImageCanvas::getImageDisplayBounds() const
{
    if (!imageScanner || !imageScanner->hasImage()) {
        return {};
    }
    
    auto bounds = getLocalBounds().toFloat().reduced(2.0f);
    return bounds;
}