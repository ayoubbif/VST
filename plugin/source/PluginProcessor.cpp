#include "scrambler/PluginProcessor.h"
#include "scrambler/PluginEditor.h"

BasicReverbProcessor::BasicReverbProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    // Initialize parameters with default values
    reverbParams.roomSize = 0.5f;
    reverbParams.damping = 0.5f;
    reverbParams.wetLevel = 0.33f;
    reverbParams.dryLevel = 0.4f;
    reverbParams.width = 1.0f;
    reverb.setParameters(reverbParams);
}

juce::AudioProcessorValueTreeState::ParameterLayout BasicReverbProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "roomSize", "Room Size", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "damping", "Damping", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "wetLevel", "Wet Level", 0.0f, 1.0f, 0.33f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "dryLevel", "Dry Level", 0.0f, 1.0f, 0.4f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "width", "Width", 0.0f, 1.0f, 1.0f));

    return {params.begin(), params.end()};
}

void BasicReverbProcessor::updateReverbParameters()
{
    reverbParams.roomSize = apvts.getRawParameterValue("roomSize")->load();
    reverbParams.damping = apvts.getRawParameterValue("damping")->load();
    reverbParams.wetLevel = apvts.getRawParameterValue("wetLevel")->load();
    reverbParams.dryLevel = apvts.getRawParameterValue("dryLevel")->load();
    reverbParams.width = apvts.getRawParameterValue("width")->load();

    reverb.setParameters(reverbParams);
}

BasicReverbProcessor::~BasicReverbProcessor() {}

void BasicReverbProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumInputChannels());

    reverb.prepare(spec);
}

void BasicReverbProcessor::releaseResources() {}

void BasicReverbProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &)
{
    juce::ScopedNoDenormals noDenormals;

    updateReverbParameters();

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    reverb.process(context);
}

juce::AudioProcessorEditor *BasicReverbProcessor::createEditor()
{
    return new BasicReverbEditor(*this);
}

void BasicReverbProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void BasicReverbProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr)
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new BasicReverbProcessor();
}