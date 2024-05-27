

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <math.h>

//==============================================================================
EQAudioProcessorEditor::EQAudioProcessorEditor (EQAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{

    //Every 50 milliseconds
    startTimer(timerValue);

    //parameters to map
    param1 = param2 = nullptr;
    mapParameter1 = mapParameter2 = false;

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (640, 460);
    
    eqPanel.setBounds(5, 5, 255, 245);
    distortionPanel.setBounds(265, 5, 250, 450);
    delayPanel.setBounds(5, 255, 255, 200);
    volumePanel.setBounds(520, 5, 115, 290);
    mapPanel.setBounds(520, 300, 115, 155);

    initialize_equalizer_parameters();
    initialize_distortion_parameters();
    initialize_delay_parameters();
    initialize_out_parameters();
    initialize_mapping_buttons();
}

EQAudioProcessorEditor::~EQAudioProcessorEditor()
{
}

//==============================================================================
void EQAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::black);

    g.setColour(panelBackgroundColorLight);
    g.fillRect(eqPanel);
    g.fillRect(distortionPanel);
    g.fillRect(delayPanel);

    g.setColour(panelBackgroundColorDark);
    g.fillRect(volumePanel);
    g.fillRect(mapPanel);

    g.setFont (14.0f);
}

void EQAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    //ModLabel.setBounds(400, 400, 90, 20);
    resize_equalizer_parameters();
    resize_distortion_elements();
    resize_delay_parameters();
    resize_out_parameters();
    resize_mapping_buttons();

}

void EQAudioProcessorEditor::initialize_equalizer_parameters(){

    eqPanelLabel.setText("EQ", juce::dontSendNotification);
    eqPanelLabel.setColour(juce::Label::ColourIds::textColourId, panelTitleColor);
    eqPanelLabel.setJustificationType(12);

    addAndMakeVisible(eqPanelLabel);
    addAndMakeVisible(filterCutoff);
    addAndMakeVisible(filterCutoffMap);
    addAndMakeVisible(filterCutoffLabel);
    addAndMakeVisible(Q);
    addAndMakeVisible(QLabel);
    addAndMakeVisible(QMap);

    filterCutoff.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    filterCutoff.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    filterCutoff.setColour(juce::Slider::ColourIds::textBoxTextColourId, textColor);
    filterCutoff.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, knobBackgroundColor);
    filterCutoff.setColour(juce::Slider::ColourIds::thumbColourId, knobThumbColor);
    initialize_mapping_button(filterCutoffMap);

    Q.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    Q.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    Q.setColour(juce::Slider::ColourIds::textBoxTextColourId, textColor);
    Q.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, knobBackgroundColor);
    Q.setColour(juce::Slider::ColourIds::thumbColourId, knobThumbColor);
    initialize_mapping_button(QMap);

    filterCutoffLabel.setText("CUTOFF", juce::NotificationType::dontSendNotification);
    filterCutoffLabel.setColour(juce::Label::ColourIds::textColourId, textColor);
    QLabel.setText("Q", juce::NotificationType::dontSendNotification);
    QLabel.setColour(juce::Label::ColourIds::textColourId, textColor);
    filterCutoffAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.equalizer_apvts, "EQcutoff", filterCutoff);
    QAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.equalizer_apvts, "Q", Q);
    
    std::string labels[3]{ "LP", "HP", "BP" };
    for (int i = 0; i < 3; i++)
    {
        addAndMakeVisible(filterTypesLabels[i]);
        filterTypesLabels[i].setText(labels[i], juce::NotificationType::dontSendNotification);
        filterTypesLabels[i].setColour(juce::Label::ColourIds::textColourId, textColor);
    }

    // setup type buttons
    for (int i = 0; i < 3; i++)
    {
        addAndMakeVisible(typeButtons[i]);
        typeButtons[i].setRadioGroupId(1000);
        typeButtons[i].setColour(juce::Label::ColourIds::backgroundColourId, textColor);
        typeButtons[i].setColour(juce::Label::ColourIds::textColourId, textColor);
        typeButtons[i].setColour(juce::Label::ColourIds::backgroundColourId, textColor);
    }
    typeButtons[0].onClick = [&]() { filterButtonClicked(0); };
    typeButtons[1].onClick = [&]() { filterButtonClicked(1); };
    typeButtons[2].onClick = [&]() { filterButtonClicked(2); };
    // set button toggle for active index
    const int index = static_cast<int>(audioProcessor.equalizer_apvts.getRawParameterValue("type")->load());
    typeButtons[index].setToggleState(true, juce::NotificationType::dontSendNotification);
}

