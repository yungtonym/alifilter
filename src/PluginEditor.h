#pragma once

#include "PluginProcessor.h"
#include "AliLookAndFeel.h"

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    AliLookAndFeel defaultLookAndFeel;

    juce::Slider getFilterCutOffDial() const;
    void setFilterCutOffDial(const juce::Slider &newFilterCutOffDial);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    juce::Slider filterLowpassDial;
    juce::Slider lowpassResDial;
    juce::Slider filterHipassDial;
    juce::Slider hipassResDial;
    juce::Label titleLabel;
    juce::Label lowpassLabel;
    juce::Label lowpassResLabel;
    juce::Label hipassLabel;
    juce::Label hipassResLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterLowpassValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowpassResValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterHipassValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> hipassResValue;

    AudioPluginAudioProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
