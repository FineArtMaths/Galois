/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Waveform.cpp"
#include <cmath>

//==============================================================================
Proto_galoisAudioProcessor::Proto_galoisAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
    )
#endif
    , tree(*this, nullptr, "Galois_Parameter_Tree",
        {
            std::make_unique<juce::AudioParameterFloat>("bit_depth", "Bit Crush", juce::NormalisableRange<float>(2, MAX_BIT_DEPTH), 2),
            std::make_unique<juce::AudioParameterInt>("sample_rate", "S & H", 1, 256, 1),
            std::make_unique<juce::AudioParameterFloat>("output_level", "Output Level", juce::NormalisableRange<float>(0, 4), 1),
            std::make_unique<juce::AudioParameterFloat>("input_level", "Input Level", juce::NormalisableRange<float>(0, 4), 1),
            std::make_unique<juce::AudioParameterInt>("wf_base_wave", "Waveform", 0, NUM_WFs - 1, 0),
            std::make_unique<juce::AudioParameterFloat>("wf_power", "Power", juce::NormalisableRange<float>(-1, 1), 0),
            std::make_unique<juce::AudioParameterFloat>("wf_fold", "Fold", juce::NormalisableRange<float>(-9, 9), 0),
            std::make_unique<juce::AudioParameterFloat>("wf_harm_freq", "Harm Freq", juce::NormalisableRange<float>(1, 40), 1),
            std::make_unique<juce::AudioParameterFloat>("wf_harm_amp", "Harm Amount", juce::NormalisableRange<float>(-10, 10), 0),
            std::make_unique<juce::AudioParameterFloat>("dry_blend", "Dry Blend", juce::NormalisableRange<float>(-1, 1), 0),
            std::make_unique<juce::AudioParameterInt>("dry_blend_mode", "Blend Mode", 0, 4, 0),
            std::make_unique<juce::AudioParameterInt>("bit_mask", "Bit Mask", -1023, 1023, 0),
            std::make_unique<juce::AudioParameterFloat>("filter_blend", "Blend", 0.0f, 1.0f, 0.0f),
            std::make_unique<juce::AudioParameterInt>("filter_pre", "Filter Before Remapping", 0, 1, 1),
            std::make_unique<juce::AudioParameterFloat>("biquad_cutoff", "Cutoff", 0.0f, 9.0f, 9.0f),
            std::make_unique<juce::AudioParameterFloat>("biquad_q", "Q", 0.0f, 1.0f, 0.5f),
            std::make_unique<juce::AudioParameterFloat>("biquad_gain", "Filter Gain", 0.0f, 20.0f, 1.0f)
        }
    )
{

    sample_reduction_register = 0;
    sample_reduction_counter = 0;

    biquad_filter = 0;
    biquad_position_names = new juce::String[2];
    biquad_position_names[0] = "PRE";
    biquad_position_names[1] = "POST";
    biquad_type_names = new juce::String[3];
    biquad_type_names[0] = "LP";
    biquad_type_names[1] = "HP";
    biquad_type_names[2] = "BP";
    cached_biquad_type = 0;

    
    preset_names = new juce::String[NUM_PROGRAMMES];
    preset_names[0] = "Init";
    preset_names[1] = "TestPreset";
    preset_filenames = new juce::String[NUM_PROGRAMMES];
    preset_filenames[0] = "preset_Init_xml";
    preset_filenames[1] = "preset_TestPreset_xml";
    current_programme = 0;

    initialize_waveforms();
    waveform_cache = new float[waveform_resolution];
    cacheWaveforms();

    tree.addParameterListener("bit_depth", this);
    tree.addParameterListener("wf_base_wave", this);
    tree.addParameterListener("wf_power", this);
    tree.addParameterListener("wf_fold", this);
    tree.addParameterListener("wf_harm_freq", this);
    tree.addParameterListener("wf_harm_amp", this);
    tree.addParameterListener("bit_mask", this);
    tree.addParameterListener("sample_rate", this);
    tree.addParameterListener("input_level", this);
    tree.addParameterListener("output_level", this);
    tree.addParameterListener("dry_blend", this);
    tree.addParameterListener("filter_blend", this);
    tree.addParameterListener("biquad_cutoff", this);
    tree.addParameterListener("biquad_q", this);
    tree.addParameterListener("biquad_gain", this);
}


