#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
    : AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                         .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
#endif
                         .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
                     ),
    parameters(*this, nullptr, "Parameters", createParameterLayout()),
    lowpassFilter(juce::dsp::IIR::Coefficients<float>::makeLowPass(44100, 20000.0f, 0.1f)),
    hipassFilter(juce::dsp::IIR::Coefficients<float>::makeHighPass(44100, 20000.0f, 0.1f))
{
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

//==============================================================================

juce::AudioProcessorValueTreeState::ParameterLayout AudioPluginAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    auto lowpassParam = std::make_unique<juce::AudioParameterFloat>(
        "lowpass",
        "Lowpass",
        juce::NormalisableRange<float>(20.0f, 20000.0f, 0.01f, 0.25),
        20000.0f
        );

    auto lpresParam = std::make_unique<juce::AudioParameterFloat>(
        "lpres",
        "Lowpass Resonance(Q)",
        0.1f,
        10.0f,
        1.0f
        );
    auto hipassParam = std::make_unique<juce::AudioParameterFloat>(
        "hipass",
        "Hipass",
        juce::NormalisableRange<float>(20.0f, 20000.0f, 0.01f, 0.25),
        20.0f
        );

    auto hpresParam = std::make_unique<juce::AudioParameterFloat>(
        "hpres",
        "Hipass Resonance(Q)",
        0.1f,
        10.0f,
        1.0f
        );

    params.push_back(std::move(lowpassParam));
    params.push_back(std::move(lpresParam));
    params.push_back(std::move(hipassParam));
    params.push_back(std::move(hpresParam));


    return { params.begin(), params.end() };
}
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;

    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumInputChannels();

    lowpassFilter.prepare(spec);
    lowpassFilter.reset();
    hipassFilter.prepare(spec);
    lowpassFilter.reset();
    updateFilter();
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}

void AudioPluginAudioProcessor::process(juce::dsp::ProcessContextReplacing<float> context)
{

}

void AudioPluginAudioProcessor::updateFilter()
{
    float lpfreq = *parameters.getRawParameterValue("lowpass");
    float lpres = *parameters.getRawParameterValue("lpres");
    float hpfreq = *parameters.getRawParameterValue("hipass");
    float hpres = *parameters.getRawParameterValue("hpres");

    *lowpassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(), lpfreq, lpres);
    *hipassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(getSampleRate(), hpfreq, hpres);
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                             juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    juce::dsp::AudioBlock<float> block(buffer);
    updateFilter();
    lowpassFilter.process(juce::dsp::ProcessContextReplacing<float> (block));
    hipassFilter.process(juce::dsp::ProcessContextReplacing<float> (block));

}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
