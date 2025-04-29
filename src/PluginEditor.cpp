#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    juce::File fontFile ("/home/aleksei-klopot/Fonts/MiriamLibre-VariableFont_wght.ttf");
    juce::Font globalCustomFont;
    juce::Font globalCustomKnobsFont;
    if (fontFile.existsAsFile())
    {
        juce::MemoryBlock fontData;
        fontFile.loadFileAsData(fontData);
        auto typeface = juce::Typeface::createSystemTypefaceFor(fontData.getData(), fontData.getSize());
        juce::Font customFont(juce::FontOptions()
                                  .withTypeface(typeface)
                                  .withHeight(60.0f));
        juce::Font customKnobsFont(juce::FontOptions()
                                       .withTypeface(typeface)
                                       .withHeight(32.0f));
        globalCustomFont = customFont;
        globalCustomKnobsFont = customKnobsFont;
    }
    titleLabel.setText("Alifilter", juce::dontSendNotification);
    titleLabel.setFont(globalCustomFont);
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setLookAndFeel(&defaultLookAndFeel);
    addAndMakeVisible(titleLabel);

    filterLowpassDial.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    filterLowpassDial.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 25);
    filterLowpassDial.setTextValueSuffix(" Hz");
    filterLowpassDial.setLookAndFeel(&defaultLookAndFeel);
    addAndMakeVisible(filterLowpassDial);
    filterLowpassValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.parameters, "lowpass", filterLowpassDial);

    lowpassLabel.setText("lowpass", juce::dontSendNotification);
    lowpassLabel.setFont(globalCustomKnobsFont);
    lowpassLabel.setJustificationType(juce::Justification::centred);
    lowpassLabel.setLookAndFeel(&defaultLookAndFeel);
    lowpassLabel.attachToComponent(&filterLowpassDial, false);
    addAndMakeVisible(lowpassLabel);

    lowpassResDial.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    lowpassResDial.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 25);
    lowpassResDial.setNumDecimalPlacesToDisplay(2); //???
    lowpassResDial.setLookAndFeel(&defaultLookAndFeel);
    addAndMakeVisible(lowpassResDial);
    lowpassResValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.parameters, "lpres", lowpassResDial);


    lowpassResLabel.setText("lp Q", juce::dontSendNotification);
    lowpassResLabel.setFont(globalCustomKnobsFont);
    lowpassResLabel.setJustificationType(juce::Justification::centred);
    lowpassResLabel.setLookAndFeel(&defaultLookAndFeel);
    lowpassResLabel.attachToComponent(&lowpassResDial, false);
    addAndMakeVisible(lowpassResLabel);

    filterHipassDial.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    filterHipassDial.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 25);
    filterHipassDial.setTextValueSuffix(" Hz");
    filterHipassDial.setLookAndFeel(&defaultLookAndFeel);
    addAndMakeVisible(filterHipassDial);
    filterHipassValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.parameters, "hipass", filterHipassDial);

    hipassLabel.setText("hipass", juce::dontSendNotification);
    hipassLabel.setFont(globalCustomKnobsFont);
    hipassLabel.setJustificationType(juce::Justification::centred);
    hipassLabel.setLookAndFeel(&defaultLookAndFeel);
    hipassLabel.attachToComponent(&filterHipassDial, false);
    addAndMakeVisible(hipassLabel);

    hipassResDial.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    hipassResDial.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 25);
    hipassResDial.setNumDecimalPlacesToDisplay(2); //???
    hipassResDial.setLookAndFeel(&defaultLookAndFeel);
    addAndMakeVisible(hipassResDial);
    hipassResValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.parameters, "hpres", hipassResDial);

    hipassResLabel.setText("hp Q", juce::dontSendNotification);
    hipassResLabel.setFont(globalCustomKnobsFont);
    hipassResLabel.setJustificationType(juce::Justification::centred);
    hipassResLabel.setLookAndFeel(&defaultLookAndFeel);
    hipassResLabel.attachToComponent(&hipassResDial, false);
    addAndMakeVisible(hipassLabel);


    setSize (800, 400);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    juce::ColourGradient gradient(
        juce::Colour::fromRGB(252, 252, 252),
        getWidth() / 2.0f, 0.0f,
        juce::Colour::fromRGB(159, 160, 162),
        getWidth() / 2.0f, getHeight(),
        false
        );
    g.setGradientFill(gradient);
    g.fillAll();

}
void AudioPluginAudioProcessorEditor::resized()
{
    auto area = getLocalBounds(); // Получаем всю доступную область окна

    // Создаем область для заголовка (например, Label)
    auto labelArea = area.removeFromTop(area.getHeight() / 4); // 20% высоты для заголовка
    titleLabel.setBounds(labelArea);

    // Оставшаяся область будет использоваться для knobs
    auto dialArea = area.reduced(40); // Уменьшаем отступы по краям на 20 пикселей

    // Разделяем область на две части для двух ручек
    auto knobWidth = dialArea.getWidth() / 4; // Каждая ручка занимает треть ширины
    auto row = dialArea;
    filterLowpassDial.setBounds(row.removeFromLeft(knobWidth).reduced(40));
    lowpassResDial.setBounds(row.removeFromLeft(knobWidth).reduced(40));
    filterHipassDial.setBounds(row.removeFromLeft(knobWidth).reduced(40));
    hipassResDial.setBounds(row.removeFromLeft(knobWidth).reduced(40));


}
