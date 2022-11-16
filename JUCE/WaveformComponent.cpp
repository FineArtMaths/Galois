#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class WaveformComponent : public juce::Component
{

public:

    WaveformComponent(Proto_galoisAudioProcessor* ap) {
        // Load images
        juce::PNGImageFormat format;
        juce::MemoryInputStream* mis = new juce::MemoryInputStream(BinaryData::graph_background_png, BinaryData::graph_background_pngSize, false);
        juce::Image img = format.decodeImage(*mis);
        delete(mis);
        wfbgImage.setImage(img);
        addAndMakeVisible(wfbgImage);

        proc = ap;
        setSize(400, 400);
        vDarkGreen = juce::Colour(0, 50, 0);
        
        wf_name = "Identity";
        addAndMakeVisible(wfNameLabel);

        wfNameLabel.setText(wf_name, juce::NotificationType::dontSendNotification);
        wfNameLabel.setFont(juce::Font(24.0f, juce::Font::plain));
        wfNameLabel.setColour(juce::Label::textColourId, juce::Colours::lightcoral);
        wfNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
        wfNameLabel.setJustificationType(juce::Justification::right);
    }

    void updateWaveform() {
//        for (int i = 0; i < resolution; i++) {
//            float amp = (float)(i - half_resolution) / half_resolution;
//            waveform[i] = proc->getWaveformValue(amp);
//        }
        wf_name = proc->getWaveformName();
        repaint();
    }

    void paint(juce::Graphics& g) override {

        g.fillAll(vDarkGreen);
        wfbgImage.setSize(getWidth(), getHeight());
        wfbgImage.setTopLeftPosition(0, 0);

        // TODO: Why on Earth do we need the 0.995 factor??
        float xscale = getWidth() / (proc->waveform_resolution * 0.995);
        float yscale = getHeight() / 2;

        g.setColour(juce::Colours::darkgreen);
        g.setFillType(juce::FillType(juce::Colours::darkgreen));
        g.drawRect(0, (int)yscale, getWidth(), (int)xscale, (int)xscale);
        g.drawRect((int)getWidth() / 2, 0, (int)xscale, getHeight(), (int)xscale);
        // Ticks
        for (int i = 0; i < getHeight(); i += 10) {
            int wid = i % 50 == 0 ? 20 : 10;
            g.drawRect((getWidth() / 2) - wid/2, i, wid, (int)xscale, (int)xscale);
        }
        for (int i = 0; i < getWidth(); i += 10) {
            int wid = i % 50 == 0 ? 20 : 10;
            g.drawRect(i, (getHeight() / 2) - wid / 2, (int)xscale, wid, (int)xscale);
        }
        // Curve
        g.setColour(juce::Colours::lightgreen);
        g.setOpacity(1.0f);
        for (int x = 1; x < proc->waveform_resolution; x++) {
            g.drawLine(
                (x-1) * xscale,
                yscale - proc->waveform_cache[x-1] * yscale,
                (x) * xscale,
                yscale - proc->waveform_cache[x] * yscale,
                xscale
            );
        }

        // Label
        wfNameLabel.setText(wf_name, juce::NotificationType::dontSendNotification);
        wfNameLabel.setSize(getWidth()/2, 40);
        wfNameLabel.setTopLeftPosition(
            getWidth() - wfNameLabel.getWidth() - 10, 
            getHeight() - wfNameLabel.getHeight() - 10
        );
    }

    void resized() override
    {
        // This is called when the MainContentComponent is resized.
        // If you add any child components, this is where you should
        // update their positions.
    }

private:
    //int resolution;
    //float half_resolution;
    //float* waveform;
    juce::Colour vDarkGreen;
    const char* wf_name;

    juce::Label wfNameLabel;
    juce::ImageComponent wfbgImage;
    Proto_galoisAudioProcessor* proc;
};

