# Add the source files for our plugin to the "SharedCode" target,
# which is defined in the main CMakeLists.txt
target_sources(VisualGranularSynth_SharedCode
    PRIVATE
        PluginProcessor.cpp
        PluginEditor.cpp
)

# Create a library from our image files.
# This is the correct place for this command.
juce_add_binary_data(BinaryData
    FILES
        Resources/input_file_11.jpeg
        Resources/input_file_12.jpeg
        Resources/input_file_13.jpeg
)

# Link this new "BinaryData" library to our plugin so it can find the images.
target_link_libraries(VisualGranularSynth_SharedCode
    PRIVATE
        BinaryData
)