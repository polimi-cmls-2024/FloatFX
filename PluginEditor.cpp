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

    //Every 50 milliseconds
    startTimer(50);

    //parameters to map
    param1 = param2 = nullptr;
    mapParameter1 = mapParameter2 = false;

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (700, 500);
    
    EQPanel.setBounds(0, 0, 160, 450);
    distortionPanel.setBounds(EQPanel.getTopRight().getX(), 0, 230, 450);
    delayPanel.setBounds(distortionPanel.getTopRight().getX(), 0, 230, 450);
    volumePanel.setBounds(delayPanel.getTopRight().getX(), 0, 100, 450);
    mapPanel.setBounds(0, EQPanel.getBottomLeft().getY(), 700, 50);

    initialize_equalizer_parameters();
    initialize_distortion_parameters();
    initialize_mapping_buttons();
}

EQAudioProcessorEditor::~EQAudioProcessorEditor()
{
}

//==============================================================================
void EQAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::aquamarine);
    g.setColour(juce::Colours::lightpink);
    g.fillRect(EQPanel);
    g.setColour(juce::Colours::rebeccapurple);
    g.fillRect(distortionPanel);
    g.setColour(juce::Colours::lightcyan);
    g.fillRect(delayPanel);
    g.setColour(juce::Colours::blue);
    g.fillRect(volumePanel);
    g.setColour(juce::Colours::darkgrey);
    g.fillRect(mapPanel);

    g.setFont (15.0f);
    
}

void EQAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    //ModLabel.setBounds(400, 400, 90, 20);
    resize_equalizer_parameters();
    resize_distortion_elements();
    resize_mapping_buttons();
}

void EQAudioProcessorEditor::initialize_equalizer_parameters(){
    addAndMakeVisible(filterCutoff);
    addAndMakeVisible(Q);
    addAndMakeVisible(filterCutoffLabel);
    addAndMakeVisible(QLabel);

    filterCutoff.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    filterCutoffLabel.attachToComponent(&filterCutoff,false);
    filterCutoff.setTextBoxStyle(juce::Slider::TextBoxAbove, false, 100, 20);
    Q.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    Q.setTextBoxStyle(juce::Slider::TextBoxAbove, false, 100, 20);

    filterCutoffLabel.setText("CUTOFF", juce::NotificationType::dontSendNotification);
    QLabel.setText("Q", juce::NotificationType::dontSendNotification);
    filterCutoffAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.equalizer_apvts, "EQcutoff", filterCutoff);
    QAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.equalizer_apvts, "Q", Q);
    
    std::string labels[3]{ "LPF", "HPF", "BANDPF" };
    for (int i = 0; i < 3; i++)
    {
        addAndMakeVisible(filterTypesLabels[i]);
        filterTypesLabels[i].setText(labels[i], juce::NotificationType::dontSendNotification);
        
    }

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
    filterCutoff.setBounds(EQPanel.getTopLeft().getX()+25, EQPanel.getTopLeft().getY()+40, knobWidth, knobWidth);
    filterCutoffLabel.setBounds(filterCutoff.getX()+20, filterCutoff.getY() - 20, 90, 20);
    Q.setBounds(EQPanel.getTopLeft().getX() + 25, filterCutoff.getBottom() + 20, knobWidth, knobWidth);
    QLabel.setBounds(Q.getX() + 35, Q.getY() -25, 90, 20);
    for (int i = 0; i < 3; i++)
    {
    
        typeButtons[i].setBounds(Q.getX(), Q.getBottom()+20+i*40, buttonWidth, buttonWidth);
        filterTypesLabels[i].setBounds(typeButtons[i].getX() + 50 , typeButtons[i].getY() + 5, 90, 20);
        //filterTypesLabels
    }
}

