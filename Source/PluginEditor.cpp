/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#define WIN_WIDTH       600
#define WIN_HEIGHT      400

//==============================================================================
SpatializerAudioProcessorEditor::SpatializerAudioProcessorEditor (SpatializerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    
    aziSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    aziSlider.setRange(-180.0f, 180.0f, 1.0f);
    aziSlider.setValue(0.0f);
    aziSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);
    aziSlider.addListener(this);
    addAndMakeVisible(aziSlider);
    
    eleSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    eleSlider.setRange(-40.0f, 90.0f, 5.0f);
    eleSlider.setValue(0.0f);
    eleSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);
    eleSlider.addListener(this);
    addAndMakeVisible(eleSlider);
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (WIN_WIDTH, WIN_HEIGHT);
}

SpatializerAudioProcessorEditor::~SpatializerAudioProcessorEditor()
{
}

//==============================================================================
void SpatializerAudioProcessorEditor::paint (juce::Graphics& g)
{
    
    g.fillAll(juce::Colours::lightslategrey);
    getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::darkorange);
    
    g.setFont(juce::Font("Avenir Next", 0, 0));
    g.setFont (68.0f);
    g.setColour (juce::Colours::orange);
    g.drawFittedText("8", 58, 25, getWidth(), 30, juce::Justification::centredTop, 1);
    g.setColour (juce::Colours::white);
    g.drawFittedText("ELEV", -14, 25, getWidth(), 30, juce::Justification::centredTop, 1);
    //    g.drawFittedText("ELEV8", 0, 25, getWidth(), 30, juce::Justification::centredTop, 1);
    g.setColour (juce::Colours::white);
    
    g.setFont(22.0f);
    g.drawFittedText("Azimuth", 2*getWidth()/3-30, getHeight()-80, 160, 20, juce::Justification::centredLeft, 1);
    g.drawFittedText("Elevation", getWidth()/3-110-15, getHeight()-80, 160, 20, juce::Justification::centredRight, 1);
    g.setFont(8.0f);
    g.drawFittedText("Designed and built by G. McWilliam", WIN_WIDTH-380, WIN_HEIGHT-20,
                     380, 20, juce::Justification::right, 1);
    
}

void SpatializerAudioProcessorEditor::resized()
{

    int aziSliderWidth = 220;
    int aziSliderHeight = 220;
    
    aziSlider.setBounds(2*getWidth()/3 - aziSliderWidth/2, getHeight()/2 - aziSliderHeight/2,
                        aziSliderWidth, aziSliderHeight);
    
    int eleSliderWidth = 100;
    int eleSliderHeight = 220;
    
    eleSlider.setBounds(getWidth()/3 - eleSliderWidth/2, getHeight()/2 - eleSliderHeight/2,
                        eleSliderWidth, eleSliderHeight);
    
}

void SpatializerAudioProcessorEditor::sliderValueChanged (juce::Slider *slider)
{
    if (slider == &aziSlider) {
        audioProcessor.azimuth = aziSlider.getValue();
    }
    
    if (slider == &eleSlider) {
        audioProcessor.elevation = eleSlider.getValue();
    }
    audioProcessor.updateParameters();
}
