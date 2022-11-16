#pragma once

#include <JuceHeader.h>
#include <BinaryData.h>
#include "PluginProcessor.h"
#include "WaveformComponent.cpp"
#include "GaloisLookAndFeel.cpp"
#include <cstdlib>

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

//#define FACTORY_PRESET_BUTTON

class MainComponent : public juce::Component
{
public:
    //==============================================================================
    MainComponent (
        Proto_galoisAudioProcessor* ap, juce::AudioProcessorValueTreeState& vts
    ) : valueTreeState(vts), wf_component(ap)
    {
#ifdef FACTORY_PRESET_BUTTON

        factoryPresetButton.setButtonText("SAVE");
        addAndMakeVisible(factoryPresetButton);
        factoryPresetButton.onClick = [this] { presetButtonClicked(); };
        factoryPresetNameLabel.setEditable(true);
        addAndMakeVisible(factoryPresetNameLabel);

#endif // FACTORY_PRESET_BUTTON

        // Load images
        juce::PNGImageFormat format;
        juce::MemoryInputStream* mis = new juce::MemoryInputStream(BinaryData::logo_png, BinaryData::logo_pngSize, false);
        juce::Image img = format.decodeImage(*mis);
        delete(mis);
        logoImage.setImage(img);
        addAndMakeVisible(logoImage);

        mis = new juce::MemoryInputStream(BinaryData::background_png, BinaryData::background_pngSize, false);
        img = format.decodeImage(*mis);
        delete(mis);
        backgroundImage.setImage(img);
        addAndMakeVisible(backgroundImage);
        

        audioProcessor = ap;
        addAndMakeVisible(wf_component);

        makeSlider(waveSlider, waveSliderLabel, "wf_base_wave", waveSliderAttachment);
        waveSliderLabel.setTooltip("WOOOOT");
        makeSlider(bitDepthSlider, bitDepthSliderLabel, "bit_depth", bitDepthSliderAttachment);
        makeSlider(powerSlider, powerSliderLabel, "wf_power", powerSliderAttachment, true);
        makeSlider(harmFreqSlider, harmFreqSliderLabel, "wf_harm_freq", harmFreqSliderAttachment);
        makeSlider(harmAmtSlider, harmAmtSliderLabel, "wf_harm_amp", harmAmtSliderAttachment, true);
        makeSlider(foldSlider, foldSliderLabel, "wf_fold", foldSliderAttachment, true);
        makeSlider(sampleRateSlider, sampleRateSliderLabel, "sample_rate", sampleRateSliderAttachment);
        makeSlider(inputSlider, inputSliderLabel, "input_level", inputSliderAttachment);
        makeSlider(outputSlider, outputSliderLabel, "output_level", outputSliderAttachment);
        makeSlider(dryBlendSlider, dryBlendSliderLabel, "dry_blend", dryBlendSliderAttachment, true);
        makeSlider(bitMaskSlider, bitMaskSliderLabel, "bit_mask", bitMaskSliderAttachment, true);
    }

    void makeSlider(juce::Slider& slider, juce::Label& label, char* param, std::unique_ptr<SliderAttachment>& sa, bool centred=false) {
        addAndMakeVisible(slider);
        if (centred) {
            slider.setLookAndFeel(&lookAndFeelCentred);
        }
        else {
            slider.setLookAndFeel(&lookAndFeel);
        }
        slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, slider.getTextBoxHeight());

        SliderAttachment* saptr = new SliderAttachment(valueTreeState, param, slider);
        sa.reset(saptr);

        addAndMakeVisible(label);

