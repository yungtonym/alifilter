#include "AliLookAndFeel.h"

AliLookAndFeel::AliLookAndFeel()
{
    setColour(juce::Slider::trackColourId, juce::Colour::fromRGB(252, 252, 252));
    setColour(juce::Slider::thumbColourId, juce::Colour::fromRGB(25, 25, 27));
    setColour(juce::Label::textColourId, juce::Colour::fromRGB(25, 25, 27));
}

AliLookAndFeel::~AliLookAndFeel()
{
}

void AliLookAndFeel::drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                      float sliderPos, float minSliderPos, float maxSliderPos,
                      juce::Slider::SliderStyle style, juce::Slider& slider)
{
    auto trackWidth = 6.0f;
    auto thumbHeight = 40.0f;
    auto thumbWidth = 25.0f;
    sliderPos = juce::jlimit(y + thumbHeight / 2.0f, y + height - thumbHeight / 2.0f, sliderPos);

    g.setColour(this->findColour(juce::Slider::trackColourId));
    g.drawLine(x + width / 2.0f, y, x + width / 2.0f, y + height, trackWidth);

    g.setColour(this->findColour(juce::Slider::thumbColourId));
    g.fillRect(x + width / 2.0f - thumbWidth / 2.0f, sliderPos - thumbHeight / 2.0f,
               thumbWidth, thumbHeight);
}
void AliLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                      float sliderPosProportional, float rotaryStartAngle,
                                      float rotaryEndAngle, juce::Slider& slider)
{
    // Рассчитываем размеры и центр круга
    auto radius = juce::jmin(width / 2, height / 2) - 4.0f;
    auto centerX = x + width / 2;
    auto centerY = y + height / 2;

    // Рассчитываем угол поворота
    auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

    // Рисуем фоновый круг
    g.setColour(this->findColour(juce::Slider::trackColourId).withAlpha(0.3f)); // Полупрозрачный цвет
    g.fillEllipse(centerX - radius, centerY - radius, radius * 2, radius * 2);

    // Рисуем указатель
    juce::Path pointer;
    auto pointerLength = radius * 0.5f;
    auto pointerThickness = 6.0f;

    pointer.addRectangle(-pointerThickness / 2, -radius, pointerThickness, pointerLength);
    g.setColour(this->findColour(juce::Slider::thumbColourId));

    g.saveState();
    g.addTransform(juce::AffineTransform::rotation(angle).translated(centerX, centerY));
    g.fillPath(pointer);
    g.restoreState();
}
