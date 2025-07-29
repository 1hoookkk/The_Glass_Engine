// PluginProcessor.cpp
#include "PluginProcessor.h"
#include "PluginEditor.h"

VisualGranularSynthAudioProcessor::VisualGranularSynthAudioProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true))
{
}

VisualGranularSynthAudioProcessor::~VisualGranularSynthAudioProcessor() = default;

void VisualGranularSynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    granularEngine.prepare (sampleRate, samplesPerBlock);

    if (! sampleLoaded.load())
    {
        // Create a 2-second sine test buffer if none loaded
        fileBuffer.setSize (2, static_cast<int> (sampleRate * 2.0));
        fileBuffer.clear();

        for (int ch = 0; ch < fileBuffer.getNumChannels(); ++ch)
        {
            auto* data = fileBuffer.getWritePointer (ch);
            for (int i = 0; i < fileBuffer.getNumSamples(); ++i)
            {
                float phase = (float) i / (float) sampleRate;
                data[i] = 0.7f * std::sin (juce::MathConstants<float>::twoPi * 440.0f * phase);
            }
        }

        sampleLoaded.store (true);
    }

    // Hand off to engine under lock
    {
        const juce::ScopedLock sl (bufferLock);
        granularEngine.setSourceBuffer (fileBuffer);
    }
}

void VisualGranularSynthAudioProcessor::releaseResources()
{
    granularEngine.reset();
}

bool VisualGranularSynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    auto out = layouts.getMainOutputChannelSet();
    return out == juce::AudioChannelSet::mono()
        || out == juce::AudioChannelSet::stereo();
}

void VisualGranularSynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                                      juce::MidiBuffer& /*midi*/)
{
    juce::ScopedNoDenormals noDenormals;
    const juce::ScopedLock sl (bufferLock);

    buffer.clear();
    granularEngine.process (buffer);
}

void VisualGranularSynthAudioProcessor::loadSample (const juce::File& file)
{
    juce::AudioFormatManager fm;
    fm.registerBasicFormats();
    if (auto* reader = fm.createReaderFor (file))
    {
        const juce::ScopedLock sl (bufferLock);

        fileBuffer.setSize ((int) reader->numChannels, (int) reader->lengthInSamples);
        reader->read (&fileBuffer,
                      0, (int) reader->lengthInSamples, 0,
                      true, true);

        fileSampleRate = reader->sampleRate;
        sampleLoaded.store (true);
        granularEngine.setSourceBuffer (fileBuffer);
    }
}

juce::AudioProcessorEditor* VisualGranularSynthAudioProcessor::createEditor()
{
    return new VisualGranularSynthAudioProcessorEditor (*this);
}

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VisualGranularSynthAudioProcessor();
}
