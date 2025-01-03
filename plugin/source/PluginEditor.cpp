#include "scrambler/PluginEditor.h"

//==============================================================================
// CustomLookAndFeel Implementation
CustomLookAndFeel::CustomLookAndFeel()
{
    setColour(juce::Slider::thumbColourId, juce::Colours::lightblue);
    setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::lightblue);
    setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::darkgrey);
}

void CustomLookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height,
                                         float sliderPosProportional, float rotaryStartAngle,
                                         float rotaryEndAngle, juce::Slider &slider)
{
    // Convert integer parameters to float at the start
    auto bounds = juce::Rectangle<float>(static_cast<float>(x),
                                         static_cast<float>(y),
                                         static_cast<float>(width),
                                         static_cast<float>(height));

    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.4f;
    auto toAngle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
    auto center = bounds.getCentre();

    // Rest of the function remains the same
    g.setColour(slider.findColour(juce::Slider::rotarySliderOutlineColourId));
    g.drawEllipse(center.getX() - radius, center.getY() - radius,
                  radius * 2.0f, radius * 2.0f, 2.0f);

    g.setColour(slider.findColour(juce::Slider::rotarySliderFillColourId));
    juce::Path filledArc;
    filledArc.addArc(center.getX() - radius, center.getY() - radius,
                     radius * 2.0f, radius * 2.0f,
                     rotaryStartAngle, toAngle, true);
    g.strokePath(filledArc, juce::PathStrokeType(3.0f));

    juce::Path pointer;
    auto pointerLength = radius * 0.8f;
    auto pointerThickness = 3.0f;

    pointer.addRectangle(-pointerThickness * 0.5f, -radius,
                         pointerThickness, pointerLength);
    pointer.applyTransform(juce::AffineTransform::rotation(toAngle)
                               .translated(center.getX(), center.getY()));
    g.setColour(slider.findColour(juce::Slider::thumbColourId));
    g.fillPath(pointer);
}

//==============================================================================
// BasicReverbEditor Implementation
BasicReverbEditor::BasicReverbEditor(BasicReverbProcessor &p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(600, 400);

    // Initialize custom look and feel
    lookAndFeel = std::make_unique<CustomLookAndFeel>();

    // Configure sliders and labels
    const auto sliders = {&roomSizeSlider, &dampingSlider, &wetLevelSlider,
                          &dryLevelSlider, &widthSlider};
    const auto labels = {&roomSizeLabel, &dampingLabel, &wetLevelLabel,
                         &dryLevelLabel, &widthLabel};
    const auto names = {"Room Size", "Damping", "Wet Level", "Dry Level", "Width"};

    int idx = 0;
    for (auto *slider : sliders)
    {
        configureSlider(*slider);
        addAndMakeVisible(slider);

        auto *label = *(std::next(labels.begin(), idx));
        configureLabel(*label, *(std::next(names.begin(), idx)));
        addAndMakeVisible(label);

        idx++;
    }

    // Create parameter attachments
    const auto params = {"roomSize", "damping", "wetLevel", "dryLevel", "width"};
    idx = 0;
    for (const auto &param : params)
    {
        sliderAttachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            processor.apvts, param, **(std::next(sliders.begin(), idx++))));
    }

    // Configure preset box
    addAndMakeVisible(presetBox);
    presetBox.addItem("Default", 1);
    presetBox.addItem("Small Room", 2);
    presetBox.addItem("Large Hall", 3);
    presetBox.addItem("Cathedral", 4);
    presetBox.onChange = [this]
    { handlePresetChange(); };
    presetBox.setColour(juce::ComboBox::backgroundColourId, juce::Colours::darkgrey);
    presetBox.setColour(juce::ComboBox::textColourId, juce::Colours::white);
}

BasicReverbEditor::~BasicReverbEditor()
{
    setLookAndFeel(nullptr);
}

void BasicReverbEditor::paint(juce::Graphics &g)
{
    // Draw gradient background
    auto bounds = getLocalBounds();
    juce::ColourGradient gradient(
        juce::Colour(0x331EC12B),
        0.0f, 0.0f,
        juce::Colour(0xFF2D3F58),
        static_cast<float>(bounds.getWidth()),
        static_cast<float>(bounds.getHeight()),
        false);
    g.setGradientFill(gradient);
    g.fillAll();

    // Draw title
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions().withHeight(30.0f));
    g.drawText("scrambler by KKL", getLocalBounds().removeFromTop(40),
               juce::Justification::centred, false);
}

void BasicReverbEditor::resized()
{
    auto bounds = getLocalBounds().reduced(20);

    // Header section
    auto headerBounds = bounds.removeFromTop(60);
    presetBox.setBounds(headerBounds.removeFromRight(150).reduced(5));

    // Sliders section
    auto sliderArea = bounds.reduced(10);
    int sliderWidth = sliderArea.getWidth() / 5;

    const auto sliders = {&roomSizeSlider, &dampingSlider, &wetLevelSlider,
                          &dryLevelSlider, &widthSlider};
    const auto labels = {&roomSizeLabel, &dampingLabel, &wetLevelLabel,
                         &dryLevelLabel, &widthLabel};

    for (size_t i = 0; i < sliders.size(); ++i)
    {
        auto area = sliderArea.removeFromLeft(sliderWidth).reduced(5);
        (*(std::next(labels.begin(), i)))->setBounds(area.removeFromTop(20));
        (*(std::next(sliders.begin(), i)))->setBounds(area);
    }
}

void BasicReverbEditor::configureSlider(juce::Slider &slider)
{
    slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 90, 20);
    slider.setLookAndFeel(lookAndFeel.get());
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::greenyellow);
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::red);
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}

void BasicReverbEditor::configureLabel(juce::Label &label, const juce::String &text)
{
    label.setText(text, juce::dontSendNotification);
    label.setFont(juce::FontOptions().withHeight(15.0f));
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, juce::Colours::white);
}

void BasicReverbEditor::handlePresetChange()
{
    switch (presetBox.getSelectedId())
    {
    case 2: // Small Room
        setPresetValues(0.4f, 0.5f, 0.33f, 0.4f, 0.5f);
        break;
    case 3: // Large Hall
        setPresetValues(0.8f, 0.3f, 0.6f, 0.3f, 0.7f);
        break;
    case 4: // Cathedral
        setPresetValues(1.0f, 0.2f, 0.8f, 0.2f, 1.0f);
        break;
    default: // Default
        setPresetValues(0.5f, 0.5f, 0.5f, 0.5f, 0.5f);
    }
}

void BasicReverbEditor::setPresetValues(float room, float damp, float wet, float dry, float width)
{
    roomSizeSlider.setValue(room, juce::sendNotification);
    dampingSlider.setValue(damp, juce::sendNotification);
    wetLevelSlider.setValue(wet, juce::sendNotification);
    dryLevelSlider.setValue(dry, juce::sendNotification);
    widthSlider.setValue(width, juce::sendNotification);
}