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
class ELEV8AudioProcessorEditor  : public juce::AudioProcessorEditor,
                                         public juce::Slider::Listener
{
public:
    ELEV8AudioProcessorEditor (ELEV8AudioProcessor&);
    ~ELEV8AudioProcessorEditor() override;

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
    ELEV8AudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ELEV8AudioProcessorEditor)
};
