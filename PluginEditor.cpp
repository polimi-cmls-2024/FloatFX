/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FloatEQAudioProcessorEditor::FloatEQAudioProcessorEditor (FloatEQAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize(400, 300);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    freqSlider.setRange(20, 20000);
    freqSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    freqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    freqLabel.setText("F", juce::dontSendNotification);
    addAndMakeVisible(freqSlider);
    addAndMakeVisible(freqLabel);

    gainSlider.setRange(-30.0, 30.0);
    gainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    gainLabel.setText("Gain", juce::dontSendNotification);
    addAndMakeVisible(gainSlider);
    addAndMakeVisible(gainLabel);

    qSlider.setRange(0.0, 20.0);
    qSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    qSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    qLabel.setText("Q", juce::dontSendNotification);
    addAndMakeVisible(qSlider);
    addAndMakeVisible(qLabel);

    bellButton.setButtonText("Bell");
    addAndMakeVisible(bellButton);

    bellButton.setButtonText("Shelf");
    addAndMakeVisible(shelfButton);

    cutButton.setButtonText("Cut");
    addAndMakeVisible(cutButton);

    frequencySliderAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts,
            "FREQ", freqSlider);
    qualitySliderAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "Q",
            qSlider);
    gainSliderAttachment = 
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "GAIN",
            gainSlider);

}

FloatEQAudioProcessorEditor::~FloatEQAudioProcessorEditor()
{
}

//==============================================================================
void FloatEQAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

}

void FloatEQAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    freqSlider.setBounds(10, 10, 256, 256);
    freqLabel.setBounds(100, 10, 90, 20);

    gainSlider.setBounds(10, 50, 256, 256);
    gainLabel.setBounds(100, 50, 90, 20);

    qSlider.setBounds(10, 90, 256, 256);
    qLabel.setBounds(100, 90, 90, 20);
}