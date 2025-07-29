#pragma once

struct ModSource;
#include <vector>
#include <memory>
#include <algorithm>
#include "ModRoute.h"
#include "ModSource.h"

// Extremely small placeholder modulation matrix just to compile.
class ModMatrix {
public:
    void clear() { routes.clear(); sources.clear(); }

    void addSource(ModSourcePtr src) {
        if (!src) return;
        sources.push_back(std::move(src));
    }

    void addRoute(const ModRoute& r) { routes.push_back(r); }

    // Sum modulation for a given parameter. In a real synth you would cache lookups.
    float sumForParam(int paramID) {
        float sum = 0.0f;
        for (const auto& r : routes) {
            if (r.targetParamID == paramID) {
                auto* src = findSource(r.source);
                if (src) sum += r.depth * src->nextValue();
            }
        }
        return sum;
    }

private:
    ModSource* findSource(ModSourceType t) const {
        for (const auto& s : sources) if (s && s->type() == t) return s.get();
        return nullptr;
    }

    std::vector<ModRoute> routes;
    std::vector<ModSourcePtr> sources;
};