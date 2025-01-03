#pragma once
#include "PluginProcessor.h"

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel();
    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider &slider) override;
};

class BasicReverbEditor : public juce::AudioProcessorEditor
{
public:
    explicit BasicReverbEditor(BasicReverbProcessor &);
    ~BasicReverbEditor() override;

    void paint(juce::Graphics &) override;
    void resized() override;

private:
    void configureSlider(juce::Slider &);
    void configureLabel(juce::Label &, const juce::String &);
    void handlePresetChange();
    void setPresetValues(float room, float damp, float wet, float dry, float width);

    BasicReverbProcessor &processor;

    // UI Components
    juce::Slider roomSizeSlider;
    juce::Slider dampingSlider;
    juce::Slider wetLevelSlider;
    juce::Slider dryLevelSlider;
    juce::Slider widthSlider;

    juce::Label roomSizeLabel;
    juce::Label dampingLabel;
    juce::Label wetLevelLabel;
    juce::Label dryLevelLabel;
    juce::Label widthLabel;

    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> sliderAttachments;

    // New UI Components
    std::unique_ptr<CustomLookAndFeel> lookAndFeel;
    juce::ComboBox presetBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BasicReverbEditor)
};