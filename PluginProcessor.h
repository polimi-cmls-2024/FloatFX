/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Equalizer.h"
#include "Distortion.h"
#include "SerialDevice.h"

//==============================================================================
/**
*/
class EQAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    EQAudioProcessor();
    ~EQAudioProcessor() override;

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

    //===== FOR ARDUINO
    void initSerial();
    
    //To read data from Arduino
    SerialDevice serialDevice;

    juce::AudioProcessorValueTreeState equalizer_apvts;
    juce::AudioProcessorValueTreeState distortion_apvts;

private:
    
    Equalizer equalizer;
    const juce::StringArray filterTypes{ "LowPass Filter", "HighPass Filter", "BandPass Filter"};

    Distortion distortion;
    const juce::StringArray distortionTypes{ "Mode 1", "Mode 2", "Mode 3", "Mode 4" };

    // Delay
    juce::AudioBuffer<float> mDelayBuffer;
    int mWritePosition{ 0 };

    void fillDelayBuffer(int, const int, const int, const float*, const float*);

    void getFromDelayBuffer(juce::AudioBuffer<float> &, int, const int, const int, const float*, const float*);

    void feedbackDelay(int channel, const int bufferLength, const int delayBufferLength, float* dryBuffer);


   

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQAudioProcessor)
};
