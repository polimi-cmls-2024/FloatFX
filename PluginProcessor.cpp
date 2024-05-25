/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

const juce::String kSerialPortName{ "\\\\.\\COM3" };

void EQAudioProcessor::initSerial() {
    serialDevice.init(kSerialPortName);
}
//==============================================================================
EQAudioProcessor::EQAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
                       equalizer_apvts(*this, nullptr), distortion_apvts(*this, nullptr), delay_apvts(*this, nullptr)
#endif
{
    
    initSerial();

    //Equalizer parameters
    equalizer_apvts.createAndAddParameter(std::make_unique<juce::AudioParameterFloat>("EQcutoff",
        "EQ Cutoff", juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f), 500.0f));
    equalizer_apvts.createAndAddParameter(std::make_unique<juce::AudioParameterFloat>("Q",
        "Q", juce::NormalisableRange<float>(0.01f, 10.0f, 0.01f), 5.0f));
    equalizer_apvts.createAndAddParameter(std::make_unique<juce::AudioParameterChoice>("type",
        "Filter Type", filterTypes, 0));

    equalizer_apvts.state = juce::ValueTree("savedParams");

    //Distortion parameters
    distortion_apvts.createAndAddParameter(std::make_unique<juce::AudioParameterFloat>("drive",
        "Drive", juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 0.0f));
    distortion_apvts.createAndAddParameter(std::make_unique<juce::AudioParameterFloat>("volume",
        "Volume", juce::NormalisableRange<float>(-20.0f, 20.0f, 0.5f), 0.0f, "dB"));
    distortion_apvts.createAndAddParameter(std::make_unique<juce::AudioParameterFloat>("distortion_mix",
        "Mix", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f, "%"));
    distortion_apvts.createAndAddParameter(std::make_unique<juce::AudioParameterFloat>("anger",
        "Anger", juce::NormalisableRange<float>(0.0f, 1.0f, 0.1f), 0.5f));
    distortion_apvts.createAndAddParameter(std::make_unique<juce::AudioParameterFloat>("hpf",
        "HPF Frequency", juce::NormalisableRange<float>(20.0f, 10000.0f, 1.0f, 0.25f), 20.0f, "Hz"));
    distortion_apvts.createAndAddParameter(std::make_unique<juce::AudioParameterFloat>("lpf",
        "LPF Frequency", juce::NormalisableRange<float>(200.0f, 20000.0f, 1.0f, 0.25f), 20000.0f, "Hz"));
    distortion_apvts.createAndAddParameter(std::make_unique<juce::AudioParameterChoice>("distortion_type",
        "Distortion Type", distortionTypes, 0));
    distortion_apvts.state = juce::ValueTree("savedParams");

    //Delay parameters
    delay_apvts.createAndAddParameter(std::make_unique<juce::AudioParameterFloat>("gain",
        "Gain", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));
    delay_apvts.createAndAddParameter(std::make_unique<juce::AudioParameterFloat>("delay_time",
        "delayTime", juce::NormalisableRange<float>(0, 2000, 50), 500));
    delay_apvts.state = juce::ValueTree("savedParams");
}

EQAudioProcessor::~EQAudioProcessor()
{
}

//==============================================================================
const juce::String EQAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool EQAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool EQAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool EQAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double EQAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int EQAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int EQAudioProcessor::getCurrentProgram()
{
    return 0;
}

void EQAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String EQAudioProcessor::getProgramName (int index)
{
    return {};
}

void EQAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void EQAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    const int numInputChannels = getTotalNumInputChannels();
    const int delayBufferSize = 2 *sampleRate;

    mDelayBuffer.setSize(numInputChannels, delayBufferSize);
    mDelayBuffer.clear();
    
    equalizer.prepare(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
    equalizer.setParameters(equalizer_apvts);

    distortion.prepare(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
    distortion.setParameters(distortion_apvts);
}

void EQAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool EQAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void EQAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear (i, 0, buffer.getNumSamples());


    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    //Equalize
    equalizer.setParameters(equalizer_apvts);
    equalizer.process(context);

    //Distort
    distortion.setParameters(distortion_apvts);
    distortion.process(context);


    //Delay
    const int bufferLength = buffer.getNumSamples();
    const int delayBufferLength = mDelayBuffer.getNumSamples();
    

    for (int channel = 0; channel < getTotalNumInputChannels(); ++channel) {

        float* dryBuffer = buffer.getWritePointer(channel);

        const float* bufferData = buffer.getReadPointer(channel);
        const float* delayBufferData = mDelayBuffer.getReadPointer(channel);

        const float gain = delay_apvts.getRawParameterValue("gain")->load();

        fillDelayBuffer(channel, bufferLength, delayBufferLength, bufferData, delayBufferData, gain);
        getFromDelayBuffer(buffer, channel, bufferLength, delayBufferLength, bufferData, delayBufferData);
        feedbackDelay(channel, bufferLength, delayBufferLength, dryBuffer, gain);
    }
    mWritePosition += bufferLength;

    //Circular buffer, in this way when we reach the end of the buffer,
    //we start from the beginning again
    mWritePosition %= delayBufferLength;

    
}

void EQAudioProcessor::fillDelayBuffer(int channel, const int bufferLength, const int delayBufferLength,
    const float* bufferData, const float* delayBufferData, const float gain) {

    if (delayBufferLength > bufferLength + mWritePosition) {
        mDelayBuffer.copyFromWithRamp(channel, mWritePosition, bufferData, bufferLength, gain, gain);

    }
    else {
        //In this case, i have to copy some samples at the end of the buffer, and the rest
        //at the beginning of the buffer.
        const int bufferRemaining = delayBufferLength - mWritePosition;
        mDelayBuffer.copyFromWithRamp(channel, mWritePosition, bufferData, bufferRemaining, gain, gain);
        mDelayBuffer.copyFromWithRamp(channel, 0, bufferData, bufferLength - bufferRemaining, gain, gain);
    }
}

void EQAudioProcessor::getFromDelayBuffer(juce::AudioBuffer<float>& buffer, int channel, const int bufferLength, const int delayBufferLength,
    const float* bufferData, const float* delayBufferData) {

    int delayTime = delay_apvts.getRawParameterValue("delay_time")->load(); ;
    const int readPosition = static_cast<int>(delayBufferLength + mWritePosition - (getSampleRate() * delayTime / 1000))%delayBufferLength;

    if(delayBufferLength > bufferLength + readPosition)
        buffer.addFrom(channel, 0, delayBufferData + readPosition, bufferLength);
    else {
        const int bufferRemaining = delayBufferLength - readPosition;
        buffer.addFrom(channel, 0, delayBufferData + readPosition, bufferRemaining);
        buffer.addFrom(channel, bufferRemaining, delayBufferData, bufferLength - bufferRemaining);
    }

}void EQAudioProcessor::feedbackDelay(int channel, const int bufferLength, const int delayBufferLength, float* dryBuffer, const float gain) {

    if (delayBufferLength > bufferLength + mWritePosition) {
        mDelayBuffer.addFromWithRamp(channel, mWritePosition, dryBuffer, bufferLength, gain, gain);
    }
    else {
        const int bufferRemaining = delayBufferLength - mWritePosition;
        mDelayBuffer.addFromWithRamp(channel, bufferRemaining, dryBuffer, bufferRemaining, gain, gain);
        mDelayBuffer.addFromWithRamp(channel, 0, dryBuffer, bufferLength - bufferRemaining, gain, gain);
        //mDelayBuffer.addFromWithRamp()
    }
}

//==============================================================================
bool EQAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* EQAudioProcessor::createEditor()
{
    return new EQAudioProcessorEditor (*this);
}

//==============================================================================
void EQAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void EQAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EQAudioProcessor();
}
