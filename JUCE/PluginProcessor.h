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

private:
    //==============================================================================
    float filter(const float input, float* wb_channel, int wb_channel_number, float filter_window);
    float apply_filter(float sample, float* wb_channel, int wbi);


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Proto_galoisAudioProcessor)
    double host_sample_rate;

    float sample_reduction_register;
    int sample_reduction_counter;

    char** blend_mode_names;

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
    int cached_dry_blend_mode;
    int cached_bit_mask;

    // Factory Presets
    juce::String* preset_names;
    char** preset_filenames;
    int current_programme;
    const int NUM_PROGRAMMES = 2;
};
