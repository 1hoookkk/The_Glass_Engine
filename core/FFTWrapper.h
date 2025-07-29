#pragma once
#include <juce_dsp/juce_dsp.h>
#include <vector>

// Simple real->complex and complex->real FFT wrapper around juce::dsp::FFT
class FFTWrapper
{
public:
    // 'order' is log2(fftSize). e.g. order=10 => fftSize=1024
    explicit FFTWrapper (int order);
    
    void setOrder (int order);                // reinitialise with new size
    int  getOrder () const { return order_; }
    int  getSize  () const { return fftSize_; }

    // Real input -> complex output (interleaved or separate)
    // outReal/outImag must be size >= fftSize_/2 + 1
    void forward (const float* in, float* outReal, float* outImag);
    void inverse (const float* inReal, const float* inImag, float* out);

    // Utility: apply a window to input before forward
    void applyWindow (float* data, int numSamples);

private:
    int order_    = 0;
    int fftSize_  = 0;
    juce::dsp::FFT fft_;
    juce::HeapBlock<float> buffer_;  // temp buffer (size 2*fftSize for juce FFT)
    juce::HeapBlock<float> window_;  // hann window precomputed

    void computeWindow();
};