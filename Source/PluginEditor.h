#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class ELEV8AudioProcessorEditor : public juce::AudioProcessorEditor,
                                  public juce::Slider::Listener
{
public:
    ELEV8AudioProcessorEditor (ELEV8AudioProcessor&);
    ~ELEV8AudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged (juce::Slider* slider) override;

private:
    juce::Slider aziSlider;
    juce::Slider eleSlider;

    ELEV8AudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ELEV8AudioProcessorEditor)
};
