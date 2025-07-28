#include "StateManager.h"

StateManager::StateManager(juce::AudioProcessorValueTreeState& stateRef) noexcept
    : apvts(stateRef) {}

void StateManager::saveTo(juce::MemoryBlock& destData) const
{
    destData = serialise();
}

void StateManager::loadFrom(const void* data, int sizeInBytes)
{
    deserialise(data, sizeInBytes);
}

juce::MemoryBlock StateManager::serialise() const
{
    juce::MemoryBlock dest;
    if (auto xml = apvts.copyState().createXml())
        juce::AudioProcessor::copyXmlToBinary(*xml, dest);
    return dest;
}

void StateManager::deserialise(const void* data, int sizeInBytes)
{
    if (data == nullptr || sizeInBytes <= 0)
        return;

    if (auto xml = juce::AudioProcessor::getXmlFromBinary(data, sizeInBytes))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::ValueTree& StateManager::getState() noexcept
{
    return apvts.state;
}

const juce::ValueTree& StateManager::getState() const noexcept
{
    return apvts.state;
}
