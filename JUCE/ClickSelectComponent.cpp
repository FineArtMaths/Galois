#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class ClickSelectComponent : public juce::Component
{

public:

    ClickSelectComponent(Proto_galoisAudioProcessor* ap) {
        proc = ap;
        setSize(40, 20);
        vDarkGreen = juce::Colour(0, 50, 0);

    }

private:
    juce::Colour vDarkGreen;
    const char* wf_name;
    juce::Label wfNameLabel;
    Proto_galoisAudioProcessor* proc;

};