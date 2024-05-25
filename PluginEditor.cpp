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
    initialize_delay_parameters();
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
    g.setColour(juce::Colour(255, 253, 241));
    g.fillRect(EQPanel);
    g.setColour(juce::Colour(253, 231, 220));
    g.fillRect(distortionPanel);
    g.setColour(juce::Colour(248, 189, 183));
    g.fillRect(delayPanel);
    g.setColour(juce::Colour(106, 68, 57));
    g.fillRect(volumePanel);
    g.setColour(juce::Colour(106, 68, 57));
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
    resize_delay_parameters();
    resize_mapping_buttons();
}

void EQAudioProcessorEditor::initialize_equalizer_parameters(){
    addAndMakeVisible(filterCutoff);
    addAndMakeVisible(filterCutoffMap);
    addAndMakeVisible(Q);
    addAndMakeVisible(filterCutoffLabel);
    addAndMakeVisible(QLabel);
    addAndMakeVisible(QMap);

    filterCutoff.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    filterCutoff.setTextBoxStyle(juce::Slider::TextBoxAbove, false, 100, 20);
    filterCutoff.setColour(juce::Slider::ColourIds::textBoxTextColourId, textColor);
    initialize_mapping_button(filterCutoffMap);
    Q.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    Q.setTextBoxStyle(juce::Slider::TextBoxAbove, false, 100, 20);
    Q.setColour(juce::Slider::ColourIds::textBoxTextColourId, textColor);
    initialize_mapping_button(QMap);

    filterCutoffLabel.setText("CUTOFF", juce::NotificationType::dontSendNotification);
    filterCutoffLabel.setColour(juce::Label::ColourIds::textColourId, textColor);
    QLabel.setText("Q", juce::NotificationType::dontSendNotification);
    QLabel.setColour(juce::Label::ColourIds::textColourId, textColor);
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
    filterCutoffMap.setBounds(EQPanel.getTopLeft().getX() /* + 25*/, EQPanel.getTopLeft().getY() + 40, knobWidth/3, knobWidth/3);
    filterCutoffMap.setCentrePosition(filterCutoff.getX() + knobWidth/2, filterCutoff.getY() + 10 + knobWidth / 2);
    Q.setBounds(EQPanel.getTopLeft().getX() + 25, filterCutoff.getBottom() + 20, knobWidth, knobWidth);
    QLabel.setBounds(Q.getX() + 35, Q.getY() -25, 90, 20);
    QMap.setBounds(EQPanel.getTopLeft().getX() + 25, filterCutoff.getBottom() + 20, knobWidth / 3, knobWidth / 3);
    QMap.setCentrePosition(Q.getX() + knobWidth / 2, Q.getY() + 10 + knobWidth / 2);
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
    driveKnob.setColour(juce::Slider::ColourIds::textBoxTextColourId, textColor);
    volumeKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    volumeKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    volumeKnob.setColour(juce::Slider::ColourIds::textBoxTextColourId, textColor);
    mixKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mixKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    mixKnob.setColour(juce::Slider::ColourIds::textBoxTextColourId, textColor);
    angerKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    angerKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    angerKnob.setColour(juce::Slider::ColourIds::textBoxTextColourId, textColor);
    HPFKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    HPFKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    HPFKnob.setColour(juce::Slider::ColourIds::textBoxTextColourId, textColor);
    LPFKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    LPFKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    LPFKnob.setColour(juce::Slider::ColourIds::textBoxTextColourId, textColor);


    driveLabel.setText("DRIVE", juce::dontSendNotification);
    driveLabel.setColour(juce::Label::ColourIds::textColourId, textColor);
    mixLabel.setText("DRY/WET", juce::dontSendNotification);
    mixLabel.setColour(juce::Label::ColourIds::textColourId, textColor);
    volumeLabel.setText("VOLUME", juce::dontSendNotification);
    volumeLabel.setColour(juce::Label::ColourIds::textColourId, textColor);
    angerLabel.setText("ANGER", juce::dontSendNotification);
    angerLabel.setColour(juce::Label::ColourIds::textColourId, textColor);
    LPFLabel.setText("LPF", juce::dontSendNotification);
    LPFLabel.setColour(juce::Label::ColourIds::textColourId, textColor);
    HPFLabel.setText("HPF", juce::dontSendNotification);
    HPFLabel.setColour(juce::Label::ColourIds::textColourId, textColor);

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

    addAndMakeVisible(driveMap);
    addAndMakeVisible(angerMap);
    addAndMakeVisible(distHPFMap);
    addAndMakeVisible(distLPFMap);
    addAndMakeVisible(distVolumeMap);
    addAndMakeVisible(distDryWetMap);
    
    initialize_mapping_button(driveMap);
    initialize_mapping_button(angerMap);
    initialize_mapping_button(distHPFMap);
    initialize_mapping_button(distLPFMap);
    initialize_mapping_button(distVolumeMap);
    initialize_mapping_button(distDryWetMap);

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
    driveMap.setBounds(leftPosition + 10, 20, driveKnobWidth/3, driveKnobWidth/3);
    driveMap.setCentrePosition(driveKnob.getX() + driveKnobWidth / 2, driveKnob.getY()  -10 + driveKnobWidth / 2);
    angerKnob.setBounds(driveKnob.getX() + 120, 30, secKnobWidth, secKnobWidth);
    angerLabel.setBounds(angerKnob.getX()+10, angerKnob.getY() - 20, 90, 20);
    angerMap.setBounds(driveKnob.getX() + 120, 30, secKnobWidth/4, secKnobWidth/4);
    angerMap.setCentrePosition(angerKnob.getX() + secKnobWidth / 2, angerKnob.getY() - 10 + secKnobWidth / 2);
    
    HPFKnob.setBounds(leftPosition + 20, driveKnob.getBottom()+ 10, secKnobWidth, secKnobWidth);
    HPFLabel.setBounds(HPFKnob.getX() + 20, HPFKnob.getY()-10, 90, 20);
    distHPFMap.setBounds(leftPosition + 20, driveKnob.getBottom() + 10, secKnobWidth / 4, secKnobWidth / 4);
    distHPFMap.setCentrePosition(HPFKnob.getX() + secKnobWidth / 2, HPFKnob.getY() - 10 + secKnobWidth / 2);
    LPFKnob.setBounds(rightPosition - 70, driveKnob.getBottom() + 10, secKnobWidth, secKnobWidth);
    LPFLabel.setBounds(rightPosition -45, LPFKnob.getY()-10, 90, 20);
    distLPFMap.setBounds(rightPosition - 70, driveKnob.getBottom() + 10, secKnobWidth / 4, secKnobWidth / 4);
    distLPFMap.setCentrePosition(LPFKnob.getX() + secKnobWidth / 2, LPFKnob.getY() - 10 + secKnobWidth / 2);
    volumeKnob.setBounds(leftPosition + 20, HPFKnob.getBottom() + 20, secKnobWidth, secKnobWidth);
    volumeLabel.setBounds(leftPosition + 30, volumeKnob.getY() - 10, 90, 20);
    distVolumeMap.setBounds(leftPosition + 20, HPFKnob.getBottom() + 20, secKnobWidth / 4, secKnobWidth / 4);
    distVolumeMap.setCentrePosition(volumeKnob.getX() + secKnobWidth / 2, volumeKnob.getY() - 10 + secKnobWidth / 2);
    
    
    mixKnob.setBounds(rightPosition - 70, HPFKnob.getBottom() + 20, secKnobWidth, secKnobWidth);
    mixLabel.setBounds(rightPosition -60, mixKnob.getY() -10, 90, 20);
    distDryWetMap.setBounds(rightPosition - 70, HPFKnob.getBottom() + 20, secKnobWidth / 4, secKnobWidth / 4);
    distDryWetMap.setCentrePosition(mixKnob.getX() + secKnobWidth / 2, mixKnob.getY() - 10 + secKnobWidth / 2);

    for (int type = 0; type < distortionTypeButtons.size(); type++)
    {
        distortionTypeButtons[type].setBounds(leftPosition + 25 + 50 * type, mixLabel.getBottom() + 80, secKnobWidth, secKnobWidth);
        distortionTypesLabels[type].setBounds(leftPosition + 15 + 50 * type, distortionTypeButtons[type].getBottom() -10, 90, 20);
    }
}

