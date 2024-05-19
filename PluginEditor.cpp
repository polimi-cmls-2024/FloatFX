/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
EQAudioProcessorEditor::EQAudioProcessorEditor (EQAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 600);
    
    addAndMakeVisible(ModLabel);
    ModLabel.setText("V4", juce::NotificationType::dontSendNotification);
    initialize_equalizer_parameters();
    initialize_distortion_parameters();
}

EQAudioProcessorEditor::~EQAudioProcessorEditor()
{
}

//==============================================================================
void EQAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    
}

void EQAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    ModLabel.setBounds(400, 400, 90, 20);
    resize_equalizer_parameters();
    resize_distortion_elements();
}

void EQAudioProcessorEditor::initialize_equalizer_parameters(){
    addAndMakeVisible(filterCutoff);
    addAndMakeVisible(filterGain);
    addAndMakeVisible(Q);
    addAndMakeVisible(filterCutoffLabel);
    addAndMakeVisible(filterGainLabel);
    addAndMakeVisible(QLabel);

    filterGain.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    filterCutoff.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    Q.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    filterGain.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    filterCutoff.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    Q.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);

    filterCutoffLabel.setText("CUTOFF", juce::NotificationType::dontSendNotification);
    filterGainLabel.setText("GAIN", juce::NotificationType::dontSendNotification);
    QLabel.setText("Q", juce::NotificationType::dontSendNotification);
    
    filterCutoffAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.equalizer_apvts, "EQcutoff", filterCutoff);
    QAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.equalizer_apvts, "Q", Q);
    filterGainAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.equalizer_apvts, "gain", filterGain);
    
    for (int i = 0; i < 3; i++)
    {
        addAndMakeVisible(filterTypesLabels[i]);
    }
    filterTypesLabels[0].setText("LPF", juce::NotificationType::dontSendNotification);
    filterTypesLabels[1].setText("HPF", juce::NotificationType::dontSendNotification);
    filterTypesLabels[2].setText("BANDPF", juce::NotificationType::dontSendNotification);

    // setup type buttons
    for (int i = 0; i < 3; i++)
    {
        addAndMakeVisible(typeButtons[i]);
        typeButtons[i].setRadioGroupId(1000);
    }
    typeButtons[0].onClick = [&]() { filterButtonClicked(0); };
    typeButtons[1].onClick = [&]() { filterButtonClicked(1); };
    typeButtons[2].onClick = [&]() { filterButtonClicked(2); };
    // set button toggle for active index
    const int index = static_cast<int>(audioProcessor.equalizer_apvts.getRawParameterValue("type")->load());
    typeButtons[index].setToggleState(true, juce::NotificationType::dontSendNotification);
}

void EQAudioProcessorEditor::resize_equalizer_parameters(){
    const int knobWidth = 100;
    const int buttonWidth = 30;
    filterCutoff.setBounds(0, 0, knobWidth, knobWidth);
    filterCutoffLabel.setBounds(filterCutoff.getRight(),knobWidth/2,90,20);
    Q.setBounds(0, 120, knobWidth, knobWidth);
    QLabel.setBounds(Q.getRight(), knobWidth + 20 + knobWidth / 2,90,20);
    filterGain.setBounds(0, 240, knobWidth, knobWidth);
    filterGainLabel.setBounds(filterGain.getRight(), 2*knobWidth + 20 + knobWidth / 2, 90, 20);
    for (int type = 0; type < typeButtons.size(); type++)
    {
        typeButtons[type].setBounds(40*type, 380, buttonWidth, buttonWidth);
        filterTypesLabels[type].setBounds(40 * type, typeButtons[type].getBottom() + 10, 90, 20);
    }
}

