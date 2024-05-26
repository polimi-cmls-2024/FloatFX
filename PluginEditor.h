/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "MapButton.h"
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

    void initialize_delay_parameters();
    void resize_delay_parameters();
    
    void filterButtonClicked(int);
    void distortionButtonClicked(int);

    void initialize_out_parameters();
    void resize_out_parameters();

    void initialize_mapping_buttons();
    void resize_mapping_buttons();
    //For parameter mapping
    void buttonClicked(juce::Button* button) override;

    void initialize_mapping_button(MapButton& b);
    void mapButtonClicked(MapButton* b);


private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    EQAudioProcessor& audioProcessor;

    //Split the GUI into panels
    juce::Rectangle<int> eqPanel;
    juce::Rectangle<int> distortionPanel;
    juce::Rectangle<int> delayPanel;
    juce::Rectangle<int> volumePanel;
    juce::Rectangle<int> mapPanel;
    
    //Equalizer
    juce::Label eqPanelLabel;
    juce::Slider filterCutoff,Q;
    juce::Label QLabel;
    juce::Label filterCutoffLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterCutoffAttach, QAttach;
    
    std::array<juce::Label, 3> filterTypesLabels;
    std::array<juce::ToggleButton, 3> typeButtons;

    //Distortion
    juce::Label distortionPanelLabel;
    juce::Slider driveKnob, mixKnob, volumeKnob, angerKnob, LPFKnob, HPFKnob;
    juce::Label driveLabel, mixLabel, volumeLabel, angerLabel, LPFLabel, HPFLabel;
    std::array<juce::Label, 4> distortionTypesLabels;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttach, mixAttach, volumeAttach, angerAttach, LPFAttach, HPFAttach;

    std::array<juce::ToggleButton, 4> distortionTypeButtons;

    //Delay
    juce::Label delayPanelLabel;
    juce::Slider delayGain, delayTime;
    juce::Label delayGainLabel, delayTimeLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayGainAttach, delayTimeAttach;
    //To modify the sliders from incoming data

    void timerCallback() override;


    //Out Volume
    juce::Label outPanelLabel;
    juce::Slider outVolumeSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outVolumeAttach;
    

    //Parameters to map
    juce::Label mapPanelLabel;

    juce::Slider* param1{ nullptr };
    juce::Slider* param2{ nullptr };

    juce::TextButton param1Button;
    juce::TextButton param2Button;
    bool mapParameter1{ false };
    bool mapParameter2{ false };

    MapButton* mapButton1{ nullptr };
    MapButton* mapButton2{ nullptr };

    friend class MapButton;
    //Buttons for mapping parameters
    //EQ
    MapButton filterCutoffMap{ &filterCutoff }, QMap{ &Q };
    //Distortion
    MapButton driveMap{ &driveKnob }, angerMap{ &angerKnob }, distHPFMap{ &HPFKnob }, distLPFMap{ &LPFKnob }, distVolumeMap{ &volumeKnob }, distDryWetMap{ &mixKnob };
    //Delay
    MapButton feedbackMap{&delayGain}, delayTimeMap{&delayTime};




    // Colours
    juce::Colour textColor{ juce::Colour(230, 230, 230) };
    juce::Colour knobThumbColor{ juce::Colour(230, 230, 230) };
    juce::Colour knobBackgroundColor{ juce::Colour(160, 160, 160) };

    juce::Colour panelTitleColor{ juce::Colour(200, 200, 200) };
    juce::Colour panelBackgroundColorLight{ juce::Colour(30, 30, 30) };
    juce::Colour panelBackgroundColorDark{ juce::Colour(20, 20, 20) };

    juce::Colour map1ColorDark{ juce::Colour(6, 38, 173)};
    juce::Colour map1ColorLight{ juce::Colour(5, 73, 229) };
    juce::Colour map2ColorDark{ juce::Colour(175, 105, 2) };
    juce::Colour map2ColorLight{ juce::Colour(220, 133, 2) };
    juce::Colour mapNullColor{ juce::Colour(30, 30, 30) };



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQAudioProcessorEditor)
};
