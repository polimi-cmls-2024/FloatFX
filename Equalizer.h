/*
  ==============================================================================

    Equalizer.h
    Created: 19 May 2024 10:09:45am
    Author:  rizzo

  ==============================================================================
*/

#pragma once

struct EqualizerParameters {
    int cutoffFreq;
    float qFactor, gain;
    int type;
};

class Equalizer {

public:
    void setParameters(const juce::AudioProcessorValueTreeState& apvts)
    {
        parameters.cutoffFreq = apvts.getRawParameterValue("EQcutoff")->load();
        parameters.qFactor = apvts.getRawParameterValue("Q")->load();
        parameters.gain = apvts.getRawParameterValue("gain")->load();
        parameters.type = static_cast<int>(apvts.getRawParameterValue("type")->load());

    }

    void prepare(int sampleRate, int bufferSize, int nChannels) {
        this->sampleRate = sampleRate;
        this->bufferSize = bufferSize;
        this->nChannels = nChannels;

        juce::dsp::ProcessSpec spec;
        spec.sampleRate = sampleRate;
        spec.maximumBlockSize = bufferSize;
        spec.numChannels = nChannels;
        filter.prepare(spec);
    }

    void process(const juce::dsp::ProcessContextReplacing<float>& context) {

        auto block = (juce::dsp::AudioBlock<float>&) context.getInputBlock();

        block.multiplyBy(parameters.gain);
        applyFilter(block);
        


    }

private:
    void applyFilter(juce::dsp::AudioBlock<float>& block) {
        switch (parameters.type)
        {
        case 0: // Low Pass Filter
            *filter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, parameters.cutoffFreq, parameters.qFactor);
            break;
        case 1: // High Pass Filter
            *filter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, parameters.cutoffFreq, parameters.qFactor);
            break;
        case 2: // Band Pass Filter
            *filter.state = *juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate, parameters.cutoffFreq, parameters.qFactor);
            break;
        }

        juce::dsp::ProcessContextReplacing<float> filterContext(block);
        filter.process(filterContext);
    }

    EqualizerParameters parameters;

    int sampleRate;
    int bufferSize;
    int nChannels;

    using StereoFilter = juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
        juce::dsp::IIR::Coefficients<float>>;
    StereoFilter filter;
};

