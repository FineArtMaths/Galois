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

        filterPositionLabel.setButtonText(ap->getFilterPosition());
        filterPositionLabel.onClick = [this] { filterPositionLabelClicked(); };
        filterPositionLabel.setSize(50, 20);
        filterPositionLabel.setColour(juce::Label::textColourId, juce::Colours::lightcoral);
        filterPositionLabel.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
        addAndMakeVisible(filterPositionLabel);

        filterTypeLabel.setButtonText(ap->getFilterType());
        filterTypeLabel.onClick = [this] { filterTypeLabelClicked(); };
        filterTypeLabel.setSize(50, 20);
        filterTypeLabel.setColour(juce::Label::textColourId, juce::Colours::lightcoral);
        filterTypeLabel.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
        addAndMakeVisible(filterTypeLabel);

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

        makeSlider(waveSlider, waveSliderLabel, "wf_base_wave", waveSliderAttachment, false, false, false);
        makeSlider(algoSlider, algoSliderLabel, "algorithm", algoSliderAttachment, false, false, false);
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

        makeSlider(filterBlendSlider, filterBlendSliderLabel, "filter_blend", filterBlendSliderAttachment, false, false);
        makeSlider(filterCutoffSlider, filterCutoffSliderLabel, "biquad_cutoff", filterCutoffSliderAttachment, false, false);
        makeSlider(filterQSlider, filterQSliderLabel, "biquad_q", filterQSliderAttachment, false, false);
    }

    void makeSlider(
        juce::Slider& slider, 
        juce::Label& label, 
        const char* param, 
        std::unique_ptr<SliderAttachment>& sa, 
        bool centred=false, bool text_box=true, bool show_label=true
    ) {
        addAndMakeVisible(slider);
        if (centred) {
            slider.setLookAndFeel(&lookAndFeelCentred);
        }
        else {
            slider.setLookAndFeel(&lookAndFeel);
        }
        slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
        if (text_box) {
            slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, slider.getTextBoxHeight());
        }
        else {
            slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
        }
        slider.setWantsKeyboardFocus(true);

        SliderAttachment* saptr = new SliderAttachment(valueTreeState, param, slider);
        sa.reset(saptr);
        if (show_label) {
            addAndMakeVisible(label);

            juce::String text = valueTreeState.getParameter(param)->name;
            label.setText(text, juce::NotificationType::dontSendNotification);
            label.setFont(juce::Font(18.0f, juce::Font::plain));
            label.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
            label.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
            label.setColour(juce::Label::outlineColourId, juce::Colours::transparentBlack);
            label.setColour(juce::Label::outlineWhenEditingColourId, juce::Colours::transparentBlack);
            label.setJustificationType(juce::Justification::centred);
            label.attachToComponent(&slider, false);
        }
    }

    ~MainComponent()
    {
        // ...
    }

    void paint(juce::Graphics& g) {
       g.fillAll(juce::Colours::slategrey);
       g.setColour(juce::Colours::black);
       g.setFillType(juce::FillType(juce::Colours::black));
       int line_spacer = knob_size + knob_spacer * 5;
    }

    void resized() override
    {
        wf_component.updateWaveform();
        backgroundImage.setSize(getWidth(), getHeight());
        backgroundImage.setTopLeftPosition(0, 0);

        wf_component.setSize(getHeight() - 20, getHeight() - 20);
        wf_component.setTopLeftPosition(10, 10);
                
        placeSlider(waveSlider, waveSliderLabel, 500 - knob_size, 460 - knob_size, 0.7);
        placeSlider(algoSlider, algoSliderLabel, 530 - knob_size, 420 - knob_size, 0.4);

        int line_spacer = knob_size + knob_spacer * 7;
        int xpos = wf_component.getWidth() + 50 + knob_spacer;
        placeSlider(inputSlider, inputSliderLabel, xpos, 30);
        xpos += knob_size + knob_spacer;
        placeSlider(dryBlendSlider, dryBlendSliderLabel, xpos, 30);
        xpos += knob_size + knob_spacer;
        placeSlider(outputSlider, outputSliderLabel, xpos, 30);
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

        xpos = wf_component.getWidth() + 50 + knob_spacer;
        placeSlider(foldSlider, foldSliderLabel, xpos, 30 + line_spacer);
        xpos += knob_size + knob_spacer;
        placeSlider(powerSlider, powerSliderLabel, xpos, 30 + line_spacer);
        xpos += knob_size + knob_spacer;
        placeSlider(harmFreqSlider, harmFreqSliderLabel, xpos, 30 + line_spacer);
        xpos += knob_size + knob_spacer;
        placeSlider(harmAmtSlider, harmAmtSliderLabel, xpos, 30 + line_spacer);

        xpos = wf_component.getWidth() + 50 + knob_spacer;
        placeSlider(bitDepthSlider, bitDepthSliderLabel, xpos, 30 + 2 * line_spacer);
        xpos += knob_size + knob_spacer;
        placeSlider(bitMaskSlider, bitMaskSliderLabel, xpos, 30 + 2 * line_spacer);
        xpos += knob_size + knob_spacer;
        placeSlider(sampleRateSlider, sampleRateSliderLabel, xpos, 30 + 2 * line_spacer);
        xpos += knob_size + knob_spacer;
        placeSlider(filterCutoffSlider, filterCutoffSliderLabel, xpos, 30 + 2 * line_spacer + 10, 0.4f);
        placeSlider(filterQSlider, filterQSliderLabel, xpos + (knob_size + knob_spacer) * 0.4f, 30 + 2 * line_spacer + 10, 0.4f);
        placeSlider(filterBlendSlider, filterBlendSliderLabel, xpos, 30 + 2 * line_spacer + knob_size * 0.6f + 10, 0.4f);

        int x = xpos + (knob_size + knob_spacer) * 0.4f;
        int y = 30 + 2 * line_spacer + knob_size * 0.6f + 10;
        filterPositionLabel.setTopLeftPosition(x, y);
        filterPositionLabel.toFront(false);
        filterTypeLabel.setTopLeftPosition(x, y + 20);
        filterTypeLabel.toFront(false);
    }

    void placeSlider(juce::Slider& slider, juce::Label& label, int x, int y, float scale=1.0f) {
        slider.setSize(knob_size * scale, knob_size * scale);
        slider.setTopLeftPosition(x, y);
        slider.onValueChange = [this] { waveformChanged(); };
    }

    void presetButtonClicked() {
        audioProcessor->saveFactoryPreset(factoryPresetNameLabel.getText());
    }

    void filterPositionLabelClicked() {
        audioProcessor->cycleParamValue("filter_pre");
        filterPositionLabel.setButtonText(audioProcessor->getFilterPosition());
    }

    void filterTypeLabelClicked() {
        audioProcessor->cycleParamValue("biquad_type");
        filterTypeLabel.setButtonText(audioProcessor->getFilterType());
    }

    void waveformChanged() {
        wf_component.updateWaveform();
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

    juce::Slider algoSlider;
    juce::Label algoSliderLabel;
    std::unique_ptr<SliderAttachment> algoSliderAttachment;

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

    juce::Slider filterBlendSlider;
    juce::Label filterBlendSliderLabel;
    std::unique_ptr<SliderAttachment> filterBlendSliderAttachment;

    juce::Slider filterCutoffSlider;
    juce::Label filterCutoffSliderLabel;
    std::unique_ptr<SliderAttachment> filterCutoffSliderAttachment;

    juce::Slider filterQSlider;
    juce::Label filterQSliderLabel;
    std::unique_ptr<SliderAttachment> filterQSliderAttachment;

    int knob_size = 110;
    int knob_spacer = 7;

    juce::ImageComponent backgroundImage;
    juce::ImageComponent logoImage;
    juce::TextButton factoryPresetButton;
    juce::Label factoryPresetNameLabel;

    juce::TextButton filterPositionLabel;
    juce::TextButton filterTypeLabel;
};