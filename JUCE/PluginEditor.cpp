#pragma once

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Proto_galoisAudioProcessorEditor::Proto_galoisAudioProcessorEditor (Proto_galoisAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), mainComponent(&p, vts), valueTreeState(vts)
{
    setSize (980, 500);
    addAndMakeVisible(mainComponent);
}

Proto_galoisAudioProcessorEditor::~Proto_galoisAudioProcessorEditor()
{
}

//==============================================================================
void Proto_galoisAudioProcessorEditor::paint (juce::Graphics& g)
{
   // mainComponent.paint(g);
}

void Proto_galoisAudioProcessorEditor::resized()
{
    mainComponent.setSize(getWidth(), getHeight());
    mainComponent.setTopLeftPosition(0, 0);
    
}
