/*
* Custom Button for the mapping buttons. This is a juce::TextButton containing a reference
* to its slider.
*/
#pragma once

#include <JuceHeader.h>

class MapButton : public juce::TextButton {
public:
    MapButton(juce::Slider* attachedSlider) : TextButton() {
        this->attachedSlider = attachedSlider;
    }
    juce::Slider* attachedSlider;
};