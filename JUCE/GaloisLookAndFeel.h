#pragma once
#include<JuceHeader.h>

class GaloisLookAndFeel : public juce::LookAndFeel_V4
{
public:
    GaloisLookAndFeel();

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, 
        float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override;

    void drawSliderArc(
        juce::Graphics& g,
        int rx,
        int ry,
        int rw,
        int rh,
        float rotaryStartAngle,
        float rotaryEndAngle,
        float angle,
        float sliderPos,
        float proportion
    );
};

class GaloisLookAndFeelCentred : public GaloisLookAndFeel
{
public:
    GaloisLookAndFeelCentred();

    void drawSliderArc(
        juce::Graphics& g,
        int rx,
        int ry,
        int rw,
        int rh,
        float rotaryStartAngle,
        float rotaryEndAngle,
        float angle,
        float sliderPos,
        float proportion
    ) override;

};