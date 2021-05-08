/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class SpatializerAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                         public juce::Slider::Listener
{
public:
    SpatializerAudioProcessorEditor (SpatializerAudioProcessor&);
    ~SpatializerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void sliderValueChanged(juce::Slider *slider) override;

private:
    
    juce::Slider aziSlider;
    juce::Slider eleSlider;
    
    // Uncomment if user is to select HRTF folder from plug-in
//    juce::ComboBox selectChannel;
//    juce::TextButton selectFolder;
//    juce::TextEditor labelFolder;
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SpatializerAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpatializerAudioProcessorEditor)
};
