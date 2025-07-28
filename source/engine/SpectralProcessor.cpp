// source/engine/SpectralProcessor.cpp
#include "SpectralProcessor.h"
#include <algorithm>
#include <cmath>

SpectralProcessor::SpectralProcessor()
{
    fft = std::make_unique<FFTWrapper>(fftOrder);
    
    // Allocate buffers
    inputBuffer.resize(fftSize);
    outputBuffer.resize(fftSize);
    fftData.resize(fftSize);                // now holds time-domain after inverse
    magnitude.resize(fftSize / 2 + 1);
    phase.resize(fftSize / 2 + 1);
    prevPhase.resize(fftSize / 2 + 1);
    phaseAccum.resize(fftSize / 2 + 1);
    spectralMask.resize(fftSize / 2 + 1, 1.0f);
    
    // Initialize buffers
    std::fill(inputBuffer.begin(), inputBuffer.end(), 0.0f);
    std::fill(outputBuffer.begin(), outputBuffer.end(), 0.0f);
    std::fill(prevPhase.begin(), prevPhase.end(), 0.0f);
    std::fill(phaseAccum.begin(), phaseAccum.end(), 0.0f);
}

SpectralProcessor::~SpectralProcessor() = default;

void SpectralProcessor::prepareToPlay(double newSampleRate, int samplesPerBlock)
{
    sampleRate = newSampleRate;
    juce::ignoreUnused(samplesPerBlock);
    
    // Reset phase accumulators
    std::fill(prevPhase.begin(), prevPhase.end(), 0.0f);
    std::fill(phaseAccum.begin(), phaseAccum.end(), 0.0f);
}

void SpectralProcessor::releaseResources()
{
    // Nothing specific to release
}

void SpectralProcessor::process(const float* input, float* output, int numSamples)
{
    if (currentMode == Mode::Bypass)
    {
        std::copy(input, input + numSamples, output);
        return;
    }
    
    for (int i = 0; i < numSamples; ++i)
    {
        // Input buffer (circular)
        inputBuffer[inputPos] = input[i];
        
        // Output from overlap-add buffer
        output[i] = outputBuffer[outputPos];
        outputBuffer[outputPos] = 0.0f;
        
        inputPos++;
        outputPos = (outputPos + 1) % fftSize;
        
        // Process frame when we have enough samples
        if (inputPos >= hopSize)
        {
            processFrame();
            inputPos = 0;
        }
    }
}

void SpectralProcessor::processFrame()
{
    // 1) Copy and window
    std::copy(inputBuffer.begin(), inputBuffer.end(), inputBuffer.begin()); // redundant but keeps your pattern
    fft->applyWindow(inputBuffer.data(), fftSize);                          // :contentReference[oaicite:2]{index=2}

    // 2) FFT: separate real/imag arrays
    const int numBins = fft->getSize() / 2 + 1;
    std::vector<float> real(numBins), imag(numBins);
    fft->forward(inputBuffer.data(), real.data(), imag.data());              // forward -> real/imag

    // 3) Cartesian -> Polar
    for (int bin = 0; bin < numBins; ++bin)
    {
        float re = real[bin], im = imag[bin];
        magnitude[bin] = std::sqrt(re * re + im * im);
        phase[bin]     = std::atan2(im, re);
    }

    // 4) Spectral processing
    switch (currentMode)
    {
        case Mode::FrequencyMask: applyFrequencyMask();     break;
        case Mode::SpectralBlur:  applySpectralBlur();      break;
        case Mode::SpectralFreeze:applySpectralFreeze();    break;
        case Mode::PitchShift:    applyPitchShift();        break;
        case Mode::FormantShift:  applyFormantShift();      break;
        default:                                           break;
    }

    // 5) Polar -> Cartesian
    for (int bin = 0; bin < numBins; ++bin)
    {
        real[bin] = magnitude[bin] * std::cos(phase[bin]);
        imag[bin] = magnitude[bin] * std::sin(phase[bin]);
    }

    // 6) Inverse FFT
    fft->inverse(real.data(), imag.data(), fftData.data());                 // inverse -> time-domain buffer

    // 7) Overlap-add back into circular buffer
    const float scaleFactor = 1.0f / (fftSize * 0.5f); // compensate for FFT scaling & overlap
    for (int n = 0; n < fftSize; ++n)
    {
        int outIdx = (outputPos + n) % fftSize;
        outputBuffer[outIdx] += fftData[n] * scaleFactor;
    }

    // 8) Rotate input buffer
    std::rotate(inputBuffer.begin(), inputBuffer.begin() + hopSize, inputBuffer.end());
}

// (The rest of your methods stay exactly as before:)

