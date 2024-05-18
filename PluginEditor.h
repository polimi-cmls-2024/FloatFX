/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class FloatEQAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    FloatEQAudioProcessorEditor (FloatEQAudioProcessor&);
    ~FloatEQAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;


private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FloatEQAudioProcessor& audioProcessor;

    //Slider, Button and Label declarations

    juce::Slider freqSlider, qSlider, gainSlider;
    juce::Label freqLabel, qLabel, gainLabel;
    juce::TextButton bellButton, shelfButton, cutButton;

    //To connect GUI components to parameters
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> qualitySliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> frequencySliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainSliderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FloatEQAudioProcessorEditor)
};
