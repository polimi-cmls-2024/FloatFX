/*
  ==============================================================================

    OscManager.h
    Created: 25 May 2024 2:25:03pm
    Author:  Guido

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class OscManager
{
public:
    OscManager() {
        oscSender.connect(ip, port);
    }
    void sendSpectrum(float* fftData, int fftSize)
    {
        juce::OSCAddressPattern address(spectrumAddress);
        juce::OSCMessage message(address);

        for (int i = 0; i < fftSize; ++i) {
            message.addFloat32(fftData[i]);
        }

        oscSender.send(message);
    }

private:

    std::string spectrumAddress = "/spectrum";

    std::string ip = "127.0.0.1";
    int port = 7771;

    juce::OSCSender oscSender;
};
