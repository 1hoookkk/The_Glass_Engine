#include "FFTWrapper.h"
#include <cmath>

FFTWrapper::FFTWrapper(int order)
    : fftOrder(order),
      fftSize(1 << order),
      fft(std::make_unique<juce::dsp::FFT>(order)),
      window(fftSize, 1.0f),
      tempBuffer(fftSize)
{
}

FFTWrapper::~FFTWrapper() = default;

void FFTWrapper::prepare(WindowType windowType)
{
    const int N = fftSize;
    switch (windowType)
    {
        case Hann:
            for (int i = 0; i < N; ++i)
                window[i] = 0.5f * (1.0f - std::cos(2.0f * juce::MathConstants<float>::pi * i / (N - 1)));
            break;
        case Hamming:
            for (int i = 0; i < N; ++i)
                window[i] = 0.54f - 0.46f * std::cos(2.0f * juce::MathConstants<float>::pi * i / (N - 1));
            break;
        case Blackman:
            for (int i = 0; i < N; ++i)
                window[i] = 0.42f - 0.5f * std::cos(2.0f * juce::MathConstants<float>::pi * i / (N - 1))
                          + 0.08f * std::cos(4.0f * juce::MathConstants<float>::pi * i / (N - 1));
            break;
    }
}

void FFTWrapper::applyWindow(float* data)
{
    for (int i = 0; i < fftSize; ++i)
        data[i] *= window[i];
}

void FFTWrapper::performFFT(const float* timeData, std::vector<std::complex<float>>& freqData)
{
    // Copy time data into tempBuffer with window
    for (int i = 0; i < fftSize; ++i)
        tempBuffer[i] = std::complex<float>(timeData[i] * window[i], 0.0f);

    // Perform forward FFT in-place (complex)
    fft->performFullForwardTransform(reinterpret_cast<float*>(tempBuffer.data()));

    // Copy results to freqData
    freqData.resize(fftSize);
    std::copy(tempBuffer.begin(), tempBuffer.end(), freqData.begin());
}

void FFTWrapper::performIFFT(const std::vector<std::complex<float>>& freqData, float* timeData)
{
    // Copy freq data to tempBuffer
    for (int i = 0; i < fftSize; ++i)
        tempBuffer[i] = freqData[i];

    // Perform inverse FFT in-place
    fft->performFullInverseTransform(reinterpret_cast<float*>(tempBuffer.data()));

    // Extract real part and normalize by fftSize
    for (int i = 0; i < fftSize; ++i)
        timeData[i] = tempBuffer[i].real() / static_cast<float>(fftSize);
}

void FFTWrapper::cartesianToPolar(const std::vector<std::complex<float>>& freqData,
                                  std::vector<float>& magnitudes,
                                  std::vector<float>& phases) const
{
    int N = fftSize;
    magnitudes.resize(N);
    phases.resize(N);
    for (int i = 0; i < N; ++i)
    {
        const auto& c = freqData[i];
        magnitudes[i] = std::abs(c);
        phases[i]     = std::arg(c);
    }
}

void FFTWrapper::polarToCartesian(const std::vector<float>& magnitudes,
                                  const std::vector<float>& phases,
                                  std::vector<std::complex<float>>& freqData) const
{
    int N = fftSize;
    freqData.resize(N);
    for (int i = 0; i < N; ++i)
        freqData[i] = std::polar(magnitudes[i], phases[i]);
}
