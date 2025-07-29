#include "FFTWrapper.h"
#include <cmath>

FFTWrapper::FFTWrapper (int order)
    : order_(order),
      fftSize_(1 << order),
      fft_(order)
{
    buffer_.allocate (2 * fftSize_, true);
    window_.allocate (fftSize_, true);
    computeWindow();
}

void FFTWrapper::setOrder (int order)
{
    order_   = order;
    fftSize_ = 1 << order;
    fft_     = juce::dsp::FFT (order_);

    buffer_.allocate (2 * fftSize_, true);
    window_.allocate (fftSize_, true);
    computeWindow();
}

void FFTWrapper::computeWindow()
{
    // Hann window
    for (int n = 0; n < fftSize_; ++n)
        window_[n] = 0.5f * (1.0f - std::cos (2.0f * juce::MathConstants<float>::pi * n / (fftSize_ - 1)));
}

void FFTWrapper::applyWindow (float* data, int numSamples)
{
    const int N = juce::jmin (numSamples, fftSize_);
    for (int i = 0; i < N; ++i)
        data[i] *= window_[i];
}

void FFTWrapper::forward (const float* in, float* outReal, float* outImag)
{
    // Copy input to temp buffer (real parts), imag parts zeroed
    // buffer_ is [real0, imag0, real1, imag1, ...] for juce::FFT
    for (int i = 0; i < fftSize_; ++i)
    {
        buffer_[2 * i]     = in[i];
        buffer_[2 * i + 1] = 0.0f;
    }

    fft_.performRealOnlyForwardTransform (buffer_.getData());

    // After juce forward, buffer_[0] = Re[0], buffer_[1] = Re[N/2],
    // and pairs for k=1..N/2-1: buffer_[2k] = Re[k], buffer_[2k+1] = Im[k]
    outReal[0] = buffer_[0];
    outImag[0] = 0.0f;
    const int half = fftSize_ / 2;
    for (int k = 1; k < half; ++k)
    {
        outReal[k] = buffer_[2 * k];
        outImag[k] = buffer_[2 * k + 1];
    }
    outReal[half] = buffer_[1];
    outImag[half] = 0.0f;
}

void FFTWrapper::inverse (const float* inReal, const float* inImag, float* out)
{
    // Rebuild interleaved buffer for juce inverse
    // buffer_[0] = Re[0], buffer_[1] = Re[N/2]
    // buffer_[2k] = Re[k], buffer_[2k+1] = Im[k] for k=1..N/2-1

    const int half = fftSize_ / 2;
    buffer_[0] = inReal[0];
    buffer_[1] = inReal[half];
    for (int k = 1; k < half; ++k)
    {
        buffer_[2 * k]     = inReal[k];
        buffer_[2 * k + 1] = inImag[k];
    }

    fft_.performRealOnlyInverseTransform (buffer_.getData());

    // Output is real time-domain signal
    for (int i = 0; i < fftSize_; ++i)
        out[i] = buffer_[2 * i]; // imag part should be ~0
}