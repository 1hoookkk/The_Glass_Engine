#pragma once
#include "ModRoute.h"
#include <memory>

class ModSource {
public:
    virtual ~ModSource() = default;
    virtual ModSourceType type() const = 0;
    virtual float nextValue() = 0;
};

using ModSourcePtr = std::unique_ptr<ModSource>;