void EQAudioProcessorEditor::initialize_distortion_parameters() {


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

    for (int i = 0; i < 4; i++)
    {
        addAndMakeVisible(distortionTypesLabels[i]);
        distortionTypesLabels[i].setText("TYPE "+std::to_string(i), juce::NotificationType::dontSendNotification);
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
    const int leftPosition = 160;
    const int rightPosition = 360;
    const int driveKnobWidth = 100;
    const int secKnobWidth = 80;
    driveKnob.setBounds(leftPosition+10, 20, driveKnobWidth, driveKnobWidth);
    driveLabel.setBounds(driveKnob.getX() + 25 , driveKnob.getY() - 20, 90, 20);
    angerKnob.setBounds(driveKnob.getX() + 120, 30, secKnobWidth, secKnobWidth);
    angerLabel.setBounds(angerKnob.getX()+10, angerKnob.getY() - 20, 90, 20);

    
    HPFKnob.setBounds(leftPosition + 20, driveKnob.getBottom()+ 10, secKnobWidth, secKnobWidth);
    HPFLabel.setBounds(HPFKnob.getX() + 20, HPFKnob.getY()-10, 90, 20);
    LPFKnob.setBounds(rightPosition - 70, driveKnob.getBottom() + 10, secKnobWidth, secKnobWidth);
    LPFLabel.setBounds(rightPosition -45, LPFKnob.getY()-10, 90, 20);
    volumeKnob.setBounds(leftPosition + 20, HPFKnob.getBottom() + 20, secKnobWidth, secKnobWidth);
    volumeLabel.setBounds(leftPosition + 30, volumeKnob.getY() - 10, 90, 20);
    
    
    mixKnob.setBounds(rightPosition - 70, HPFKnob.getBottom() + 20, secKnobWidth, secKnobWidth);
    mixLabel.setBounds(rightPosition -60, mixKnob.getY() -10, 90, 20);

    for (int type = 0; type < distortionTypeButtons.size(); type++)
    {
        distortionTypeButtons[type].setBounds(leftPosition + 25 + 50 * type, mixLabel.getBottom() + 80, secKnobWidth, secKnobWidth);
        distortionTypesLabels[type].setBounds(leftPosition + 15 + 50 * type, distortionTypeButtons[type].getBottom() -10, 90, 20);
    }
}
void EQAudioProcessorEditor::initialize_mapping_buttons() {
    param1Button.setButtonText("MAP X PARAMETER");
    param2Button.setButtonText("MAP Y PARAMETER");

    param1Button.addListener(this);
    param2Button.addListener(this);

    addAndMakeVisible(param1Button);
    addAndMakeVisible(param2Button);
}

void EQAudioProcessorEditor::resize_mapping_buttons() {
    param1Button.setBounds(100, 450 + 10, 150, 30);
    param2Button.setBounds(300, 450 + 10, 150, 30);
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

void EQAudioProcessorEditor::buttonClicked(juce::Button* button) {
    button->setToggleState(true, juce::NotificationType::dontSendNotification);
    if (mapParameter1 == true) {
        param1Button.setToggleState(false, juce::NotificationType::dontSendNotification);
        mapParameter1 = false;
    }
    if (mapParameter2 == true) {
        param2Button.setToggleState(false, juce::NotificationType::dontSendNotification);
        mapParameter2 = false;
    }
    if (button == &param1Button) {
        mapParameter1 = true;
    }
    if (button == &param2Button) {
        mapParameter2 = true;
    }
}

void EQAudioProcessorEditor::getMappingParameters() {

}

void EQAudioProcessorEditor::timerCallback() {
    if (audioProcessor.serialDevice.isConnected) {
        Message m = audioProcessor.serialDevice.messages.top();
        //DBG("MESSAGE");
        DBG("AXIS:");
        DBG(m.direction);
        DBG("VERSE:");
        DBG(m.verse);
        DBG("VALUE");
        DBG(m.value);
        //DBG("\n");
        if (m.verse == MINUS_SIGN)
            m.value = -m.value;
        if (param1 != nullptr && m.direction == X_AXIS) {
            float max = param1->getMaximum();
            float min = param1->getMinimum();
            float middle = (max - min) / 2;
            param1->setValue( middle + (max-middle) * (float(m.value) / 100.0));
        }
        if (param2 != nullptr && m.direction == Y_AXIS) {
            float max = param2->getMaximum();
            float min = param2->getMinimum();
            float middle = (max - min) / 2;
            param2->setValue(middle + (max - middle) * (float(m.value) / 100.0));
        }
    }
}