Proto_galoisAudioProcessor::~Proto_galoisAudioProcessor()
{
    delete[] sample_reduction_register;
    delete[] preset_filenames;
    delete[] preset_names;
    delete[] waveform_cache;
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
    return NUM_PROGRAMMES;
}

int Proto_galoisAudioProcessor::getCurrentProgram()
{
    return current_programme;
}

void Proto_galoisAudioProcessor::setCurrentProgram (int index)
{
    if (index < 0 || index >= NUM_PROGRAMMES) {
        return;
    }
    current_programme = index;
    int dataSize = 0;
    const char* xml = BinaryData::getNamedResource(preset_filenames[index].toRawUTF8(), dataSize);
    std::unique_ptr<juce::XmlElement> xmlState = juce::XmlDocument(xml).getDocumentElement();

    if (xmlState.get() != nullptr) {    // FAILING HERE
        if (xmlState->hasTagName(tree.state.getType())) {
            tree.replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }
}

const juce::String Proto_galoisAudioProcessor::getProgramName (int index)
{
    if (index < 0 || index >= NUM_PROGRAMMES) {
        return "ERROR in value of index";
    }
    return preset_names[index];
    return juce::String("");
}

void Proto_galoisAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Proto_galoisAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    host_sample_rate = sampleRate;
    num_channels = getNumInputChannels();
    sample_reduction_register = new float[num_channels];
    biquad_filter = new Biquad[num_channels];
    parameterChanged("", 0);
    updateFilter();
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

float scale_sample(float s) {
    if (abs(s) > 1) {
        s *= 1.0f / (float)ceil(abs(s));
    }
    return s;
}

float Proto_galoisAudioProcessor::getWaveformValue(
    float sample) {
    return remap_sample(
        sample,
        cached_wf_base_wave,
        cached_wf_power,
        cached_wf_harm_freq,
        cached_wf_harm_amp,
        cached_bit_depth,
        cached_wf_fold,
        cached_bit_mask
    );
}

void Proto_galoisAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    for (auto i = 0; i < num_channels; ++i){
        float* channel = buffer.getWritePointer(i);
        for (auto j = 0; j < buffer.getNumSamples(); ++j){
            float sample = channel[j];

            // Sample reduction
            sample_reduction_counter++;
            if (sample_reduction_counter >= cached_sample_rate) {
                sample_reduction_counter = 0;
                sample_reduction_register[i] = sample;
            }
            else {
                sample = sample_reduction_register[i];
            }
            
            // Input level
            sample *= sqrt(cached_input_level);

            // Filter
            if (cached_filter_pre == 0) {
                sample = apply_filter(sample, i);
            }
            // Waveform remapping
            sample = getWaveformValue(sample);

            // Filter
            if (cached_filter_pre == 1) {
                sample = apply_filter(sample, i);
            }

            // Dry Blend
            sample = cached_dry_blend_sign * channel[j] * cached_dry_blend_abs + sample * (1 - cached_dry_blend_abs);
            sample /= 2;

            // Output Level             
            sample *= cached_output_level;
            sample *= 0.7;
            // Clamp to valid range
            sample = clamp(sample, -1, 1);
            
            channel[j] = sample;
     
        }
    }
}

float Proto_galoisAudioProcessor::apply_filter(float sample, int channel) {
    float filtered_sample = biquad_filter[channel].apply(sample);
    sample = filtered_sample * (cached_filter_blend) + sample * (1 - cached_filter_blend);
    sample /= 2;
    return sample;
}

const char* Proto_galoisAudioProcessor::getWaveformName() {
    int i = (int)*tree.getRawParameterValue("wf_base_wave");
    return wf_names[i];

}

//==============================================================================
bool Proto_galoisAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Proto_galoisAudioProcessor::createEditor()
{
    return new Proto_galoisAudioProcessorEditor (*this, tree);
}

//==============================================================================
void Proto_galoisAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = tree.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void Proto_galoisAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr) {
        if (xmlState->hasTagName(tree.state.getType())) {
            tree.replaceState(juce::ValueTree::fromXml(*xmlState));
            current_programme = -1;
        }
    }
}

