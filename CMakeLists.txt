cmake_minimum_required(VERSION 3.16)
project(VisualGranularSynth VERSION 0.1.0 LANGUAGES CXX)

# —————————————————————————————————————————
# 1) C++ settings
# —————————————————————————————————————————
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# —————————————————————————————————————————
# 2) JUCE (must be cloned into JUCE/)
# —————————————————————————————————————————
if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/JUCE/CMakeLists.txt")
  message(FATAL_ERROR "Please git-clone https://github.com/juce-framework/JUCE.git into JUCE/")
endif()
add_subdirectory(JUCE)

# —————————————————————————————————————————
# 3) Core DSP/Engine library
# —————————————————————————————————————————
add_library(VisualGranularSynthLib STATIC
  source/dsp/granular/GranularEngine.cpp
  source/dsp/granular/GranularEngine.h
  source/dsp/granular/Grain.h
  source/dsp/granular/GrainPool.h
  source/dsp/granular/WindowTable.h
  source/core/ModMatrix.h
  source/core/ModSource.h
  source/core/ModRoute.h
)

target_include_directories(VisualGranularSynthLib
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/source
)

target_link_libraries(VisualGranularSynthLib
  PUBLIC
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_gui_basics
    juce::juce_graphics
    juce::juce_core
)

# Important: Define VST2 as disabled for the library too
target_compile_definitions(VisualGranularSynthLib
  PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
    JucePlugin_Build_VST=0
    JucePlugin_Build_VST3=1
)

# —————————————————————————————————————————
# 4) VST3 plug-in target
# —————————————————————————————————————————
juce_add_plugin(VisualGranularSynth
  COMPANY_NAME     "VGS"
  PRODUCT_NAME     "VisualGranularSynth"
  PLUGIN_MANUFACTURER_CODE "VGSM"
  PLUGIN_CODE "VGS1"
  IS_SYNTH         TRUE
  NEEDS_MIDI_INPUT TRUE
  NEEDS_MIDI_OUTPUT FALSE
  EDITOR_WANTS_KEYBOARD_FOCUS TRUE

  # CRITICAL: Only build VST3
  FORMATS VST3

  # Additional plugin properties
  VST3_CATEGORIES "Instrument" "Synth" "Granular"
)

# Add sources after juce_add_plugin
target_sources(VisualGranularSynth
  PRIVATE
    source/plugin/PluginProcessor.cpp
    source/plugin/PluginEditor.cpp
    source/plugin/PluginProcessor.h
    source/plugin/PluginEditor.h
)

# Link to our engine library
target_link_libraries(VisualGranularSynth
  PRIVATE 
    VisualGranularSynthLib
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

# Critical compile definitions to avoid VST2/VST3 conflicts
target_compile_definitions(VisualGranularSynth
  PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JucePlugin_Build_VST=0         # Explicitly disable VST2
    JucePlugin_Build_VST3=1        # Enable VST3
    JucePlugin_Build_AU=0          # Disable AU
    JucePlugin_Build_AUv3=0        # Disable AUv3
    JucePlugin_Build_RTAS=0        # Disable RTAS
    JucePlugin_Build_AAX=0         # Disable AAX
    JucePlugin_Build_Standalone=0  # Disable Standalone
    JucePlugin_Build_Unity=0       # Disable Unity
    JucePlugin_Build_LV2=0         # Disable LV2
)

# Performance optimization flags
if(MSVC)
    target_compile_options(VisualGranularSynth PRIVATE 
        /O2 /fp:fast /arch:AVX2
    )
    target_compile_options(VisualGranularSynthLib PRIVATE 
        /O2 /fp:fast /arch:AVX2
    )
endif()

# —————————————————————————————————————————
# 5) Performance harness executable
# —————————————————————————————————————————
add_executable(PerfHarness
    perf/PerfHarness.cpp
    perf/BlockProfiler.h
)

target_include_directories(PerfHarness PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/source
    ${CMAKE_CURRENT_SOURCE_DIR}/perf
)

target_link_libraries(PerfHarness
    PRIVATE
        VisualGranularSynthLib
        juce::juce_audio_utils
        juce::juce_audio_processors
)

target_compile_definitions(PerfHarness
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_STANDALONE_APPLICATION=1
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JucePlugin_Build_VST=0
        JucePlugin_Build_VST3=1
)

# —————————————————————————————————————————
# 6) Unit tests (optional)
# —————————————————————————————————————————
option(BUILD_TESTS "Build unit tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_executable(VGSTests
        tests/GranularEngineTests.cpp
    )
    target_link_libraries(VGSTests PRIVATE
        VisualGranularSynthLib
        juce::juce_core
        juce::juce_audio_basics
    )
    add_test(NAME GranularTests COMMAND VGSTests)
endif()