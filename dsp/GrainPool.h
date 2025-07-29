```cpp
#pragma once
#include <immintrin.h>
#include <cstring>
#include <cstdint>
#include <algorithm>

class GrainPool {
public:
    static constexpr size_t SIMD_WIDTH = 8;
    static constexpr size_t MAX_GRAINS = 512;
    static constexpr size_t ALIGNED_SIZE = ((MAX_GRAINS + SIMD_WIDTH - 1) / SIMD_WIDTH) * SIMD_WIDTH;

    GrainPool(size_t maxGrains = MAX_GRAINS) 
        : capacity(std::min(maxGrains, MAX_GRAINS))
        , activeCount(0)
        , nextSlot(0) {
        
        position = static_cast<float*>(_mm_malloc(ALIGNED_SIZE * sizeof(float), 32));
        startPos = static_cast<float*>(_mm_malloc(ALIGNED_SIZE * sizeof(float), 32));
        pitch = static_cast<float*>(_mm_malloc(ALIGNED_SIZE * sizeof(float), 32));
        gain = static_cast<float*>(_mm_malloc(ALIGNED_SIZE * sizeof(float), 32));
        panL = static_cast<float*>(_mm_malloc(ALIGNED_SIZE * sizeof(float), 32));
        panR = static_cast<float*>(_mm_malloc(ALIGNED_SIZE * sizeof(float), 32));
        age = static_cast<float*>(_mm_malloc(ALIGNED_SIZE * sizeof(float), 32));
        duration = static_cast<float*>(_mm_malloc(ALIGNED_SIZE * sizeof(float), 32));
        windowPhase = static_cast<float*>(_mm_malloc(ALIGNED_SIZE * sizeof(float), 32));
        active = static_cast<uint32_t*>(_mm_malloc(ALIGNED_SIZE * sizeof(uint32_t), 32));
        
        std::memset(active, 0, ALIGNED_SIZE * sizeof(uint32_t));
        std::memset(age, 0, ALIGNED_SIZE * sizeof(float));
    }

    ~GrainPool() {
        _mm_free(position);
        _mm_free(startPos);
        _mm_free(pitch);
        _mm_free(gain);
        _mm_free(panL);
        _mm_free(panR);
        _mm_free(age);
        _mm_free(duration);
        _mm_free(windowPhase);
        _mm_free(active);
    }

    int allocateGrain(float startPosition, float grainPitch, float grainGain, 
                      float grainPan, float grainDuration) noexcept {
        if (activeCount >= capacity) {
            int oldest = 0;
            float maxAge = 0.0f;
            for (size_t i = 0; i < capacity; ++i) {
                if (active[i] && age[i] > maxAge) {
                    maxAge = age[i];
                    oldest = i;
                }
            }
            active[oldest] = 0;
            --activeCount;
            nextSlot = oldest;
        }

        size_t slot = nextSlot;
        for (size_t i = 0; i < capacity; ++i) {
            size_t idx = (slot + i) % capacity;
            if (!active[idx]) {
                position[idx] = startPosition;
                startPos[idx] = startPosition;
                pitch[idx] = grainPitch;
                gain[idx] = grainGain;
                panL[idx] = std::sqrt(1.0f - grainPan) * 0.7071f;
                panR[idx] = std::sqrt(1.0f + grainPan) * 0.7071f;
                age[idx] = 0.0f;
                duration[idx] = grainDuration;
                windowPhase[idx] = 0.0f;
                active[idx] = 1;
                nextSlot = (idx + 1) % capacity;
                ++activeCount;
                return idx;
            }
        }
        return -1;
    }

    void processBlock(const float* sourceBuffer, size_t sourceLength,
                      float* outputL, float* outputR, size_t numSamples,
                      double sampleRate, const float* windowTable, size_t windowSize) noexcept {
        
        const __m256 ones = _mm256_set1_ps(1.0f);
        const __m256 zeros = _mm256_setzero_ps();
        const __m256 sourceLen = _mm256_set1_ps(static_cast<float>(sourceLength));
        const __m256 windowSizeF = _mm256_set1_ps(static_cast<float>(windowSize - 1));
        const __m256 sampleRateInv = _mm256_set1_ps(1.0f / static_cast<float>(sampleRate));

        for (size_t sample = 0; sample < numSamples; ++sample) {
            __m256 sumL = _mm256_setzero_ps();
            __m256 sumR = _mm256_setzero_ps();

            for (size_t g = 0; g < capacity; g += SIMD_WIDTH) {
                __m256i activeVec = _mm256_load_si256(reinterpret_cast<const __m256i*>(&active[g]));
                __m256 activeMask = _mm256_castsi256_ps(_mm256_cmpgt_epi32(activeVec, _mm256_setzero_si256()));

                __m256 pos = _mm256_load_ps(&position[g]);
                __m256 pitchVec = _mm256_load_ps(&pitch[g]);
                __m256 gainVec = _mm256_load_ps(&gain[g]);
                __m256 panLVec = _mm256_load_ps(&panL[g]);
                __m256 panRVec = _mm256_load_ps(&panR[g]);
                __m256 ageVec = _mm256_load_ps(&age[g]);
                __m256 durVec = _mm256_load_ps(&duration[g]);
                __m256 winPhase = _mm256_load_ps(&windowPhase[g]);

                __m256i posInt = _mm256_cvttps_epi32(pos);
                __m256 frac = _mm256_sub_ps(pos, _mm256_cvtepi32_ps(posInt));

                alignas(32) int posIndices[8];
                _mm256_store_si256(reinterpret_cast<__m256i*>(posIndices), posInt);

                __m256 samp0 = _mm256_set_ps(
                    sourceBuffer[posIndices[7] % sourceLength],
                    sourceBuffer[posIndices[6] % sourceLength],
                    sourceBuffer[posIndices[5] % sourceLength],
                    sourceBuffer[posIndices[4] % sourceLength],
                    sourceBuffer[posIndices[3] % sourceLength],
                    sourceBuffer[posIndices[2] % sourceLength],
                    sourceBuffer[posIndices[1] % sourceLength],
                    sourceBuffer[posIndices[0] % sourceLength]
                );

                __m256 samp1 = _mm256_set_ps(
                    sourceBuffer[(posIndices[7] + 1) % sourceLength],
                    sourceBuffer[(posIndices[6] + 1) % sourceLength],
                    sourceBuffer[(posIndices[5] + 1) % sourceLength],
                    sourceBuffer[(posIndices[4] + 1) % sourceLength],
                    sourceBuffer[(posIndices[3] + 1) % sourceLength],
                    sourceBuffer[(posIndices[2] + 1) % sourceLength],
                    sourceBuffer[(posIndices[1] + 1) % sourceLength],
                    sourceBuffer[(posIndices[0] + 1) % sourceLength]
                );

                __m256 sample = _mm256_add_ps(samp0, _mm256_mul_ps(frac, _mm256_sub_ps(samp1, samp0)));

                __m256 winIdx = _mm256_mul_ps(winPhase, windowSizeF);
                __m256i winIdxInt = _mm256_cvttps_epi32(winIdx);
                __m256 winFrac = _mm256_sub_ps(winIdx, _mm256_cvtepi32_ps(winIdxInt));

                alignas(32) int winIndices[8];
                _mm256_store_si256(reinterpret_cast<__m256i*>(winIndices), winIdxInt);

                __m256 win0 = _mm256_set_ps(
                    windowTable[winIndices[7]],
                    windowTable[winIndices[6]],
                    windowTable[winIndices[5]],
                    windowTable[winIndices[4]],
                    windowTable[winIndices[3]],
                    windowTable[winIndices[2]],
                    windowTable[winIndices[1]],
                    windowTable[winIndices[0]]
                );

                __m256 win1 = _mm256_set_ps(
                    windowTable[std::min(winIndices[7] + 1, static_cast<int>(windowSize - 1))],
                    windowTable[std::min(winIndices[6] + 1, static_cast<int>(windowSize - 1))],
                    windowTable[std::min(winIndices[5] + 1, static_cast<int>(windowSize - 1))],
                    windowTable[std::min(winIndices[4] + 1, static_cast<int>(windowSize - 1))],
                    windowTable[std::min(winIndices[3] + 1, static_cast<int>(windowSize - 1))],
                    windowTable[std::min(winIndices[2] + 1, static_cast<int>(windowSize - 1))],
                    windowTable[std::min(winIndices[1] + 1, static_cast<int>(windowSize - 1))],
                    windowTable[std::min(winIndices[0] + 1, static_cast<int>(windowSize - 1))]
                );

                __m256 window = _mm256_add_ps(win0, _mm256_mul_ps(winFrac, _mm256_sub_ps(win1, win0)));

                sample = _mm256_mul_ps(sample, _mm256_mul_ps(gainVec, window));
                sample = _mm256_and_ps(sample, activeMask);

                sumL = _mm256_add_ps(sumL, _mm256_mul_ps(sample, panLVec));
                sumR = _mm256_add_ps(sumR, _mm256_mul_ps(sample, panRVec));

                pos = _mm256_add_ps(pos, pitchVec);
                pos = _mm256_and_ps(pos, _mm256_cmp_ps(pos, sourceLen, _CMP_LT_OQ));
                
                ageVec = _mm256_add_ps(ageVec, ones);
                winPhase = _mm256_div_ps(ageVec, durVec);

                __m256 stillActive = _mm256_and_ps(activeMask, _mm256_cmp_ps(ageVec, durVec, _CMP_LT_OQ));
                activeVec = _mm256_castps_si256(stillActive);

                _mm256_store_ps(&position[g], pos);
                _mm256_store_ps(&age[g], ageVec);
                _mm256_store_ps(&windowPhase[g], winPhase);
                _mm256_store_si256(reinterpret_cast<__m256i*>(&active[g]), activeVec);
            }

            alignas(32) float sumLArr[8];
            alignas(32) float sumRArr[8];
            _mm256_store_ps(sumLArr, sumL);
            _mm256_store_ps(sumRArr, sumR);

            float outL = 0.0f, outR = 0.0f;
            for (int i = 0; i < 8; ++i) {
                outL += sumLArr[i];
                outR += sumRArr[i];
            }

            outputL[sample] += outL;
            outputR[sample] += outR;
        }

        activeCount = 0;
        for (size_t i = 0; i < capacity; ++i) {
            if (active[i]) ++activeCount;
        }
    }

    size_t getActiveGrainCount() const noexcept { return activeCount; }
    size_t getCapacity() const noexcept { return capacity; }

private:
    alignas(32) float* position;
    alignas(32) float* startPos;
    alignas(32) float* pitch;
    alignas(32) float* gain;
    alignas(32) float* panL;
    alignas(32) float* panR;
    alignas(32) float* age;
    alignas(32) float* duration;
    alignas(32) float* windowPhase;
    alignas(32) uint32_t* active;

    size_t capacity;
    size_t activeCount;
    size_t nextSlot;
};
```