/*
  ==============================================================================
    This file contains the basic framework code for a JUCE plugin processor.
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class ELEV8AudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    ELEV8AudioProcessor();
    ~ELEV8AudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    void process(juce::dsp::ProcessContextReplacing<float> context);
    void updateParameters();
    
//    int channel = 0;
    float azimuth {0.0};
    float elevation {0.0};
    
    juce::AudioParameterFloat *aziParam;
    juce::AudioParameterFloat *eleParam;
    juce::AudioParameterChoice *channelParam;
    
    juce::dsp::Convolution conv;
    
    juce::dsp::Convolution irL;
    juce::dsp::Convolution irR;
    
    juce::File folder;
    juce::String hrir;
//    std::string hrir;
    
    void updateHRIR();
    int HRTF_LEN = 128;

private:
    
    std::atomic<bool> shouldLoadImpulseResponse;
    juce::File fileToLoad;
//    std::string fileToLoad;
    const char *audioData;
    const char *hrirToLoad;
    
    int dataSize = 574;
    
    
    int elevAngles[14] = {0, 10, -10, 20, -20, 30, -30, 40, -40, 50, 60, 70, 80, 90};

    int aziAnglesMin40[58] = {0, 6, 13, 19, 26, 32, 39, 45, 51, 58, 64, 71, 77, 84, 90, 96, 103,
                        109, 116, 122, 129, 135, 141, 148, 154, 161, 167, 174, 180, -0, -6, -13, -19, -26, -32, -39, -45, -51, -58, -64, -71, -77, -84, -90, -96, -103, -109, -116, -122, -129, -135, -141, -148, -154, -161, -167, -174, -180};
    
    int aziAnglesMin20to20[74] = {0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 80,
                        85, 90, 95, 100, 105, 110, 115, 120, 125, 130, 135, 140, 145,
                        150, 155, 160, 165, 170, 175, 180, -0, -5, -10, -15, -20, -25, -30, -35, -40, -45, -50, -55, -60, -65, -70, -80, -85, -90, -95, -100, -105, -110, -115, -120, -125, -130, -135, -140, -145, -150, -155, -160, -165, -170, -175, -180};

    int aziAnglesMin30and30[62] = {0, 6, 12, 18, 24, 30, 36, 42, 48, 54, 60, 66, 72, 78, 84, 90,
                        96, 102, 108, 114, 120, 126, 132, 138, 144, 150, 156, 162, 168,
                        174, 180, -0, -6, -12, -18, -24, -30, -36, -42, -48, -54, -60, -66, -72, -78, -84, -90, -96, -102, -108, -114, -120, -126, -132, -138, -144, -150, -156, -162, -168, -174, -180};
    
    int aziAngles40[58] = {0, 6, 13, 19, 26, 32, 39, 45, 51, 58, 64, 71, 77, 84, 90, 96, 103, 109,                      116, 122, 129, 135, 141, 148, 154, 161, 167, 174, 180, -0, -6, -13, -19,                       -26, -32, -39, -45, -51, -58, -64, -71, -77, -84, -90, -96, -103, -109, -116,                    -122, -129, -135, -141, -148, -154, -161, -167, -174, -180};

    int aziAngles50[48] = {0, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120,
                        128, 136, 144, 152, 160, 168, 176, -0, -8, -16, -24, -32, -40, -48, -56, -64, -72, -80, -88, -96, -104, -112, -120, -128, -136, -144, -152, -160, -168, -176};

    int aziAngles60[40] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140,
                        150, 160, 170, 180, -0, -10, -20, -30, -40, -50, -60, -70, -80, -90, -100, -110, -120, -130, -140,
        150, 160, 170, 180};

    int aziAngles70[28] = {0, 15, 30, 45, 60, 75, 90, 105, 120, 135, 150, 165, 180, -0, -15, -30, -45,                  -60, -75, -90, -105, -120, -135, -150, -165, -180};

    int aziAngles80[14] = {30, 60, 90, 120, 150, 180, -30, -60, -90, -120, -150, -180};

    int aziAngles90[2] = {0, -0};
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ELEV8AudioProcessor)
};