void EQAudioProcessorEditor::initialize_delay_parameters() {
    addAndMakeVisible(delayGain);
    addAndMakeVisible(delayTime);
    addAndMakeVisible(feedbackMap);
    addAndMakeVisible(delayTimeMap);
    addAndMakeVisible(delayGainLabel);
    addAndMakeVisible(delayTimeLabel);

    initialize_mapping_button(feedbackMap);
    initialize_mapping_button(delayTimeMap);

    delayGain.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    delayTime.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    delayGain.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    delayTime.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    delayGain.setColour(juce::Slider::ColourIds::textBoxTextColourId, textColor);
    delayTime.setColour(juce::Slider::ColourIds::textBoxTextColourId, textColor);
    delayGainAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.delay_apvts, "gain", delayGain);
    delayTimeAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.delay_apvts, "delay_time", delayTime);

    delayGainLabel.setText("FEEDBACK", juce::NotificationType::dontSendNotification);
    delayTimeLabel.setText("DELAY TIME", juce::NotificationType::dontSendNotification);

    delayGainLabel.setColour(juce::Label::ColourIds::textColourId, textColor);
    delayTimeLabel.setColour(juce::Label::ColourIds::textColourId, textColor);


}

void EQAudioProcessorEditor::resize_delay_parameters() {
    int knobDim = 180;
    delayGain.setBounds(390 + 20, 50, knobDim, knobDim);
    delayGainLabel.setBounds(delayGain.getX() + knobDim / 2 - 20, delayGain.getY() - 20, 90,20);
    feedbackMap.setBounds(390 + 20, 50, 80, 80);
    feedbackMap.setCentrePosition(delayGain.getX() + knobDim / 2, delayGain.getY() - 10 + knobDim / 2);
    delayTime.setBounds(390 + 20, 250, knobDim, knobDim);
    delayTimeLabel.setBounds(delayTime.getX() + knobDim / 2 - 20, delayTime.getY() - 20, 90, 20);
    delayTimeMap.setBounds(390 + 20, 250, 80, 80);

    delayTimeMap.setCentrePosition(delayTime.getX() + knobDim / 2, delayTime.getY() - 10 + knobDim / 2);
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
    
    /*if (mapParameter1 == true) {
        param1Button.setToggleState(false, juce::NotificationType::dontSendNotification);
        mapParameter1 = false;
    }
    if (mapParameter2 == true) {
        param2Button.setToggleState(false, juce::NotificationType::dontSendNotification);
        mapParameter2 = false;
    }*/
    if (button == &param1Button) {
        button->setToggleState(true, juce::NotificationType::dontSendNotification);
        mapParameter1 = true;
    }
    if (button == &param2Button) {
        button->setToggleState(true, juce::NotificationType::dontSendNotification);
        mapParameter2 = true;
    }
    /*if (button == &filterCutoffMap) {
        if (mapParameter1)
            param1 = &filterCutoff;
        if (mapParameter2)
            param2 = &filterCutoff;
    }*/
    

}

