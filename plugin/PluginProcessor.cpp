#include "PluginProcessor.h"
#include "PluginEditor.h"

ArtefactAudioProcessor::ArtefactAudioProcessor()
     : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
       apvts(*this, nullptr, "Parameters", createParameterLayout())
{
}

ArtefactAudioProcessor::~ArtefactAudioProcessor() {}
const juce::String ArtefactAudioProcessor::getName() const { return JucePlugin_Name; }
bool ArtefactAudioProcessor::acceptsMidi() const { return true; }
bool ArtefactAudioProcessor::producesMidi() const { return false; }
bool ArtefactAudioProcessor::isMidiEffect() const { return false; }
double ArtefactAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int ArtefactAudioProcessor::getNumPrograms() { return 1; }
int ArtefactAudioProcessor::getCurrentProgram() { return 0; }
void ArtefactAudioProcessor::setCurrentProgram (int index) { juce::ignoreUnused (index); }
const juce::String ArtefactAudioProcessor::getProgramName (int index) { juce::ignoreUnused (index); return {}; }
void ArtefactAudioProcessor::changeProgramName (int index, const juce::String& newName) { juce::ignoreUnused (index, newName); }
void ArtefactAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) {}
void ArtefactAudioProcessor::releaseResources() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ArtefactAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    return true;
}
#endif

void ArtefactAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiMessages& midiMessages)
{
    buffer.clear();
}

bool ArtefactAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* ArtefactAudioProcessor::createEditor()
{
    return new ArtefactAudioProcessorEditor (*this);
}

void ArtefactAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void ArtefactAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

juce::AudioProcessorValueTreeState::ParameterLayout ArtefactAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>("DRIVE", "Drive", 0.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("CRUSH", "Crush", 0.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("FILTER", "Filter", 0.0f, 1.0f, 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("JITTER", "Jitter", 0.0f, 1.0f, 0.0f));

    return layout;
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ArtefactAudioProcessor();
}