void EQAudioProcessorEditor::resize_equalizer_parameters(){
    const int knobWidth = 90;
    const int buttonWidth = 30;

    eqPanelLabel.setBounds(5, 15, 255, 30);

    filterCutoff.setBounds(20, 80, knobWidth, knobWidth);
    filterCutoffLabel.setBounds(55, 60, 90, 20);
    filterCutoffMap.setBounds(25, 60, 30, 20);
    //filterCutoffMap.setCentrePosition(filterCutoffLabel.getX() - 20, filterCutoffLabel.getY());

    Q.setBounds(120 + 20, 80, knobWidth, knobWidth);
    QLabel.setBounds(120 + 55 + 15, 60, 90, 20);
    QMap.setBounds(120 + 25 + 15, 60, 30, 20);
    //QMap.setCentrePosition(Q.getX() + knobWidth / 2, Q.getY() + 10 + knobWidth / 2);

    for (int i = 0; i < 3; i++)
    {
        typeButtons[i].setBounds(eqPanel.getTopLeft().getX() + 40 + i * 70, eqPanel.getTopLeft().getY() + 200, buttonWidth, buttonWidth);
        filterTypesLabels[i].setBounds(typeButtons[i].getX() + 25, eqPanel.getTopLeft().getY() + 205, 90, 20);
    }
}

void EQAudioProcessorEditor::initialize_distortion_parameters() {

    distortionPanelLabel.setText("Distortion", juce::dontSendNotification);
    distortionPanelLabel.setColour(juce::Label::ColourIds::textColourId, panelTitleColor);
    distortionPanelLabel.setJustificationType(12);

    driveKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    driveKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    driveKnob.setColour(juce::Slider::ColourIds::textBoxTextColourId, textColor);
    driveKnob.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, knobBackgroundColor);
    driveKnob.setColour(juce::Slider::ColourIds::thumbColourId, knobThumbColor);
    volumeKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    volumeKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    volumeKnob.setColour(juce::Slider::ColourIds::textBoxTextColourId, textColor);
    volumeKnob.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, knobBackgroundColor);
    volumeKnob.setColour(juce::Slider::ColourIds::thumbColourId, knobThumbColor);
    mixKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mixKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    mixKnob.setColour(juce::Slider::ColourIds::textBoxTextColourId, textColor);
    mixKnob.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, knobBackgroundColor);
    mixKnob.setColour(juce::Slider::ColourIds::thumbColourId, knobThumbColor);
    angerKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    angerKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    angerKnob.setColour(juce::Slider::ColourIds::textBoxTextColourId, textColor);
    angerKnob.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, knobBackgroundColor);
    angerKnob.setColour(juce::Slider::ColourIds::thumbColourId, knobThumbColor);
    HPFKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    HPFKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    HPFKnob.setColour(juce::Slider::ColourIds::textBoxTextColourId, textColor);
    HPFKnob.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, knobBackgroundColor);
    HPFKnob.setColour(juce::Slider::ColourIds::thumbColourId, knobThumbColor);
    LPFKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    LPFKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    LPFKnob.setColour(juce::Slider::ColourIds::textBoxTextColourId, textColor);
    LPFKnob.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, knobBackgroundColor);
    LPFKnob.setColour(juce::Slider::ColourIds::thumbColourId, knobThumbColor);

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
    
    addAndMakeVisible(distortionPanelLabel);
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

    const int knobWidth = 90;
    const int buttonWidth = 30;

    distortionPanelLabel.setBounds(265, 15, 250, 30);

    driveKnob.setBounds(260 + 20, 80, knobWidth, knobWidth);
    driveLabel.setBounds(260 + 55, 60, 90, 20);
    driveMap.setBounds(260 + 25, 60, 30, 20);
    //driveMap.setCentrePosition(driveKnob.getX() + driveKnobWidth / 2, driveKnob.getY()  -10 + driveKnobWidth / 2);

    mixKnob.setBounds(260 + 20 + 120, 80, knobWidth, knobWidth);
    mixLabel.setBounds(260 + 45 + 120, 60, 90, 20);
    distDryWetMap.setBounds(260 + 15 + 120, 60, 30, 20);
    //angerMap.setCentrePosition(angerKnob.getX() + secKnobWidth / 2, angerKnob.getY() - 10 + secKnobWidth / 2);


    HPFKnob.setBounds(260 + 20, 80 + 140, knobWidth, 80);
    HPFLabel.setBounds(260 + 65, 60 + 140, 90, 20);
    distHPFMap.setBounds(260 + 35, 60 + 140, 30, 20);
    //driveMap.setCentrePosition(driveKnob.getX() + driveKnobWidth / 2, driveKnob.getY()  -10 + driveKnobWidth / 2);

    LPFKnob.setBounds(260 + 20 + 120, 80 + 140, knobWidth, 80);
    LPFLabel.setBounds(260 + 60 + 120, 60 + 140, 90, 20);
    distLPFMap.setBounds(260 + 30 + 120, 60 + 140, 30, 20);
    //angerMap.setCentrePosition(angerKnob.getX() + secKnobWidth / 2, angerKnob.getY() - 10 + secKnobWidth / 2);


    volumeKnob.setBounds(260 + 20, 80 + 140 + 130, knobWidth, 80);
    volumeLabel.setBounds(260 + 50, 60 + 140 + 130, 90, 20);
    distVolumeMap.setBounds(260 + 20, 60 + 140 + 130, 30, 20);
    //driveMap.setCentrePosition(driveKnob.getX() + driveKnobWidth / 2, driveKnob.getY()  -10 + driveKnobWidth / 2);

    angerKnob.setBounds(260 + 20 + 120, 80 + 140 + 130, knobWidth, 80);
    angerLabel.setBounds(260 + 55 + 120, 60 + 140 + 130, 90, 20);
    angerMap.setBounds(260 + 25 + 120, 60 + 140 + 130, 30, 20);
    //angerMap.setCentrePosition(angerKnob.getX() + secKnobWidth / 2, angerKnob.getY() - 10 + secKnobWidth / 2);

    
    for (int type = 0; type < distortionTypeButtons.size(); type++)
    {
        distortionTypeButtons[type].setBounds(25 + 50 * type + 10000, mixLabel.getBottom() + 80, 20, 20);
        distortionTypesLabels[type].setBounds(15 + 50 * type + 10000, distortionTypeButtons[type].getBottom() -10, 90, 20);
    }
}

