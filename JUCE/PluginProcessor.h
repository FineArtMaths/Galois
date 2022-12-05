/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Biquad.cpp"

//==============================================================================
/**
*/
class Proto_galoisAudioProcessor  : public juce::AudioProcessor
                            , public juce::AudioProcessorValueTreeState::Listener
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

    float getWaveformValue(float sample);

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

    //==============================================================================
    // Parameters
    juce::AudioProcessorValueTreeState tree;
    void parameterChanged(const juce::String& parameterID, float newValue);
    void cacheWaveforms();
    const char* getWaveformName();

    float* waveform_cache;
    const int waveform_resolution = 200;

    void saveFactoryPreset(juce::String name);
    juce::String getFilterPosition();
    juce::String getFilterType();
    void cycleParamValue(juce::String parameterID);

private:

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Proto_galoisAudioProcessor)
    double host_sample_rate;
    int num_channels;

    float* sample_reduction_register;
    int sample_reduction_counter;

    // Filter 
    Biquad* biquad_filter;
    float cached_biquad_cutoff;
    float cached_biquad_q;
    float cached_biquad_gain;
    int cached_biquad_type;
    int cached_filter_pre;
    float cached_filter_blend;
    void updateFilter();
    float apply_filter(float sample, int channel);

    juce::String* biquad_position_names;
    juce::String* biquad_type_names;

    // Cached parameter values
    float cached_bit_depth;
    int cached_sample_rate;
    float cached_output_level;
    float cached_input_level;
    int cached_wf_base_wave;
    float cached_wf_power;
    float cached_wf_fold;
    float cached_wf_harm_freq;
    float cached_wf_harm_amp;
    float cached_dry_blend;
    float cached_dry_blend_abs;
    float cached_dry_blend_sign;
    int cached_dry_blend_mode;
    int cached_bit_mask;
    float cached_low_cutoff;

    // Factory Presets
    juce::String* preset_names;
    juce::String* preset_filenames;
    int current_programme;
    const int NUM_PROGRAMMES = 2;
};
