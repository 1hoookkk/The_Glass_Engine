// source/engine/SpectralProcessor.h
#pragma once
#include "../core/FFTWrapper.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <vector>

class SpectralProcessor
{
public:
    SpectralProcessor();
    ~SpectralProcessor();
    
    // Setup
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void releaseResources();
    
    // Processing modes
    enum class Mode
    {
        Bypass,
        FrequencyMask,
        SpectralBlur,
        SpectralFreeze,
        Convolution,
        PitchShift,
        FormantShift
    };
    
    void setMode(Mode newMode) { currentMode = newMode; }
    Mode getMode() const { return currentMode; }
    
    // Main processing
    void process(const float* input, float* output, int numSamples);
    
    // Effect parameters
    void setSpectralMask(const std::vector<float>& mask);
    void setBlurAmount(float amount);
    void setFreezeEnabled(bool enabled);
    void setPitchShift(float semitones);
    void setFormantShift(float amount);
    
    // Image-based control
    void applyImageMask(const std::vector<float>& imageBrightness, int width, int height);
    
    int getFFTSize() const { return fftSize; }
    
private:
    static constexpr int fftOrder = 11; // 2048 samples
    static constexpr int fftSize = 1 << fftOrder;
    static constexpr int hopSize = fftSize / 4;
    
    Mode currentMode = Mode::Bypass;
    double sampleRate = 44100.0;
    
    // FFT
    std::unique_ptr<FFTWrapper> fft;
    
    // Buffers
    std::vector<float> inputBuffer;
    std::vector<float> outputBuffer;
    std::vector<float> fftData;
    std::vector<float> magnitude;
    std::vector<float> phase;
    std::vector<float> prevPhase;
    std::vector<float> phaseAccum;
    
    // Spectral mask
    std::vector<float> spectralMask;
    
    // Parameters
    float blurAmount = 0.0f;
    bool freezeEnabled = false;
    float pitchShiftFactor = 1.0f;
    float formantShiftAmount = 0.0f;
    
    // Overlap-add state
    int inputPos = 0;
    int outputPos = 0;
    
    // Processing functions
    void processFrame();
    void applyFrequencyMask();
    void applySpectralBlur();
    void applySpectralFreeze();
    void applyPitchShift();
    void applyFormantShift();
    
    // Utilities
    float princArg(float phase);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectralProcessor)
};