void EQAudioProcessorEditor::initialize_delay_parameters() {
    delayPanelLabel.setJustificationType(12);
    delayPanelLabel.setText("Delay", juce::dontSendNotification);
    delayPanelLabel.setColour(juce::Label::ColourIds::textColourId, panelTitleColor);
    addAndMakeVisible(delayPanelLabel);

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
    delayTimeLabel.setText("TIME", juce::NotificationType::dontSendNotification);

    delayGainLabel.setColour(juce::Label::ColourIds::textColourId, textColor);
    delayTimeLabel.setColour(juce::Label::ColourIds::textColourId, textColor);

    delayGain.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, knobBackgroundColor);
    delayGain.setColour(juce::Slider::ColourIds::thumbColourId, knobThumbColor);
    delayTime.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, knobBackgroundColor);
    delayTime.setColour(juce::Slider::ColourIds::thumbColourId, knobThumbColor);

}

void EQAudioProcessorEditor::resize_delay_parameters() {
    const int knobWidth = 90;
    const int buttonWidth = 30;

    delayPanelLabel.setBounds(5, 265, 255, 30);

    delayGain.setBounds(20, 80 + 260, knobWidth, knobWidth);
    delayGainLabel.setBounds(50, 60 + 260, 90, 20);
    feedbackMap.setBounds(20, 60 + 260, 30, 20);
    //feedbackMap.setCentrePosition(delayGain.getX() + knobDim / 2, delayGain.getY() - 10 + knobDim / 2);

    delayTime.setBounds(120 + 20, 80 + 260, knobWidth, knobWidth);
    delayTimeLabel.setBounds(120 + 60, 60 + 260, 90, 20);
    delayTimeMap.setBounds(120 + 30, 60 + 260, 30, 20);
    //delayTimeMap.setCentrePosition(delayTime.getX() + knobDim / 2, delayTime.getY() - 10 + knobDim / 2);

}

