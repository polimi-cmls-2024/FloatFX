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
class EQAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    EQAudioProcessorEditor (EQAudioProcessor&);
    ~EQAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void initialize_equalizer_parameters();
    void resize_equalizer_parameters();
    
    void initialize_distortion_parameters();
    void resize_distortion_elements();
    
    void filterButtonClicked(int);
    void distortionButtonClicked(int);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    EQAudioProcessor& audioProcessor;
    
    //Equalizer
    juce::Slider filterCutoff, filterGain, Q;
    juce::Label filterCutoffLabel, filterGainLabel, QLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterCutoffAttach, filterGainAttach, QAttach;
    
    std::array<juce::Label, 3> filterTypesLabels;
    std::array<juce::ToggleButton, 3> typeButtons;

    //Distortion
    juce::Slider driveKnob, mixKnob, volumeKnob, angerKnob, LPFKnob, HPFKnob;
    juce::Label driveLabel, mixLabel, volumeLabel, angerLabel, LPFLabel, HPFLabel;
    std::array<juce::Label, 4> distortionTypesLabels;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttach, mixAttach, volumeAttach, angerAttach, LPFAttach, HPFAttach;

    std::array<juce::ToggleButton, 4> distortionTypeButtons;

    juce::Label ModLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQAudioProcessorEditor)
};
