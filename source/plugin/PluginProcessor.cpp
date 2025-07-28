#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

VisualGranularSynthAudioProcessor::VisualGranularSynthAudioProcessor()
    : AudioProcessor(BusesProperties()
          .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
}

VisualGranularSynthAudioProcessor::~VisualGranularSynthAudioProcessor() {}

void VisualGranularSynthAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    granularEngine.prepare(sampleRate, samplesPerBlock);

    if (! sampleLoaded.load())
    {
        juce::AudioBuffer<float> testSource(2, static_cast<int>(sampleRate * 2.0));
        for (int ch = 0; ch < 2; ++ch)
        {
            auto* data = testSource.getWritePointer(ch);
            for (int i = 0; i < testSource.getNumSamples(); ++i)
            {
                float phase = static_cast<float>(i) / (float)sampleRate;
                data[i] = 0.7f * std::sin(2.0f * juce::MathConstants<float>::pi * 440.0f * phase);
            }
        }
        granularEngine.setSourceBuffer(testSource);
    }
}

void VisualGranularSynthAudioProcessor::releaseResources()
{
    granularEngine.reset();
}

bool VisualGranularSynthAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    auto out = layouts.getMainOutputChannelSet();
    return (out == juce::AudioChannelSet::mono() || out == juce::AudioChannelSet::stereo());
}

void VisualGranularSynthAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    const juce::ScopedLock lock(bufferLock);

    if (fileBuffer.getNumSamples() == 0)
        return;

    static int readPosition = 0;
    const int bufferSize   = buffer.getNumSamples();
    const int fileSize     = fileBuffer.getNumSamples();
    const int numChannels  = buffer.getNumChannels();
    const int fileChannels = fileBuffer.getNumChannels();

    for (int chan = 0; chan < numChannels; ++chan)
    {
        float* out = buffer.getWritePointer(chan);
        const float* in = fileBuffer.getReadPointer(juce::jmin(chan, fileChannels - 1));

        for (int i = 0; i < bufferSize; ++i)
        {
            out[i] = in[readPosition];
            if (++readPosition >= fileSize)
                readPosition = 0;
        }
    }
}

juce::AudioProcessorEditor* VisualGranularSynthAudioProcessor::createEditor()
{
    return new VisualGranularSynthAudioProcessorEditor(*this);
}

void VisualGranularSynthAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // implement parameter saving here if needed
}

void VisualGranularSynthAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // implement parameter restoration here if needed
}

void VisualGranularSynthAudioProcessor::loadSample(const juce::File& file)
{
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
    if (reader)
    {
        const juce::ScopedLock lock(bufferLock);
        fileBuffer.setSize((int)reader->numChannels, (int)reader->lengthInSamples);
        reader->read(&fileBuffer, 0, (int)reader->lengthInSamples, 0, true, true);
        fileSampleRate = reader->sampleRate;

        granularEngine.setSourceBuffer(fileBuffer);
        sampleLoaded.store(true);
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VisualGranularSynthAudioProcessor();
}