void EQAudioProcessorEditor::initialize_out_parameters() {
    outPanelLabel.setText("Out", juce::dontSendNotification);
    outPanelLabel.setColour(juce::Label::ColourIds::textColourId, panelTitleColor);
    outPanelLabel.setJustificationType(12);
    addAndMakeVisible(outPanelLabel);

    outVolumeSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    outVolumeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    outVolumeSlider.setColour(juce::Slider::ColourIds::textBoxTextColourId, textColor);
    outVolumeSlider.setColour(juce::Slider::ColourIds::trackColourId, knobBackgroundColor);
    outVolumeSlider.setColour(juce::Slider::ColourIds::thumbColourId, knobThumbColor);

    outVolumeAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.out_apvts, "out_volume", outVolumeSlider);
    addAndMakeVisible(outVolumeSlider);
}
void EQAudioProcessorEditor::resize_out_parameters() {
    outPanelLabel.setBounds(520, 15, 115, 30);
    outVolumeSlider.setBounds(540, 50, 80, 200);
}

void EQAudioProcessorEditor::initialize_mapping_buttons() {
    param1Button.setButtonText("MAP X");
    param2Button.setButtonText("MAP Y");

    mapPanelLabel.setText("Mapping", juce::dontSendNotification);
    mapPanelLabel.setColour(juce::Label::ColourIds::textColourId, panelTitleColor);
    mapPanelLabel.setJustificationType(12);

    param1Button.setColour(juce::TextButton::ColourIds::buttonColourId, map1ColorDark);
    param1Button.setColour(juce::TextButton::ColourIds::buttonOnColourId, map1ColorLight);
    param2Button.setColour(juce::TextButton::ColourIds::buttonColourId, map2ColorDark);
    param2Button.setColour(juce::TextButton::ColourIds::buttonOnColourId, map2ColorLight);

    param1Button.addListener(this);
    param2Button.addListener(this);

    addAndMakeVisible(param1Button);
    addAndMakeVisible(param2Button);
    addAndMakeVisible(mapPanelLabel);
}

void EQAudioProcessorEditor::resize_mapping_buttons() {
    mapPanelLabel.setBounds(520, 310, 115, 30);
    param1Button.setBounds(520 + 20, 360, 80, 30);
    param2Button.setBounds(520 + 20, 400, 80, 30);
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
    
    if (button == &param1Button) {
        button->setToggleState(true, juce::NotificationType::dontSendNotification);
        param2Button.setToggleState(false, juce::NotificationType::dontSendNotification);
        mapParameter1 = true;
        mapParameter2 = false;
    }
    if (button == &param2Button) {
        button->setToggleState(true, juce::NotificationType::dontSendNotification);
        param1Button.setToggleState(false, juce::NotificationType::dontSendNotification);
        mapParameter1 = false;
        mapParameter2 = true;
    }
    

}

void EQAudioProcessorEditor::initialize_mapping_button(MapButton& b) {
    b.addListener(this);
    b.setAlpha(50);
    b.onClick = [&]() { mapButtonClicked(&b); };
}

