#pragma once
#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>

// Enum for modulation sources (add more as needed)
enum class ModSourceType
{
    LFO1, LFO2, Macro1, Macro2, Envelope1, None
};

// ModRoute is a struct linking a source to a target parameter, with depth and enabled flag.
struct ModRoute
{
    ModSourceType source;
    juce::String targetParamID; // e.g. "grainPitch"
    float depth = 0.0f;
    bool enabled = true;

    // Serialize to ValueTree
    juce::ValueTree toValueTree() const
    {
        juce::ValueTree t("ModRoute");
        t.setProperty("source", static_cast<int>(source), nullptr);
        t.setProperty("target", targetParamID, nullptr);
        t.setProperty("depth", depth, nullptr);
        t.setProperty("enabled", enabled, nullptr);
        return t;
    }
    static ModRoute fromValueTree(const juce::ValueTree& t)
    {
        ModRoute r;
        r.source = static_cast<ModSourceType>(int(t["source"]));
        r.targetParamID = t["target"].toString();
        r.depth = float(t["depth"]);
        r.enabled = bool(t["enabled"]);
        return r;
    }
};