void EQAudioProcessorEditor::initialize_distortion_parameters() {

    addAndMakeVisible(driveKnob);
    addAndMakeVisible(volumeKnob);
    addAndMakeVisible(mixKnob);
    addAndMakeVisible(angerKnob);
    addAndMakeVisible(HPFKnob);
    addAndMakeVisible(LPFKnob);

    addAndMakeVisible(driveLabel);
    addAndMakeVisible(mixLabel);
    addAndMakeVisible(volumeLabel);
    addAndMakeVisible(angerLabel);
    addAndMakeVisible(LPFLabel);
    addAndMakeVisible(HPFLabel);

    driveKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    driveKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    volumeKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    volumeKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    mixKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mixKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    angerKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    angerKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    HPFKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    HPFKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    LPFKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    LPFKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);

    driveLabel.setText("DRIVE", juce::dontSendNotification);
    mixLabel.setText("DRY/WET", juce::dontSendNotification);
    volumeLabel.setText("VOLUME", juce::dontSendNotification);
    angerLabel.setText("ANGER", juce::dontSendNotification);
    LPFLabel.setText("LPF", juce::dontSendNotification);
    HPFLabel.setText("HPF", juce::dontSendNotification);

    for (int i = 0; i < 4; i++)
    {
        addAndMakeVisible(distortionTypesLabels[i]);
        distortionTypesLabels[i].setText("TYPE", juce::NotificationType::dontSendNotification);
    }
    

    driveAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.distortion_apvts, "drive", driveKnob);
    volumeAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.distortion_apvts, "volume", volumeKnob);
    mixAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.distortion_apvts, "distortion_mix", mixKnob);
    angerAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.distortion_apvts, "anger", angerKnob);
    HPFAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.distortion_apvts, "hpf", HPFKnob);
    LPFAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.distortion_apvts, "lpf", LPFKnob);

    for (int i = 0; i < 4; i++)
    {
        addAndMakeVisible(distortionTypeButtons[i]);
        distortionTypeButtons[i].setRadioGroupId(1001);
    }

    distortionTypeButtons[0].onClick = [&]() { distortionButtonClicked(0); };
    distortionTypeButtons[1].onClick = [&]() { distortionButtonClicked(1); };
    distortionTypeButtons[2].onClick = [&]() { distortionButtonClicked(2); };
    distortionTypeButtons[3].onClick = [&]() { distortionButtonClicked(3); };
    // set button toggle for active index
    const int index = static_cast<int>(audioProcessor.distortion_apvts.getRawParameterValue("distortion_type")->load());
    distortionTypeButtons[index].setToggleState(true, juce::NotificationType::dontSendNotification);

}

void EQAudioProcessorEditor::resize_distortion_elements() {
    const int leftPosition = 200;
    const int driveKnobWidth = 80;
    const int secKnobWidth = 40;
    driveKnob.setBounds(leftPosition+60, 0, driveKnobWidth, driveKnobWidth);
    driveLabel.setBounds(leftPosition +40 + driveKnobWidth / 2, driveKnobWidth, 90, 20);
    HPFKnob.setBounds(leftPosition + 40, driveLabel.getBottom() + 10, secKnobWidth, secKnobWidth);
    HPFLabel.setBounds(leftPosition + 40, HPFKnob.getBottom(), 90, 20);
    LPFKnob.setBounds(leftPosition + 120, driveLabel.getBottom() + 10, secKnobWidth, secKnobWidth);
    LPFLabel.setBounds(leftPosition + 120, LPFKnob.getBottom(), 90, 20);
    volumeKnob.setBounds(leftPosition + 40, HPFKnob.getBottom() + 10, secKnobWidth, secKnobWidth);
    volumeLabel.setBounds(leftPosition + 40, volumeKnob.getBottom(), 90, 20);
    angerKnob.setBounds(leftPosition + 120, LPFKnob.getBottom() + 10, secKnobWidth, secKnobWidth);
    angerLabel.setBounds(leftPosition + 120, angerKnob.getBottom(), 90, 20);

    mixKnob.setBounds(leftPosition + 80, angerLabel.getBottom() + 10, secKnobWidth, secKnobWidth);
    mixLabel.setBounds(leftPosition + 80, mixKnob.getBottom(), 90, 20);

    for (int type = 0; type < distortionTypeButtons.size(); type++)
    {
        distortionTypeButtons[type].setBounds(leftPosition + 40 * type, mixLabel.getBottom() + 20, secKnobWidth, secKnobWidth);
        distortionTypesLabels[type].setBounds(leftPosition + 40 * type, distortionTypeButtons[type].getBottom() + 10, 90, 20);
    }
}

void EQAudioProcessorEditor::filterButtonClicked(int index)
{
    const float choice = static_cast<float>(index / 3.0f);
    audioProcessor.equalizer_apvts.getParameter("type")->setValueNotifyingHost(choice);
}

void EQAudioProcessorEditor::distortionButtonClicked(int index)
{
    const float choice = static_cast<float>(index / 3.0f);
    audioProcessor.distortion_apvts.getParameter("distortion_type")->setValueNotifyingHost(choice);
}