//This function contains the logic for:
//- mapping the accelerometer to a parameter
//- handle multiple mappings
//- handle the mapping changes
void EQAudioProcessorEditor::mapButtonClicked(MapButton* b) {
    if (mapParameter1) {
        if (mapButton1 != nullptr && mapButton1 != b) {
            mapButton1->setColour(juce::TextButton::ColourIds::buttonColourId, mapNullColor);
            if (param1 != nullptr) {
                param1->setColour(juce::Slider::ColourIds::rotarySliderFillColourId, knobBackgroundColor);
                param1->setColour(juce::Slider::ColourIds::thumbColourId, knobThumbColor);
            }
            if (mapButton1 == mapButton2) {
                mapButton2->setColour(juce::TextButton::ColourIds::buttonColourId, map2ColorLight);
                param2 = mapButton2->attachedSlider;
                param2->setColour(juce::Slider::ColourIds::rotarySliderFillColourId, map2ColorDark);
                param2->setColour(juce::Slider::ColourIds::thumbColourId, map2ColorLight);
            }
        }
        mapButton1 = b;
        b->setColour(juce::TextButton::ColourIds::buttonColourId, map1ColorLight);
        param1 = b->attachedSlider;
        param1->setColour(juce::Slider::ColourIds::rotarySliderFillColourId, map1ColorDark);
        param1->setColour(juce::Slider::ColourIds::thumbColourId, map1ColorLight);

        param1Button.setToggleState(false, juce::NotificationType::dontSendNotification);
        mapParameter1 = false;
    }
    else if (mapParameter2) {
        if (mapButton2 != nullptr && mapButton2 != b) {
            mapButton2->setColour(juce::TextButton::ColourIds::buttonColourId, mapNullColor);
            if (param2 != nullptr) {
                param2->setColour(juce::Slider::ColourIds::rotarySliderFillColourId, knobBackgroundColor);
                param2->setColour(juce::Slider::ColourIds::thumbColourId, knobThumbColor);
            }
            if (mapButton1 == mapButton2) {
                mapButton1->setColour(juce::TextButton::ColourIds::buttonColourId, map1ColorLight);
                param1 = mapButton1->attachedSlider;
                param1->setColour(juce::Slider::ColourIds::rotarySliderFillColourId, map1ColorDark);
                param1->setColour(juce::Slider::ColourIds::thumbColourId, map1ColorLight);
            }
        }
        mapButton2 = b;
        b->setColour(juce::TextButton::ColourIds::buttonColourId, map2ColorLight);
        param2 = b->attachedSlider;
        param2->setColour(juce::Slider::ColourIds::rotarySliderFillColourId, map2ColorDark);
        param2->setColour(juce::Slider::ColourIds::thumbColourId, map2ColorLight);

        param2Button.setToggleState(false, juce::NotificationType::dontSendNotification);
        mapParameter2 = false;
    }
    else {
        b->setColour(juce::TextButton::ColourIds::buttonColourId, mapNullColor);
        b->attachedSlider->setColour(juce::Slider::ColourIds::rotarySliderFillColourId, knobBackgroundColor);
        b->attachedSlider->setColour(juce::Slider::ColourIds::thumbColourId, knobThumbColor);
        if (b == mapButton1) {
            param1 = nullptr;
            mapButton1 = nullptr;
        }
        else {
            param2 = nullptr;
            mapButton2 = nullptr;
        }
    }
}

float logScale(float value) {
    return log(1.0 + value * 20.0) / log(21.0);
}

void EQAudioProcessorEditor::timerCallback() {
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
            DBG("AXIS:");
            DBG(m.direction);
            DBG("VERSE:");
            DBG(m.verse);
            DBG("VALUE");
            DBG(m.value);
            if (m.verse == MINUS_SIGN)
                mean_value -= (float)m.value;
            else {
                mean_value += (float)m.value;
            }
        }
        mean_value = mean_value / (float)i;

        if (param1 != nullptr && m.direction == X_AXIS) {
            float max = param1->getMaximum();
            float min = param1->getMinimum();
            float middle = (max + min) / 2;

            if (param1 == &filterCutoff || param1 == &HPFKnob || param1 == &LPFKnob) {
                if (m.verse == MINUS_SIGN) {
                    param1->setValue(middle + (max - middle) * (-logScale(float(abs(mean_value)) / 100.0)));
                }
                else {
                    param1->setValue(middle + (max - middle) * (float(mean_value) / 100.0));
                }
            }
            else {
                param1->setValue(middle + (max - middle) * (float(mean_value) / 100.0));
            }
        }
        if (param2 != nullptr && m.direction == Y_AXIS) {
            float max = param2->getMaximum();
            float min = param2->getMinimum();
            float middle = (max + min) / 2;
            
            if (param2 == &filterCutoff || param2 == &HPFKnob || param2 == &LPFKnob) {
                if (m.verse == MINUS_SIGN) {
                    param2->setValue(middle + (max - middle) * (-logScale(float(abs(mean_value)) / 100.0)));
                }
                else {
                    param2->setValue(middle + (max - middle) * (float(mean_value) / 100.0));
                }
            }
            else {
                param2->setValue(middle + (max - middle) * (float(mean_value) / 100.0));
            }
        }
    }
}
