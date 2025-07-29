#include "PluginProcessor.h"
#include "PluginEditor.h"

ArtefactAudioProcessorEditor::ArtefactAudioProcessorEditor (ArtefactAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    // --- LOAD FONT ---
    auto typeface = juce::Typeface::createSystemTypefaceFor(BinaryData::SilkscreenRegular_ttf, BinaryData::SilkscreenRegular_ttfSize);
    pixelFont = juce::Font(typeface);
    pixelFont.setHeight(14.0f);

    // --- CANVAS ---
    canvas = std::make_unique<juce::ImageComponent>("Canvas");
    auto canvasImage = juce::ImageFileFormat::loadFrom(BinaryData::The_Grid_jpeg, BinaryData::The_Grid_jpegSize);
    if (canvasImage.isValid())
        canvas->setImage(canvasImage, juce::RectanglePlacement::stretchToFit);
    addAndMakeVisible(*canvas);

    // --- SAMPLER SLOTS ---
    for (int i = 0; i < 8; ++i)
    {
        auto slotButton = std::make_unique<juce::TextButton>("[SLOT " + juce::String(i + 1) + "]");
        slotButton->setClickingTogglesState(true);
        slotButton->setRadioGroupId(1); // Makes them act like radio buttons
        slotButton->setColour(juce::TextButton::textColourOnId, juce::Colours::yellow);
        slotButton->setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xff2a2a2a));
        slotButton->setColour(juce::TextButton::buttonColourId, juce::Colour(0xff2a2a2a));
        addAndMakeVisible(*slotButton);
        samplerSlots.add(std::move(slotButton));
    }
    samplerSlots[0]->setToggleState(true, juce::dontSendNotification);

    // --- KNOBS ---
    auto createKnob = [&](std::unique_ptr<juce::Slider>& knob, const juce::String& text)
    {
        knob = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox);
        knob->setLookAndFeel(&lookAndFeel);
        addAndMakeVisible(*knob);
    };
    createKnob(driveKnob, "DRIVE");
    createKnob(crushKnob, "CRUSH");
    createKnob(filterKnob, "FILTER");
    createKnob(jitterKnob, "JITTER");

    // --- LABELS ---
    auto createLabel = [&](std::unique_ptr<juce::Label>& label, const juce::String& text) {
        label = std::make_unique<juce::Label>("", text);
        label->setFont(pixelFont);
        label->setColour(juce::Label::textColourId, juce::Colours::cyan);
        label->setJustificationType(juce::Justification::centred);
        addAndMakeVisible(*label);
    };
    createLabel(driveLabel, "DRIVE");
    createLabel(crushLabel, "CRUSH");
    createLabel(filterLabel, "FILTER");
    createLabel(jitterLabel, "JITTER");
    
    // --- ATTACHMENTS (Links knobs to parameters) ---
    driveAttachment = std::make_unique<SliderAttachment>(processorRef.apvts, "DRIVE", *driveKnob);
    crushAttachment = std::make_unique<SliderAttachment>(processorRef.apvts, "CRUSH", *crushKnob);
    filterAttachment = std::make_unique<SliderAttachment>(processorRef.apvts, "FILTER", *filterKnob);
    jitterAttachment = std::make_unique<SliderAttachment>(processorRef.apvts, "JITTER", *jitterKnob);

    setSize (800, 600);
}

ArtefactAudioProcessorEditor::~ArtefactAudioProcessorEditor()
{
    // Unset the look and feel to avoid dangling pointers
    driveKnob->setLookAndFeel(nullptr);
    crushKnob->setLookAndFeel(nullptr);
    filterKnob->setLookAndFeel(nullptr);
    jitterKnob->setLookAndFeel(nullptr);
}

void ArtefactAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour(0xff2a2a2a)); // Dark charcoal background

    // Add a subtle noise layer
    juce::Random rand;
    for (int i = 0; i < 2000; ++i)
    {
        g.setColour(juce::Colour(0x08ffffff)); // Very transparent white
        g.setPixel(rand.nextInt(getWidth()), rand.nextInt(getHeight()));
    }
}

void ArtefactAudioProcessorEditor::resized()
{
    juce::Rectangle<int> bounds = getLocalBounds().reduced(10);
    
    // Main layout
    juce::FlexBox mainBox;
    mainBox.flexDirection = juce::FlexBox::Direction::row;

    // Left Column
    juce::FlexBox canvasBox;
    canvasBox.flexDirection = juce::FlexBox::Direction::column;
    canvasBox.items.add(juce::FlexItem(*canvas).withFlex(1.0f)); // Canvas takes all vertical space
    mainBox.items.add(juce::FlexItem(canvasBox).withFlex(0.65f)); // Canvas box takes 65% of width

    // Right Column
    juce::FlexBox rightColumnBox;
    rightColumnBox.flexDirection = juce::FlexBox::Direction::column;
    
    juce::FlexBox slotBox;
    slotBox.flexDirection = juce::FlexBox::Direction::column;
    for(auto& slot : samplerSlots)
        slotBox.items.add(juce::FlexItem(*slot).withFlex(1.0f).withMargin(2));
    
    juce::FlexBox knobBox;
    knobBox.flexDirection = juce::FlexBox::Direction::row;
    
    auto createKnobItem = [&](juce::Slider& knob, juce::Label& label) {
        juce::FlexBox itemBox;
        itemBox.flexDirection = juce::FlexBox::Direction::column;
        itemBox.items.add(juce::FlexItem(knob).withFlex(0.8f));
        itemBox.items.add(juce::FlexItem(label).withFlex(0.2f));
        return juce::FlexItem(itemBox).withFlex(1.0f);
    };

    knobBox.items.add(createKnobItem(*driveKnob, *driveLabel));
    knobBox.items.add(createKnobItem(*crushKnob, *crushLabel));
    knobBox.items.add(createKnobItem(*filterKnob, *filterLabel));
    knobBox.items.add(createKnobItem(*jitterKnob, *jitterLabel));

    rightColumnBox.items.add(juce::FlexItem(slotBox).withFlex(0.6f));
    rightColumnBox.items.add(juce::FlexItem(knobBox).withFlex(0.4f).withMargin(juce::FlexItem::Margin(10,0,0,0)));

    mainBox.items.add(juce::FlexItem(rightColumnBox).withFlex(0.35f).withMargin(juce::FlexItem::Margin(0,0,0,10)));

    mainBox.performLayout(bounds);
}