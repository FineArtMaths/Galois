/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class Proto_galoisAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    Proto_galoisAudioProcessor();
    ~Proto_galoisAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Proto_galoisAudioProcessor)
    juce::AudioParameterInt* bit_depth;
    juce::AudioParameterInt* sample_rate;
    juce::AudioParameterFloat* output_level;
    juce::AudioParameterFloat* input_level;
    double host_sample_rate;
    juce::AudioParameterFloat* reject_dry_amt;

    juce::AudioBuffer<float> working_buffer;
    int working_buffer_idx;
    float sample_reduction_register;
    int sample_reduction_counter;
    float prev_sample_zero_x_check;

    juce::AudioParameterInt* wf_time_scale;
    juce::AudioParameterInt* wf_wave;

    juce::AudioParameterInt* bool_op_and;
    juce::AudioParameterInt* bool_op_or;
    juce::AudioParameterInt* bool_op_xor;
    juce::AudioParameterInt* bool_op_lshift;
    juce::AudioParameterInt* bool_op_rshift;

    const int MAX_BIT_DEPTH = 1024;
};
