#pragma once

#include <juce_dsp/juce_dsp.h>
#include <vector>
#include <complex>

class FFTWrapper
{
public:
    enum WindowType { Hann, Hamming, Blackman };

    FFTWrapper(int order = 10);
    ~FFTWrapper();

    void prepare(WindowType windowType);

    void performFFT(const float* timeData, std::vector<std::complex<float>>& freqData);
    void performIFFT(const std::vector<std::complex<float>>& freqData, float* timeData);

    void cartesianToPolar(const std::vector<std::complex<float>>& freqData,
                          std::vector<float>& magnitudes,
                          std::vector<float>& phases) const;

    void polarToCartesian(const std::vector<float>& magnitudes,
                          const std::vector<float>& phases,
                          std::vector<std::complex<float>>& freqData) const;

    int getFFTSize() const noexcept { return fftSize; }
    int getNumBins() const noexcept { return fftSize / 2 + 1; }

private:
    void applyWindow(float* data);

    int fftOrder;
    int fftSize;
    std::unique_ptr<juce::dsp::FFT> fft;
    std::vector<float> window;
    std::vector<std::complex<float>> tempBuffer;
};