        juce::String text = valueTreeState.getParameter(param)->name;
        label.setText(text, juce::NotificationType::dontSendNotification);
        label.setFont(juce::Font(14.0f, juce::Font::plain));
        label.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
        label.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
        label.setColour(juce::Label::outlineColourId, juce::Colours::transparentBlack);
        label.setColour(juce::Label::outlineWhenEditingColourId, juce::Colours::transparentBlack);
        label.setJustificationType(juce::Justification::centred);
        label.attachToComponent(&slider, false);
    }

    ~MainComponent()
    {
        // ...
    }

    void paint(juce::Graphics& g) {
       wf_component.updateWaveform();
       g.setColour(juce::Colours::black);
       g.setFillType(juce::FillType(juce::Colours::black));
       int line_spacer = knob_size + knob_spacer * 5;
       g.drawRect(
           wf_component.getWidth() + 20,
           2*line_spacer,
           getWidth() - wf_component.getWidth() - 30,
           2
       );
       char s[10];
       itoa(audioProcessor->getCurrentProgram(), s, 10);
    }

    void resized() override
    {
        backgroundImage.setSize(getWidth(), getHeight());
        backgroundImage.setTopLeftPosition(0, 0);
        wf_component.setSize(getHeight() - 20, getHeight() - 20);
        wf_component.setTopLeftPosition(10, 10);

        int line_spacer = knob_size + knob_spacer * 5;
        int xpos = wf_component.getWidth() + knob_spacer;
        placeSlider(waveSlider, waveSliderLabel, xpos, 30);
        xpos += knob_size + knob_spacer;
        placeSlider(foldSlider, foldSliderLabel, xpos, 30);
        xpos += knob_size + knob_spacer;
        placeSlider(powerSlider, powerSliderLabel, xpos, 30);
        xpos += knob_size + knob_spacer;
        logoImage.setSize(knob_size*0.9, knob_size * 0.9);
        logoImage.setTopLeftPosition(xpos, 20);
        logoImage.toFront(false);
#ifdef FACTORY_PRESET_BUTTON
        factoryPresetButton.setSize(knob_size, knob_size/4);
        factoryPresetButton.setTopLeftPosition(xpos , 20);
        factoryPresetButton.toFront(false);
        factoryPresetNameLabel.setSize(knob_size, knob_size/4);
        factoryPresetNameLabel.setTopLeftPosition(xpos, 40);
        factoryPresetNameLabel.toFront(false);
#endif

        xpos = wf_component.getWidth() + knob_spacer;
        placeSlider(harmFreqSlider, harmFreqSliderLabel, xpos, 30 + line_spacer);
        xpos += knob_size + knob_spacer;
        placeSlider(harmAmtSlider, harmAmtSliderLabel, xpos, 30 + line_spacer);
        xpos += knob_size + knob_spacer;
        placeSlider(bitDepthSlider, bitDepthSliderLabel, xpos, 30 + line_spacer);
        xpos += knob_size + knob_spacer;
        placeSlider(bitMaskSlider, bitMaskSliderLabel, xpos, 30 + line_spacer);

        xpos = wf_component.getWidth() + knob_spacer;
        placeSlider(inputSlider, inputSliderLabel, xpos, 30 + 2 * line_spacer + 20);
        xpos += knob_size + knob_spacer;
        placeSlider(sampleRateSlider, sampleRateSliderLabel, xpos, 30 + 2 * line_spacer + 20);
        xpos += knob_size + knob_spacer;
        placeSlider(dryBlendSlider, dryBlendSliderLabel, xpos, 30 + 2 * line_spacer + 20);
        xpos += knob_size + knob_spacer;
        placeSlider(outputSlider, outputSliderLabel, xpos, 30 + 2 * line_spacer + 20);
    }

    void placeSlider(juce::Slider& slider, juce::Label& label, int x, int y) {
        slider.setSize(knob_size, knob_size);
        slider.setTopLeftPosition(x, y);
    }

    void presetButtonClicked() {
        audioProcessor->saveFactoryPreset(factoryPresetNameLabel.getText());
    }

private:
    Proto_galoisAudioProcessor* audioProcessor;
    juce::AudioProcessorValueTreeState& valueTreeState;
    GaloisLookAndFeel lookAndFeel;
    GaloisLookAndFeelCentred lookAndFeelCentred;


    // Waveform display component
    WaveformComponent wf_component;

    // Controls
    juce::Slider waveSlider;
    juce::Label waveSliderLabel;
    std::unique_ptr<SliderAttachment> waveSliderAttachment;

    juce::Slider bitDepthSlider;
    juce::Label bitDepthSliderLabel;
    std::unique_ptr<SliderAttachment> bitDepthSliderAttachment;

    juce::Slider powerSlider;
    juce::Label powerSliderLabel;
    std::unique_ptr<SliderAttachment> powerSliderAttachment;

    juce::Slider harmFreqSlider;
    juce::Label harmFreqSliderLabel;
    std::unique_ptr<SliderAttachment> harmFreqSliderAttachment;

    juce::Slider harmAmtSlider;
    juce::Label harmAmtSliderLabel;
    std::unique_ptr<SliderAttachment> harmAmtSliderAttachment;

    juce::Slider foldSlider;
    juce::Label foldSliderLabel;
    std::unique_ptr<SliderAttachment> foldSliderAttachment;

    juce::Slider sampleRateSlider;
    juce::Label sampleRateSliderLabel;
    std::unique_ptr<SliderAttachment> sampleRateSliderAttachment;

    juce::Slider outputSlider;
    juce::Label outputSliderLabel;
    std::unique_ptr<SliderAttachment> outputSliderAttachment;

    juce::Slider inputSlider;
    juce::Label inputSliderLabel;
    std::unique_ptr<SliderAttachment> inputSliderAttachment;

    juce::Slider dryBlendSlider;
    juce::Label dryBlendSliderLabel;
    std::unique_ptr<SliderAttachment> dryBlendSliderAttachment;

    juce::Slider dryBlendModeSlider;
    juce::Label dryBlendModeSliderLabel;
    std::unique_ptr<SliderAttachment> dryBlendModeSliderAttachment;

    juce::Slider bitMaskSlider;
    juce::Label bitMaskSliderLabel;
    std::unique_ptr<SliderAttachment> bitMaskSliderAttachment;

    int knob_size = 110;
    int knob_spacer = 10;

    juce::ImageComponent backgroundImage;
    juce::ImageComponent logoImage;
    juce::TextButton factoryPresetButton;
    juce::Label factoryPresetNameLabel;
};