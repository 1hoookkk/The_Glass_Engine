// source/engine/SpectralEngine.cpp
#include "SpectralEngine.h"
#include "SpectralProcessor.h" // Include SpectralProcessor to use its functionality
#include <algorithm>
#include <cmath>

// Define the private implementation class within the .cpp file
// This is the Pimpl idiom in action.
class SpectralEngine::Impl
{
public:
    Impl()
    {
        // The SpectralProcessor now handles all the setup and processing logic
        // for spectral effects.
    }

    ~Impl() = default;

    void prepareToPlay(double newSampleRate, int samplesPerBlock)
    {
        spectralProcessor.prepareToPlay(newSampleRate, samplesPerBlock);
    }

    void releaseResources()
    {
        spectralProcessor.releaseResources();
    }

    void process(juce::AudioBuffer<float>& buffer)
    {
        // Assuming single channel processing for simplicity based on SpectralProcessor
        // If multi-channel, you'd loop through channels and call process for each.
        if (buffer.getNumChannels() > 0)
        {
            spectralProcessor.process(buffer.getReadPointer(0), buffer.getWritePointer(0), buffer.getNumSamples());
        }
    }
    
    void process(const float* inputChannelData, float* outputChannelData, int numSamples)
    {
        spectralProcessor.process(inputChannelData, outputChannelData, numSamples);
    }

    void setMode(ProcessingMode newMode)
    {
        // Map SpectralEngine's mode to SpectralProcessor's mode
        switch (newMode)
        {
            case ProcessingMode::Bypass:       spectralProcessor.setMode(SpectralProcessor::Mode::Bypass); break;
            case ProcessingMode::FrequencyMask:spectralProcessor.setMode(SpectralProcessor::Mode::FrequencyMask); break;
            case ProcessingMode::SpectralBlur: spectralProcessor.setMode(SpectralProcessor::Mode::SpectralBlur); break;
            case ProcessingMode::SpectralFreeze:spectralProcessor.setMode(SpectralProcessor::Mode::SpectralFreeze); break;
            // case ProcessingMode::Convolution: // Add handling for convolution if implemented in SpectralProcessor
            default:                           spectralProcessor.setMode(SpectralProcessor::Mode::Bypass); break;
        }
    }

    void applyMask(const std::vector<float>& maskData, int width, int height)
    {
        spectralProcessor.applyImageMask(maskData, width, height); // Assumed to be the same as applyImageMask
    }

    void setSpectralBlur(float amount)
    {
        spectralProcessor.setBlurAmount(amount);
    }

    void setFreezeEnabled(bool enabled)
    {
        spectralProcessor.setFreezeEnabled(enabled);
    }

    void setPitchShift(float semitones)
    {
        spectralProcessor.setPitchShift(semitones);
    }

    void setFormantShift(float amount)
    {
        spectralProcessor.setFormantShift(amount);
    }

private:
    SpectralProcessor spectralProcessor; // The actual processor
};

// Implement SpectralEngine's public interface using the Impl class
SpectralEngine::SpectralEngine() : impl(std::make_unique<Impl>()) {}
SpectralEngine::~SpectralEngine() = default;

void SpectralEngine::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    impl->prepareToPlay(sampleRate, samplesPerBlock);
}

void SpectralEngine::releaseResources()
{
    impl->releaseResources();
}

void SpectralEngine::process(juce::AudioBuffer<float>& buffer)
{
    impl->process(buffer);
}

void SpectralEngine::process(const float* inputChannelData, float* outputChannelData, int numSamples)
{
    impl->process(inputChannelData, outputChannelData, numSamples);
}

void SpectralEngine::setMode(ProcessingMode mode)
{
    impl->setMode(mode);
}

void SpectralEngine::applyMask(const std::vector<float>& maskData, int width, int height)
{
    impl->applyMask(maskData, width, height);
}

void SpectralEngine::setSpectralBlur(float amount)
{
    impl->setSpectralBlur(amount);
}

void SpectralEngine::setFreezeEnabled(bool enabled)
{
    impl->setFreezeEnabled(enabled);
}

void SpectralEngine::setPitchShift(float semitones)
{
    impl->setPitchShift(semitones);
}

void SpectralEngine::setFormantShift(float amount)
{
    impl->setFormantShift(amount);
}