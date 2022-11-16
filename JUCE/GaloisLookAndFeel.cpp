#pragma once

#include <JuceHeader.h>

class GaloisLookAndFeel : public juce::LookAndFeel_V4
{
public:
    GaloisLookAndFeel()
    {
    }

    // Pasted from https://github.com/remberg/juceCustomSliderSample
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
        float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override {
        const float radius = juce::jmin(width / 2, height / 2) * 0.85f;
        const float centreX = x + width * 0.5f;
        const float centreY = y + height * 0.5f;
        const float rx = centreX - radius;
        const float ry = centreY - radius;
        const float rw = radius * 2.0f;
        float fwidth = (float)width;
        float fheight = (float)height;
        const float angle = rotaryStartAngle
            + sliderPos
            * (rotaryEndAngle - rotaryStartAngle);

        g.setColour(juce::Colour(0xff213246));
        juce::Path filledArc;
        filledArc.addPieSegment(rx, ry, rw + 1, rw + 1, rotaryStartAngle, rotaryEndAngle, 0.6);

        g.fillPath(filledArc);

        g.setColour(juce::Colours::lightgreen);
        juce::Path filledArc1;
        filledArc1.addPieSegment(rx, ry, rw + 1, rw + 1, rotaryStartAngle, angle, 0.6);

        g.fillPath(filledArc1);
        
        // Pointer

        juce::Path p;
        float pointerLength = radius * 0.63f;
        float pointerThickness = radius * 0.2f;
        p.addRectangle(-pointerThickness * 0.5f, -radius - 1, pointerThickness, pointerLength);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
        g.setColour(juce::Colour(0xff39587a));
        g.fillPath(p);

        juce::Path p2;
        pointerThickness = radius * 0.05f;
        p2.addRectangle(-pointerThickness * 0.5f, -radius - 1, pointerThickness, pointerLength);
        p2.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
        g.setColour(juce::Colour(0xff070b0f));
        g.fillPath(p2);

        // Central Circle

        float dotradius = radius * (float)0.4;
        float dotradius2 = rw * (float)0.4;
        g.setColour(juce::Colour(0xff39587a));
        g.fillEllipse(centreX - (dotradius),
            centreY - (dotradius),
            dotradius2, dotradius2);

        dotradius = radius * (float)0.3;
        dotradius2 = rw * (float)0.3;
        g.setColour(juce::Colour(0xff41658b));
        g.fillEllipse(centreX - (dotradius),
            centreY - (dotradius),
            dotradius2, dotradius2);

        dotradius = radius * (float)0.2;
        dotradius2 = rw * (float)0.2;
        g.setColour(juce::Colour(0xff527eae));
        g.fillEllipse(centreX - (dotradius),
            centreY - (dotradius),
            dotradius2, dotradius2);
    }

};

class GaloisLookAndFeelCentred : public GaloisLookAndFeel
{
public:
    GaloisLookAndFeelCentred()
    {
    }

    // Pasted from https://github.com/remberg/juceCustomSliderSample
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
        float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override {
        const float radius = juce::jmin(width / 2, height / 2) * 0.85f;
        const float centreX = x + width * 0.5f;
        const float centreY = y + height * 0.5f;
        const float rx = centreX - radius;
        const float ry = centreY - radius;
        const float rw = radius * 2.0f;
        float fwidth = (float)width;
        float fheight = (float)height;
        const float angle = rotaryStartAngle
            + sliderPos
            * (rotaryEndAngle - rotaryStartAngle);

        const float half_angle = rotaryStartAngle
            + 0.5f
            * (rotaryEndAngle - rotaryStartAngle);

        g.setColour(juce::Colour(0xff213246));
        juce::Path filledArc;
        filledArc.addPieSegment(rx, ry, rw + 1, rw + 1, rotaryStartAngle, rotaryEndAngle, 0.6);

        g.fillPath(filledArc);

        juce::Path filledArc1;
        if(sliderPos > 0.5){
            g.setColour(juce::Colours::lightgreen);
            filledArc1.addPieSegment(rx, ry, rw + 1, rw + 1, half_angle, angle, 0.6);
        }
        else if (sliderPos < 0.5) {
            g.setColour(juce::Colours::lightcoral);
            filledArc1.addPieSegment(rx, ry, rw + 1, rw + 1, angle, half_angle, 0.6);
        }

        g.fillPath(filledArc1);

        juce::Path p;
        float pointerLength = radius * 0.63f;
        float pointerThickness = radius * 0.2f;
        p.addRectangle(-pointerThickness * 0.5f, -radius - 1, pointerThickness, pointerLength);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
        g.setColour(juce::Colour(0xff39587a));
        g.fillPath(p);

        juce::Path p2;
        pointerThickness = radius * 0.05f;
        p2.addRectangle(-pointerThickness * 0.5f, -radius - 1, pointerThickness, pointerLength);
        p2.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
        g.setColour(juce::Colour(0xff070b0f));
        g.fillPath(p2);

        float dotradius = radius * (float)0.4;
        float dotradius2 = rw * (float)0.4;
        g.setColour(juce::Colour(0xff39587a));
        g.fillEllipse(centreX - (dotradius),
            centreY - (dotradius),
            dotradius2, dotradius2);

        dotradius = radius * (float)0.3;
        dotradius2 = rw * (float)0.3;
        g.setColour(juce::Colour(0xff41658b));
        g.fillEllipse(centreX - (dotradius),
            centreY - (dotradius),
            dotradius2, dotradius2);

        dotradius = radius * (float)0.2;
        dotradius2 = rw * (float)0.2;
        g.setColour(juce::Colour(0xff527eae));
        g.fillEllipse(centreX - (dotradius),
            centreY - (dotradius),
            dotradius2, dotradius2);
    }

};