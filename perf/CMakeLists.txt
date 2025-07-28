# perf/CMakeLists.txt
add_executable(VGSPerf
    PerfHarness.cpp
    BlockProfiler.h
)

# so it can see both JUCE headers & our source/*.h
target_include_directories(VGSPerf
    PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/../source
)

# link in our library (which already links JUCE modules)
target_link_libraries(VGSPerf
    PRIVATE VisualGranularSynthLib
)
