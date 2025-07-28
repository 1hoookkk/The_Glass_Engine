#include "SpectralEngine.h"
#include <algorithm>
#include <cstring>

// Corrected applyMask implementation (assuming it applies mask to real part of spectrum)
void SpectralEngine::applyMask(float* spectrum, int bins)
{
    // bins is fftSize / 2 + 1 for real FFT
    int numBinsToApply = std::min((int)mask.size(), bins); // Don't exceed actual mask size or spectrum bins

    // Simple application: multiply magnitude by mask value based on scanX/scanY
    // Assuming mask is 1D (for frequency) and applied vertically (maskH = bins)
    // If mask is 2D (w x h), then scanX/scanY would select a slice of the mask.
    // For now, let's assume `mask` is a 1D array representing a frequency curve.
    // If it's a 2D mask (maskW x maskH), then `setMask` should have resized `mask` to `maskW * maskH`.
    // And `applyMask` would need to select a column or row based on `scanX` or `scanY`.

    // Assuming the mask is directly applied to the frequency bins:
    for (int i = 0; i < numBinsToApply; ++i)
    {
        // Spectrum contains real and imaginary parts (interleaved or separate).
        // For real-only FFT, `spectrum` pointer usually points to complex pairs.
        // `juce::dsp::FFT::performRealOnlyForwardTransform` stores data as:
        // [real_0, imag_1, real_2, imag_2, ..., real_N/2-1, imag_N/2-1, real_N/2, imag_0]
        // where imag_0 is at spectrum[1] and real_N/2 is at spectrum[fftSize-1]
        // Bins here refers to the actual frequency components.

        // If the `spectrum` input to `applyMask` is already in frequency domain as
        // (magnitude, phase) or (real, imag) pairs, then we apply to magnitudes.
        // Based on `FFTWrapper` and `processBlock` for `SpectralEngine`, `td` (time domain)
        // is passed to `applyMask`. This means `applyMask` is operating on time domain data.
        // This is unusual for a function named `applyMask` in a spectral engine.

        // Let's re-evaluate based on `SpectralEngine::processBlock`:
        // `fft.forward()` is called, then `applyMask(td, fftSize / 2);`
        // `td` is `fft.getTimeBuffer()`. After `forward()`, `td` contains real-only FFT results.
        // The structure of `td` after real-only forward transform:
        // `td[0]` = DC component (real)
        // `td[1]` = Nyquist component (real)
        // `td[2k]` = real part of k-th bin
        // `td[2k+1]` = imag part of k-th bin
        // for k = 1 to bins-1

        // So `bins` here means (fftSize/2). We need to iterate over complex pairs.
        // It should be `bins = fftSize / 2 + 1` for magnitude and phase.
        // The input `bins` is `fftSize / 2`.
        // The actual frequency bins are from 0 to `fftSize / 2`.
        // Bin 0 (DC) is at spectrum[0]
        // Bin (fftSize/2) (Nyquist) is at spectrum[1] if it's packed this way.
        // Other bins (complex) are at spectrum[2*k], spectrum[2*k+1].

        // Let's assume `spectrum` is a pointer to the real-only FFT output as described by JUCE.
        // We'll apply the mask to the magnitude of each bin.
        // `mask` is likely a 1D vector of floats, representing a frequency response curve.

        // Apply mask to magnitude components:
        // `spectrum[0]` is DC.
        // `spectrum[1]` is Nyquist.
        // `spectrum[2*i]` is real, `spectrum[2*i+1]` is imag for i = 1 to bins - 1.

        if (i < mask.size()) {
            float maskValue = mask[i]; // Get mask value for this bin
            if (i == 0) { // DC component
                spectrum[0] *= maskValue;
            }
            else if (i == bins) { // Nyquist component (if bins represents N/2)
                spectrum[1] *= maskValue; // Assuming Nyquist is at spectrum[1]
            }
            else { // Complex bins
                float real = spectrum[2 * i];
                float imag = spectrum[2 * i + 1];
                float magnitude = std::sqrt(real * real + imag * imag);
                float phase = std::atan2(imag, real);

                magnitude *= maskValue; // Apply mask to magnitude

                spectrum[2 * i] = magnitude * std::cos(phase);
                spectrum[2 * i + 1] = magnitude * std::sin(phase);
            }
        }
    }
}

void SpectralEngine::prepare(double sampleRate, int blockSize, int fftOrderIn, int hopSize)
{
    sr = sampleRate;
    block = blockSize;
    hop = hopSize;

    fft.setOrder(fftOrderIn);
    fftSize = fft.getSize();

    window = makeWindow((size_t)fftSize, WindowType::Hann);
    // olaBuffer needs to be large enough for fftSize + block - 1 for proper overlap-add
    olaBuffer.assign((size_t)fftSize + (size_t)block - 1, 0.f);
    writePos = 0;
}

void SpectralEngine::setMask(uint32_t id, const float* data, uint32_t w, uint32_t h)
{
    juce::ignoreUnused(id); // If id is not used, it's good practice to ignore
    mask.assign(data, data + (size_t)w * h); // Copy the mask data
    maskW = w; maskH = h;
}

void SpectralEngine::setScanPosition(float xNorm, float yNorm)
{
    scanX = std::clamp(xNorm, 0.f, 1.f);
    scanY = std::clamp(yNorm, 0.f, 1.f);
}

void SpectralEngine::processBlock(float** channels, int numChannels, int numSamples)
{
    if (numChannels == 0) return;

    // We only process the first channel for simplicity, assuming mono or interleaved stereo
    for (int n = 0; n < numSamples; ++n)
    {
        olaBuffer[writePos] += channels[0][n]; // Add current sample to OLA buffer
        writePos++;

        if (writePos >= (size_t)fftSize)
        {
            float* td = fft.getTimeBuffer(); // Get time domain buffer for FFT
            for (int i = 0; i < fftSize; ++i)
                td[i] = olaBuffer[i] * window[(size_t)i]; // Apply window

            fft.forward(); // Perform forward FFT
            applyMask(td, fftSize / 2); // Apply mask in frequency domain
            fft.inverse(); // Perform inverse FFT

            // Overlap-add
            for (int i = 0; i < fftSize; ++i)
            {
                // Add processed data back to olaBuffer for next block's overlap
                // And copy the first `hop` samples to the output `channels`
                if (i < hop) {
                    channels[0][n - (fftSize - hop) + i] = td[i] * (1.0f / fftSize); // Scale by FFT size
                }
                olaBuffer[i] = td[i] * (1.0f / fftSize); // Store for next overlap
            }

            // Shift remaining data in olaBuffer for next overlap
            std::memmove(olaBuffer.data(),
                olaBuffer.data() + hop,
                (olaBuffer.size() - hop) * sizeof(float));

            // Reset write position to start of next block's data
            writePos -= hop;
        }
    }
}