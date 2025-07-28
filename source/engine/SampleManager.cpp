// source/engine/SampleManager.cpp
#include "SampleManager.h"

SampleManager::SampleManager()
{
    formatManager.registerBasicFormats();
    resampler = std::make_unique<juce::LagrangeInterpolator>();
    
    for (auto& sample : samples)
    {
        sample = std::make_unique<Sample>();
    }
}

SampleManager::~SampleManager() = default;

void SampleManager::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    juce::ignoreUnused(samplesPerBlock);
}

void SampleManager::releaseResources()
{
    // Nothing specific to release
}

bool SampleManager::loadSample(int slot, const juce::File& file)
{
    if (slot < 0 || slot >= MAX_SAMPLES) return false;
    
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
    if (!reader) return false;
    
    auto& sample = samples[slot];
    sample->buffer.setSize(static_cast<int>(reader->numChannels), 
                          static_cast<int>(reader->lengthInSamples));
    
    reader->read(&sample->buffer, 0, static_cast<int>(reader->lengthInSamples), 0, true, true);
    
    sample->name = file.getFileNameWithoutExtension();
    sample->sampleRate = reader->sampleRate;
    sample->isLoaded = true;
    
    // Resample if needed
    if (sample->sampleRate != currentSampleRate && currentSampleRate > 0)
    {
        const double ratio = currentSampleRate / sample->sampleRate;
        const int newLength = static_cast<int>(sample->buffer.getNumSamples() * ratio);
        
        juce::AudioBuffer<float> resampledBuffer(sample->buffer.getNumChannels(), newLength);
        
        for (int channel = 0; channel < sample->buffer.getNumChannels(); ++channel)
        {
            resampler->reset();
            resampler->process(ratio,
                              sample->buffer.getReadPointer(channel),
                              resampledBuffer.getWritePointer(channel),
                              newLength);
        }
        
        sample->buffer = std::move(resampledBuffer);
        sample->sampleRate = currentSampleRate;
    }
    
    return true;
}

bool SampleManager::loadSample(int slot, const juce::AudioBuffer<float>& buffer, double sourceSampleRate)
{
    if (slot < 0 || slot >= MAX_SAMPLES || buffer.getNumSamples() == 0) return false;
    
    auto& sample = samples[slot];
    sample->buffer = buffer;
    sample->sampleRate = sourceSampleRate;
    sample->name = "Sample " + juce::String(slot + 1);
    sample->isLoaded = true;
    
    return true;
}

void SampleManager::clearSample(int slot)
{
    if (slot >= 0 && slot < MAX_SAMPLES)
    {
        samples[slot] = std::make_unique<Sample>();
    }
}

void SampleManager::clearAllSamples()
{
    for (auto& sample : samples)
    {
        sample = std::make_unique<Sample>();
    }
}

const juce::AudioBuffer<float>* SampleManager::getSample(int slot) const
{
    if (slot < 0 || slot >= MAX_SAMPLES || !samples[slot]->isLoaded)
        return nullptr;
    
    return &samples[slot]->buffer;
}

juce::AudioBuffer<float>* SampleManager::getSample(int slot)
{
    if (slot < 0 || slot >= MAX_SAMPLES || !samples[slot]->isLoaded)
        return nullptr;
    
    return &samples[slot]->buffer;
}

const SampleManager::Sample* SampleManager::getSampleInfo(int slot) const
{
    if (slot < 0 || slot >= MAX_SAMPLES)
        return nullptr;
    
    return samples[slot].get();
}

int SampleManager::getNumSamples() const
{
    return MAX_SAMPLES;
}

int SampleManager::getNumLoadedSamples() const
{
    int count = 0;
    for (const auto& sample : samples)
    {
        if (sample->isLoaded) count++;
    }
    return count;
}

bool SampleManager::isSampleLoaded(int slot) const
{
    if (slot < 0 || slot >= MAX_SAMPLES) return false;
    return samples[slot]->isLoaded;
}

void SampleManager::mixSamples(const std::vector<int>& slots, const std::vector<float>& gains, 
                               juce::AudioBuffer<float>& output)
{
    output.clear();
    
    const int numSlots = static_cast<int>(std::min(slots.size(), gains.size()));
    
    for (int i = 0; i < numSlots; ++i)
    {
        const int slot = slots[i];
        const float gain = gains[i];
        
        if (const auto* sample = getSample(slot))
        {
            const int numSamples = std::min(output.getNumSamples(), sample->getNumSamples());
            const int numChannels = std::min(output.getNumChannels(), sample->getNumChannels());
            
            for (int channel = 0; channel < numChannels; ++channel)
            {
                output.addFrom(channel, 0, *sample, channel, 0, numSamples, gain);
            }
        }
    }
}