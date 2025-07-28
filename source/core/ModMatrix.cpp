// ModMatrix.cpp
#include "ModMatrix.h"

void ModMatrix::addSource(std::shared_ptr<ModSource> source) {
    sources.push_back(std::move(source));
}

void ModMatrix::addRoute(const std::shared_ptr<ModSource>& source, const std::string& targetParamID, float amount) {
    routes.emplace_back(source, targetParamID, amount);
}

void ModMatrix::clearRoutes() {
    routes.clear();
}

void ModMatrix::prepare(double sampleRate) {
    for (auto& src : sources)
        src->prepare(sampleRate);
}

void ModMatrix::process(int numSamples) {
    for (auto& src : sources)
        src->process(numSamples);
}

float ModMatrix::getModValueForParam(const std::string& paramID) const {
    float sum = 0.0f;
    for (const auto& route : routes) {
        if (route.targetParamID == paramID && route.source)
            sum += route.source->getValue() * route.amount;
    }
    return sum;
}
