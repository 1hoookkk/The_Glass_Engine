Here’s a **clean, professional `README.md`** you can use right now for your VisualGranularSynth project:

---

```
# VisualGranularSynth

A next-generation VST3 instrument for drawing sound: paint images to directly shape spectra, grain clouds, and modulation fields in real-time.  
Inspired by MetaSynth, CDP, and the adventurous spirit of Aphex Twin—built for deep sound design and live performance.

---

## 🎛️ Features

- **Image-driven synthesis:** Paint masks and density maps to control grain rate, size, pitch, pan, and FFT-bin gain.
- **Spectral and granular fusion:** Combine spectral masking with SIMD-optimized granular processing.
- **Realtime discipline:** Zero locks, zero heap, zero exceptions on the audio thread.
- **GPU/CPU architecture:** GPU for image analysis and UI, CPU for all audio DSP—never the other way around.
- **Performance guarantees:** Designed for <5ms latency at 48kHz, with traceable, block-by-block timing.
- **Table-based ModMatrix:** Clear, fast, and live-editable modulation routing.
- **Accessibility:** Colorblind-safe UI palettes and scalable CPU modes.
- **Originality & ethics:** All core algorithms, mappings, and content are original or properly licensed.

---

## 📂 Project Structure

```

/source/         - Core engine, DSP, plugin code
/perf/           - Performance harness, profiling, metrics
/tests/          - Unit and regression tests
/scripts/        - Utilities for build/perf
/assets/         - Example images and control fields
/patches/        - Preset and demo patches

```

---

## 🚀 Quick Start

1. **Build:**
   - Requires JUCE (add as submodule or local folder, not tracked by Git).
   - `cmake -B build -S .`
   - `cmake --build build --config Release`

2. **Run Perf Harness:**  
   `./build/perf/PerfHarness.exe`  
   (Outputs `metrics.json` and `trace.json`)

3. **Load in your DAW** (VST3 build)

---

## 🧑‍💻 Real-Time Audio Constraints

- Audio thread: **no allocations, locks, or exceptions**  
- Modulation updates via triple-buffer (RCU pattern), GPU publishes at 30–60Hz
- Strict latency budgets per block:  
  - 64: ≤0.35ms
  - 128: ≤0.70ms
  - 256: ≤1.4ms

---

## 🛣️ Milestones

- **M0:** Granular engine MVP, perf benchmarks, clean threading
- **M1:** State recall, ModMatrix persistence
- **M2:** GPU image scanner to spectral mask
- **M3:** ModMatrix UI and macros
- **M4:** SIMD/Perf pass and release polish

---

## 📜 License

[MIT](LICENSE)  
(Factory content, images, and audio demos may be separately licensed.)

---

## 🙏 Credits & Inspiration

- MetaSynth (U&I Software) – direct sound drawing
- CDP (Composers’ Desktop Project) – spectral/microsound tools
- Aphex Twin – timbral curiosity and playability

---

**Draw sound. Build texture. Push the edge of performance.**

---

```

---
