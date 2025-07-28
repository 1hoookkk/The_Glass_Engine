add_executable(unit_tests placeholder.cpp)
target_link_libraries(unit_tests PRIVATE VisualGranularSynthLib juce::juce_audio_basics)
add_test(NAME unit_tests COMMAND unit_tests)