void SpectralProcessor::applyFrequencyMask()
{
    int bins = (int)magnitude.size();
    for (int i = 0; i < bins; ++i)
        magnitude[i] *= spectralMask[i];
}

void SpectralProcessor::applySpectralBlur()
{
    if (blurAmount <= 0.0f) return;
    int bins = (int)magnitude.size();
    int radius = static_cast<int>(blurAmount * 10.0f);
    std::vector<float> blurred(bins);

    for (int i = 0; i < bins; ++i)
    {
        float sum = 0.0f, wsum = 0.0f;
        for (int j = -radius; j <= radius; ++j)
        {
            int idx = i + j;
            if (idx >= 0 && idx < bins)
            {
                float w = 1.0f / (1.0f + std::abs(j));
                sum += magnitude[idx] * w;
                wsum += w;
            }
        }
        blurred[i] = sum / wsum;
    }

    for (int i = 0; i < bins; ++i)
        magnitude[i] = magnitude[i] * (1.0f - blurAmount) + blurred[i] * blurAmount;
}

void SpectralProcessor::applySpectralFreeze()
{
    int bins = (int)phase.size();
    if (!freezeEnabled)
    {
        prevPhase = phase;
        return;
    }
    for (int i = 0; i < bins; ++i)
    {
        phaseAccum[i] += 0.01f;
        phase[i] = phaseAccum[i];
    }
}

void SpectralProcessor::applyPitchShift()
{
    if (pitchShiftFactor == 1.0f) return;
    int bins = (int)magnitude.size();
    std::vector<float> mag2(bins), phs2(bins);

    for (int i = 1; i < bins - 1; ++i)
    {
        float shifted = i * pitchShiftFactor;
        int lo = int(shifted), hi = lo + 1;
        float frac = shifted - lo;
        if (hi < bins)
        {
            mag2[i] = magnitude[lo] * (1 - frac) + magnitude[hi] * frac;
            float dp = phase[lo] - prevPhase[lo];
            dp = princArg(dp);
            float trueFreq = (2 * juce::MathConstants<float>::pi * hopSize * lo) / fftSize
                            + dp * fftSize / (2 * juce::MathConstants<float>::pi * hopSize);
            phaseAccum[i] += (2 * juce::MathConstants<float>::pi * hopSize * trueFreq) / fftSize;
            phs2[i] = phaseAccum[i];
        }
    }
    magnitude = mag2;
    phase     = phs2;
}

void SpectralProcessor::applyFormantShift()
{
    if (formantShiftAmount == 0.0f) return;
    int bins = (int)magnitude.size();
    std::vector<float> warped(bins);
    float warp = std::pow(2.0f, formantShiftAmount / 12.0f);
    for (int i = 0; i < bins; ++i)
    {
        float src = i / warp;
        int lo = int(src), hi = lo + 1;
        float frac = src - lo;
        warped[i] = (hi < bins)
                   ? magnitude[lo] * (1-frac) + magnitude[hi] * frac
                   : 0.0f;
    }
    magnitude = warped;
}

void SpectralProcessor::setSpectralMask(const std::vector<float>& mask)
{
    size_t bins = spectralMask.size(), msz = mask.size();
    if (msz == 0) return;
    for (size_t i = 0; i < bins; ++i)
    {
        float pos = i * float(msz - 1) / float(bins - 1);
        int idx = int(pos);
        float f = pos - idx;
        spectralMask[i] = (idx + 1 < msz)
                        ? mask[idx] * (1-f) + mask[idx+1] * f
                        : mask[idx];
    }
}

void SpectralProcessor::setBlurAmount(float amount)
{
    blurAmount = juce::jlimit(0.0f, 1.0f, amount);
}

void SpectralProcessor::setFreezeEnabled(bool enabled)
{
    freezeEnabled = enabled;
}

void SpectralProcessor::setPitchShift(float semitones)
{
    pitchShiftFactor = std::pow(2.0f, juce::jlimit(-24.0f, 24.0f, semitones) / 12.0f);
}

void SpectralProcessor::setFormantShift(float amount)
{
    formantShiftAmount = juce::jlimit(-12.0f, 12.0f, amount);
}

void SpectralProcessor::applyImageMask(const std::vector<float>& imageBrightness, int width, int height)
{
    juce::ignoreUnused(width, height);
    setSpectralMask(imageBrightness);
}

float SpectralProcessor::princArg(float phaseIn)
{
    const float twoPi = 2.0f * juce::MathConstants<float>::pi;
    while (phaseIn > juce::MathConstants<float>::pi)  phaseIn -= twoPi;
    while (phaseIn < -juce::MathConstants<float>::pi) phaseIn += twoPi;
    return phaseIn;
}