void EQAudioProcessorEditor::initialize_mapping_button(MapButton& b) {
    b.addListener(this);
    b.setAlpha(0);
    b.onClick = [&]() { mapButtonClicked(&b); };
}

void EQAudioProcessorEditor::mapButtonClicked(MapButton* b) {
    if (mapParameter1) {
        param1 = b->attachedSlider;
        param1Button.setToggleState(false, juce::NotificationType::dontSendNotification);
        mapParameter1 = false;
    }
    if (mapParameter2) {
        param2 = b->attachedSlider;
        param2Button.setToggleState(false, juce::NotificationType::dontSendNotification);
        mapParameter2 = false;
    }
}

void EQAudioProcessorEditor::timerCallback() {
    DBG("TIMER CALLBACK CHIAMATA");
    DBG("DIMENSIONE STACK MESSAGES");
    DBG(audioProcessor.serialDevice.messages.size());
    int messages_to_pop = 50;
    float mean_value = 0;
    int i = 0;
    if (audioProcessor.serialDevice.isConnected) {
        Message m;
        for (i = 0; i < messages_to_pop; i++) {
            if (audioProcessor.serialDevice.messages.empty())
                break;
            m = audioProcessor.serialDevice.messages.top();
            audioProcessor.serialDevice.messages.pop();
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
            mean_value += (float)m.value;
            
        }
        DBG("i VALUE");
        DBG(i);
        mean_value = mean_value / (float)i;
        DBG("MEAN VALUE: ");
        DBG(mean_value);
        if (param1 != nullptr && m.direction == X_AXIS) {
            float max = param1->getMaximum();
            float min = param1->getMinimum();
            float middle = (max - min) / 2;
            param1->setValue( middle + (max-middle) * (float(mean_value) / 100.0));
        }
        if (param2 != nullptr && m.direction == Y_AXIS) {
            float max = param2->getMaximum();
            float min = param2->getMinimum();
            float middle = (max - min) / 2;
            param2->setValue(middle + (max - middle) * (float(mean_value) / 100.0));
        }
    }
}