/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "MainComponent.cpp"

//==============================================================================
/**
*/
class Proto_galoisAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    Proto_galoisAudioProcessorEditor (Proto_galoisAudioProcessor&, juce::AudioProcessorValueTreeState& vts);
    ~Proto_galoisAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Proto_galoisAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& valueTreeState;
    MainComponent mainComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Proto_galoisAudioProcessorEditor)
};
