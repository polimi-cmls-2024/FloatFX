/*
  ==============================================================================

    MapButton.h
    Created: 25 May 2024 3:46:42pm
    Author:  Davide

  ==============================================================================
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