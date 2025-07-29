#include "ModRouteRowComponent.h"
#include "PluginEditor.h" // New: Include PluginEditor to use VisualGranularSynthAudioProcessorEditor


ModRouteRowComponent::ModRouteRowComponent(VisualGranularSynthAudioProcessor& p, int idx)
    : proc(p), routeIndex(idx)
{
    // Source selector
    sourceBox.addItem("LFO1",    int(ModSourceType::LFO1)+1);
    sourceBox.addItem("LFO2",    int(ModSourceType::LFO2)+1);
    sourceBox.addItem("Macro1",  int(ModSourceType::Macro1)+1);
    sourceBox.addItem("Macro2",  int(ModSourceType::Macro2)+1);
    addAndMakeVisible(sourceBox);
    sourceBox.onChange = [this]
    {
        // Fix: Use accessor for modMatrix and lock for write access
        std::lock_guard<std::mutex> lock(proc.getModMatrix().getRoutesMutex()); 
        if (juce::isPositiveAndBelow(routeIndex, (int)proc.getModMatrix().routes.size()))
        {
            proc.getModMatrix().routes[routeIndex].source = ModSourceType(sourceBox.getSelectedId()-1);
        }
    };

    // Target selector
    // Fix: Use accessor for APVTS
    auto& params = proc.getAPVTS().getParameterIDs();
    int id = 1;
    for (auto pid : params)
        targetBox.addItem(pid, id++);
    addAndMakeVisible(targetBox);
    targetBox.onChange = [this]
    {
        // Fix: Use accessor for modMatrix and lock for write access
        std::lock_guard<std::mutex> lock(proc.getModMatrix().getRoutesMutex());
        if (juce::isPositiveAndBelow(routeIndex, (int)proc.getModMatrix().routes.size()))
        {
            proc.getModMatrix().routes[routeIndex].targetParamID = targetBox.getText();
        }
    };

    // Depth slider
    depthSlider