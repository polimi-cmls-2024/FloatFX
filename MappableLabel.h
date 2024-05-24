/*
  ==============================================================================

    MappableLabel.h
    Created: 24 May 2024 2:52:43pm
    Author:  Davide

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class MappableLabel : public juce::Label {

public:
    MappableLabel(bool* mapParameter1, bool* mapParameter2, juce::Slider** param1, juce::Slider** param2,
        juce::Button* param1Button, juce::Button* param2Button) : Label() 
    {
        this->mapParameter1 = mapParameter1;
        this->mapParameter2 = mapParameter2;
        this->param1 = param1;
        this->param2 = param2;
        this->param1Button = param1Button;
        this->param2Button = param2Button;
    }

    void mouseDoubleClick(const juce::MouseEvent& e) override {
        //This is needed to restore the white color of a parameter
        if(e.mouseWasClicked() && !*mapParameter1 && !*mapParameter2)
            this->setColour(this->textColourId, juce::Colours::white);
        if (e.mouseWasClicked() && *mapParameter1) {
            juce::Slider* component = dynamic_cast<juce::Slider*> (this->getAttachedComponent());

            if (component != NULL) {
                //If the parameter is already mapped on param2, remove the other mapping
                
                if (component == *param2)
                    *param2 = nullptr;
                this->setColour(this->textColourId, juce::Colours::red);
                *param1 = component;
                *mapParameter1 = false;
                param1Button->setToggleState(false, juce::NotificationType::dontSendNotification);
            }
        }
        if (e.mouseWasClicked() && *mapParameter2) {
            juce::Slider* component = dynamic_cast<juce::Slider*> (this->getAttachedComponent());

            if (component != NULL) {
                //If the parameter is already mapped on param2, remove the other mapping
                if (component == *param1)
                    *param1 = nullptr;
                this->setColour(this->textColourId, juce::Colours::red);
                *param2 = component;
                *mapParameter2 = false;
                param2Button->setToggleState(false, juce::NotificationType::dontSendNotification);
            }
        }
    }

private:
    
    bool* mapParameter1, * mapParameter2;
    juce::Slider** param1, **param2;
    juce::Button* param1Button, * param2Button;

};