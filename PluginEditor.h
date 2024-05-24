/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "MappableLabel.h"
//==============================================================================
/**
*/
class EQAudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::Timer, public juce::Button::Listener
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

    void initialize_mapping_buttons();
    void resize_mapping_buttons();
    //For parameter mapping
    void buttonClicked(juce::Button* button) override;

    void getMappingParameters();

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    EQAudioProcessor& audioProcessor;

    //Split the GUI into panels
    juce::Rectangle<int> EQPanel;
    juce::Rectangle<int> distortionPanel;
    juce::Rectangle<int> delayPanel;
    juce::Rectangle<int> volumePanel;
    juce::Rectangle<int> mapPanel;
    
    //Equalizer
    juce::Slider filterCutoff,Q;
    juce::Label QLabel;
    MappableLabel filterCutoffLabel{ &mapParameter1, &mapParameter2, &param1, &param2,
        &param1Button, &param2Button };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterCutoffAttach, QAttach;
    
    std::array<juce::Label, 3> filterTypesLabels;
    std::array<juce::ToggleButton, 3> typeButtons;

    //Distortion
    juce::Slider driveKnob, mixKnob, volumeKnob, angerKnob, LPFKnob, HPFKnob;
    juce::Label driveLabel, mixLabel, volumeLabel, angerLabel, LPFLabel, HPFLabel;
    std::array<juce::Label, 4> distortionTypesLabels;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttach, mixAttach, volumeAttach, angerAttach, LPFAttach, HPFAttach;

    std::array<juce::ToggleButton, 4> distortionTypeButtons;

    //juce::Label ModLabel;

    //To modify the sliders from incoming data

    void timerCallback() override;

    //Parameters to map

    juce::Slider* param1{ nullptr };
    juce::Slider* param2{ nullptr };

    juce::TextButton param1Button;
    juce::TextButton param2Button;
    bool mapParameter1{ false };
    bool mapParameter2{ false };

    float param1Range;
    float param2Range;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQAudioProcessorEditor)
};
