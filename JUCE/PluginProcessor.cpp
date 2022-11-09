/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Waveform.cpp"
#include <cmath>

//==============================================================================
Proto_galoisAudioProcessor::Proto_galoisAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                       )
#endif
{
    addParameter(bit_depth = new juce::AudioParameterInt("bit_depth", "Bit Depth", 2, MAX_BIT_DEPTH, MAX_BIT_DEPTH));
    addParameter(sample_rate = new juce::AudioParameterInt("sample_rate", "Sample Rate Divider", 1, 256, 1));
    addParameter(output_level = new juce::AudioParameterFloat("output_level", "Output Level", 0, 5, 1));
    addParameter(input_level = new juce::AudioParameterFloat("input_level", "Input Level", 0, 5, 1));
    addParameter(wf_time_scale = new juce::AudioParameterInt("wf_time_scale", "Wave Scale", 1, 20, 1));
    addParameter(wf_wave = new juce::AudioParameterInt("wf_wave", "Wave Shape", 0, NUM_WFs - 1,    0));
    addParameter(reject_dry_amt = new juce::AudioParameterFloat("reject_dry_amt", "Reject Dry Amt", -2, 2, 0));
    addParameter(bool_op_and = new juce::AudioParameterInt("bool_op_and", "AND", 0, MAX_BIT_DEPTH, 0));
    addParameter(bool_op_or = new juce::AudioParameterInt("bool_op_or", "OR", 0, MAX_BIT_DEPTH, 0));
    addParameter(bool_op_xor = new juce::AudioParameterInt("bool_op_xor", "XOR", 0, MAX_BIT_DEPTH, 0));
    addParameter(bool_op_lshift = new juce::AudioParameterInt("bool_op_lshift", "LSHIFT", 0, floor(log(MAX_BIT_DEPTH)/log(2)) * 5, 0));
    addParameter(bool_op_rshift = new juce::AudioParameterInt("bool_op_rshift", "RSHIFT", 0, floor(log(MAX_BIT_DEPTH) / log(2)), 0));
}

Proto_galoisAudioProcessor::~Proto_galoisAudioProcessor()
{
}

//==============================================================================
const juce::String Proto_galoisAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Proto_galoisAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Proto_galoisAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Proto_galoisAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Proto_galoisAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Proto_galoisAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Proto_galoisAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Proto_galoisAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Proto_galoisAudioProcessor::getProgramName (int index)
{
    return {};
}

void Proto_galoisAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Proto_galoisAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    working_buffer = juce::AudioBuffer<float>(2, sampleRate);
    working_buffer.clear();
    working_buffer_idx = 0;
    sample_reduction_register = 0;
    sample_reduction_counter = 0;
    prev_sample_zero_x_check = 0;
}

void Proto_galoisAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Proto_galoisAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void Proto_galoisAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    bool use_booleans = (bool_op_and->get() + bool_op_or->get() + bool_op_xor->get() + bool_op_lshift->get() + bool_op_rshift->get()) > 0;
    for (auto i = 0; i < buffer.getNumChannels(); ++i){   
        float* channel = buffer.getWritePointer(i);
        for (auto j = 0; j < buffer.getNumSamples(); ++j){
            float sample = channel[j];
            // Sample reduction
            sample_reduction_counter++;
            if (sample_reduction_counter >= sample_rate->get()) {
                sample_reduction_counter = 0;
                sample_reduction_register = sample;
            }
            else {
                sample = sample_reduction_register;
            }
            // Input level
            if (input_level->get() != 1) {
                sample *= input_level->get();
            }
            // Bit reduction
            if (bit_depth->get() < MAX_BIT_DEPTH) {
                sample = floor(sample * bit_depth->get()) / bit_depth->get();
            }
            // Boolean operations
            if (use_booleans) {
                int i_sample = floor(sample * MAX_BIT_DEPTH);
                if (bool_op_and->get() > 0) {
                    i_sample = i_sample & bool_op_and->get();
                }
                if (bool_op_or->get() > 0) {
                    i_sample = i_sample | bool_op_or->get();
                }
                if (bool_op_xor->get() > 0) {
                    i_sample = i_sample ^ bool_op_xor->get();
                }
                if (bool_op_lshift->get() > 0) {
                    i_sample = i_sample << bool_op_lshift->get();
                }
                if (bool_op_rshift->get() > 0) {
                    i_sample = i_sample >> bool_op_rshift->get();
                }
                sample = (float)i_sample / (float)MAX_BIT_DEPTH;
                while (abs(sample) >= 1) {
                    sample *= 0.6;
                }
            }
            // Waveform remapping
            sample = map_to_wf(sample, wf_time_scale->get(), wf_wave->get());
            // Reject dry
            if (reject_dry_amt->get() > 0) {
                sample += -1 * channel[j] * reject_dry_amt->get();
            }
            // Zero crossing check
            if (sgn(sample) != 0 && sgn(prev_sample_zero_x_check) != 0 && sgn(sample) != sgn(prev_sample_zero_x_check)) {
                prev_sample_zero_x_check = sample;
                sample = 0;
            }
            // Output Level and clipping
            sample *= output_level->get();
            sample = clamp(sample, -1, 1);
            channel[j] = sample;
        }
        working_buffer_idx++;
        working_buffer_idx %= buffer.getNumSamples();
    }
}

//==============================================================================
bool Proto_galoisAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Proto_galoisAudioProcessor::createEditor()
{
    return new Proto_galoisAudioProcessorEditor (*this);
}

//==============================================================================
void Proto_galoisAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Proto_galoisAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Proto_galoisAudioProcessor();
}
