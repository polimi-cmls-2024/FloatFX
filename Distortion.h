/*
  ==============================================================================

    Distortion.h
    Created: 19 May 2024 4:10:24pm
    Author:  Davide

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#define float_Pi 3.1415

struct DistortionParameters {
    float drive, mix, anger, volume;
    int hpf_freq, lpf_freq;

    int distortion_type;
};


class Distortion
{
public:
    void setParameters(const juce::AudioProcessorValueTreeState& apvts)
    {
        parameters.drive = apvts.getRawParameterValue("drive")->load();
        parameters.mix = apvts.getRawParameterValue("distortion_mix")->load();
        parameters.hpf_freq = apvts.getRawParameterValue("hpf")->load();
        parameters.lpf_freq = apvts.getRawParameterValue("lpf")->load();
        parameters.distortion_type = static_cast<int>(apvts.getRawParameterValue("distortion_type")->load());
        parameters.anger = apvts.getRawParameterValue("anger")->load();
        parameters.volume = apvts.getRawParameterValue("volume")->load();
    }

    void prepare(double inputSampleRate, int maxBlockSize, int output_channels)
    {   
        sample_rate = inputSampleRate;
        buffer_size = maxBlockSize;
        num_channels = output_channels;

        dryBuffer.setSize(num_channels, maxBlockSize);

        juce::dsp::ProcessSpec spec;
        spec.sampleRate = sample_rate;
        spec.maximumBlockSize = buffer_size;
        spec.numChannels = num_channels;

        filterChain.prepare(spec);
    }


    void process(const juce::dsp::ProcessContextReplacing<float>& context)
    {
        auto block = (juce::dsp::AudioBlock<float>&) context.getInputBlock();

        for (int channel = 0; channel < num_channels; channel++)
        {
            dryBuffer.copyFrom(channel, 0, block.getChannelPointer(channel), buffer_size);
        }
        applyInputFilters(block);
        distortBuffer(block);
        applyMix(block, dryBuffer);
        //context.getOutputBlock();
    }

private:
    //This function applies filters on the wet distorted signal.
    //The user can decide which frequencies to cut off.
    void applyInputFilters(juce::dsp::AudioBlock<float>& block)
    {
        *filterChain.get<FilterChainIndex::HPF>().state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sample_rate, parameters.hpf_freq, 5.0f);
        *filterChain.get<FilterChainIndex::LPF>().state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(sample_rate, parameters.lpf_freq, 5.0f);

        juce::dsp::ProcessContextReplacing<float> filterContext(block);
        filterChain.process(filterContext);
    }

    void distortBuffer(juce::dsp::AudioBlock<float>& block)
    {
        const float outputGain = juce::Decibels::decibelsToGain(parameters.volume);
        const float autoGain = juce::Decibels::decibelsToGain(parameters.drive / -5.0f) *
            (-0.7f * parameters.anger + 1.0f);
        for (int sample = 0; sample < buffer_size; sample++)
        {
            for (int channel = 0; channel < num_channels; channel++)
            {
                float wetSample = block.getSample(channel, sample);
                wetSample *= (parameters.drive / 10.0f) + 1.0f;         // apply drive
                distortSample(wetSample, parameters.distortion_type);    // apply distortion
                wetSample *= autoGain;                                  // apply autogain
                wetSample *= outputGain;                                // apply volume
                block.setSample(channel, sample, wetSample);
            }
        }
    }

    void distortSample(float& sample, int type)
    {
        float angerValue;
        switch (type)
        {
        case 0: // inverse absolute value
            angerValue = -0.9f * parameters.anger + 1.0f;
            sample = sample / (angerValue + abs(sample));
            break;
        case 1: // arctan
            angerValue = -2.5f * parameters.anger + 3.0f;
            sample = (2.0f / float_Pi) * atan((float_Pi / angerValue) * sample);
            break;
        case 2: // erf
            angerValue = -2.5f * parameters.anger + 3.0f;
            sample = erf(sample * sqrt(float_Pi) / angerValue);
            break;
        case 3: // inverse square root
            angerValue = 4.5f * parameters.anger + 0.5f;
            sample = sample / sqrt((1.0f / angerValue) + (sample * sample));
            break;
        }
    }

    void applyMix(juce::dsp::AudioBlock<float>& wetBlock, const juce::AudioBuffer<float>& dryBlock)
    {
        for (int sample = 0; sample < buffer_size; sample++)
        {
            for (int channel = 0; channel < num_channels; channel++)
            {
                const float wetSample = wetBlock.getSample(channel, sample) * parameters.mix;
                const float drySample = dryBlock.getSample(channel, sample) * (1.0f - parameters.mix);
                wetBlock.setSample(channel, sample, wetSample + drySample);
            }
        }
    }

    DistortionParameters parameters;
    int num_channels;
    double sample_rate;
    int buffer_size;

    //Dry signal buffer
    juce::AudioBuffer<float> dryBuffer;

    //We filter out very low frequencies and very high frequencies before applying distortion
    using StereoFilter = juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
        juce::dsp::IIR::Coefficients<float>>;
    juce::dsp::ProcessorChain<StereoFilter, StereoFilter> filterChain;
    enum FilterChainIndex { HPF, LPF};

};