void Proto_galoisAudioProcessor::saveFactoryPreset(juce::String name) {
    juce::ValueTree t = tree.copyState();
    std::unique_ptr<juce::XmlElement> xml = t.createXml();
    juce::File dir = juce::File::getSpecialLocation(juce::File::SpecialLocationType::userDocumentsDirectory);
    juce::String fname = dir.getFullPathName();
    fname.append("\\preset_", 9);
    fname.append(name, 99);
    fname.append(".xml", 4);
    juce::File f = juce::File(fname);
    xml->writeTo(f);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Proto_galoisAudioProcessor();
}

//==============================================================================
// Cache the waveform here
void Proto_galoisAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue) {
    cacheWaveforms();
    
//    if (parameterID == "biquad_cutoff") {
        cached_biquad_cutoff = 16 * pow(2, *tree.getRawParameterValue("biquad_cutoff"));
        if (cached_biquad_cutoff >= host_sample_rate / 2) {
            cached_biquad_cutoff = host_sample_rate / 2 - 1;
        }
//    } else if (parameterID == "biquad_q") {
        cached_biquad_q = 1.01 - *tree.getRawParameterValue("biquad_q");
//    } else if (parameterID == "biquad_gain") {
        cached_biquad_gain = *tree.getRawParameterValue("biquad_gain");
//    }

//    if (parameterID.contains("biquad")) {
        updateFilter();
//    }
}

void Proto_galoisAudioProcessor::updateFilter() {
    for (int i = 0; i < num_channels; ++i) {
        biquad_filter[i].recalculate(
            host_sample_rate,
            cached_biquad_cutoff,
            cached_biquad_q,
            cached_biquad_gain,
            cached_biquad_type
            );
    }
}

void Proto_galoisAudioProcessor::cacheWaveforms() {
    cached_bit_depth = *tree.getRawParameterValue("bit_depth");
    cached_sample_rate = *tree.getRawParameterValue("sample_rate");
    cached_output_level = *tree.getRawParameterValue("output_level");
    cached_output_level = pow(cached_output_level * ROOT_2, 2);
    cached_input_level = *tree.getRawParameterValue("input_level");
    cached_input_level = pow(cached_input_level * ROOT_2, 2);
    cached_wf_base_wave = *tree.getRawParameterValue("wf_base_wave");
    cached_wf_power = *tree.getRawParameterValue("wf_power");
    cached_wf_fold = *tree.getRawParameterValue("wf_fold");
    cached_wf_harm_freq = *tree.getRawParameterValue("wf_harm_freq");
    cached_wf_harm_amp = *tree.getRawParameterValue("wf_harm_amp");
    cached_dry_blend = *tree.getRawParameterValue("dry_blend");
    cached_dry_blend_mode = *tree.getRawParameterValue("dry_blend_mode");
    cached_dry_blend_abs = abs(*tree.getRawParameterValue("dry_blend"));
    cached_dry_blend_sign = sgn(*tree.getRawParameterValue("dry_blend"));
    cached_bit_mask = *tree.getRawParameterValue("bit_mask");
    cached_filter_pre = *tree.getRawParameterValue("filter_pre");
    cached_filter_blend= *tree.getRawParameterValue("filter_blend");

    float waveform_resolution_half = (float)waveform_resolution / 2;
    for (int i = 0; i < waveform_resolution; i++) {
        float amp = (float)(i - waveform_resolution_half) / waveform_resolution_half;
        waveform_cache[i] = getWaveformValue(amp);
    }
}

juce::String Proto_galoisAudioProcessor::getFilterPosition() {
    int i = *tree.getRawParameterValue("filter_pre");
    return biquad_position_names[i];
}

juce::String Proto_galoisAudioProcessor::getFilterType() {
    return biquad_type_names[cached_biquad_type];
}

void Proto_galoisAudioProcessor::cycleParamValue(juce::String parameterID) {
    if (parameterID == "biquad_type") {
        int current = cached_biquad_type;
        ++current;
        if (current > 2) {
            current = 0;
        }
        cached_biquad_type = current;
        updateFilter();
    }
    else {
        int current = *tree.getRawParameterValue(parameterID);
        ++current;
        if (current > (tree.getParameter(parameterID)->getNormalisableRange().end)) {
            current = 0;
        }
        tree.getParameter(parameterID)->beginChangeGesture();
        tree.getParameter(parameterID)->setValueNotifyingHost(current);
        tree.getParameter(parameterID)->endChangeGesture();
        parameterChanged(parameterID, current);
    }
}


