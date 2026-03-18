#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ELEV8AudioProcessor::ELEV8AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::mono(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

ELEV8AudioProcessor::~ELEV8AudioProcessor()
{
}

//==============================================================================
const juce::String ELEV8AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ELEV8AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ELEV8AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ELEV8AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ELEV8AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ELEV8AudioProcessor::getNumPrograms()
{
    // Some hosts don't cope well if you tell them there are 0 programs,
    // so this should be at least 1 even if programs are not implemented.
    return 1;
}

int ELEV8AudioProcessor::getCurrentProgram()
{
    return 0;
}

void ELEV8AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ELEV8AudioProcessor::getProgramName (int index)
{
    return {};
}

void ELEV8AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ELEV8AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate       = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels      = getTotalNumOutputChannels();

    conv.prepare(spec);
    reset();
}

void ELEV8AudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ELEV8AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (juce::AudioChannelSet::mono() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ELEV8AudioProcessor::updateParameters()
{
    int curElevAngle = 0;
    int curAziAngle  = 0;
    int curMin       = 90;

    // Find the closest valid elevation angle to the user-selected value
    for (int i = 0; i < 14; i++) {
        if (abs(elevation - elevAngles[i]) < curMin) {
            curElevAngle = elevAngles[i];
            curMin       = abs(elevation - curElevAngle);
        }
    }

    int* aziAngleSelection = nullptr;
    int  selSize           = 0;

    switch (curElevAngle) {
        case -20: case -10: case 0: case 10: case 20:
            aziAngleSelection = aziAnglesMin20to20;
            selSize = 74;
            break;
        case -30: case 30:
            aziAngleSelection = aziAnglesMin30and30;
            selSize = 62;
            break;
        case 40:
            aziAngleSelection = aziAngles40;
            selSize = 58;
            break;
        case -40:
            aziAngleSelection = aziAnglesMin40;
            selSize = 58;
            break;
        case 50:
            aziAngleSelection = aziAngles50;
            selSize = 48;
            break;
        case 60:
            aziAngleSelection = aziAngles60;
            selSize = 38;
            break;
        case 70:
            aziAngleSelection = aziAngles70;
            selSize = 26;
            break;
        case 80:
            aziAngleSelection = aziAngles80;
            selSize = 14;
            break;
        default:
            aziAngleSelection = aziAngles90;
            selSize = 2;
            break;
    }

    curMin = 90;

    // Find the closest valid azimuth angle to the user-selected value
    for (int i = 0; i < selSize; i++) {
        if (abs(azimuth - aziAngleSelection[i]) < curMin) {
            curAziAngle = aziAngleSelection[i];
            curMin      = abs(azimuth - curAziAngle);
        }
    }

    // Build zero-padded azimuth string to match HRTF filename format
    std::string aziAngleStr;
    if      (curAziAngle <= -100) aziAngleStr = "-"   + std::to_string(abs(curAziAngle));
    else if (curAziAngle <=  -10) aziAngleStr = "-0"  + std::to_string(abs(curAziAngle));
    else if (curAziAngle <     0) aziAngleStr = "-00" + std::to_string(abs(curAziAngle));
    else if (curAziAngle <    10) aziAngleStr = "00"  + std::to_string(curAziAngle);
    else if (curAziAngle <   100) aziAngleStr = "0"   + std::to_string(curAziAngle);
    else                          aziAngleStr =         std::to_string(curAziAngle);

    targetFile = "H" + std::to_string(curElevAngle) + "e" + aziAngleStr + "a.wav";

    for (int i = 0; i < BinaryData::namedResourceListSize; i++) {
        auto binaryName = BinaryData::namedResourceList[i];
        juce::String fileName = BinaryData::getNamedResourceOriginalFilename(binaryName);

        if (fileName == targetFile) {
            audioData = BinaryData::getNamedResource(binaryName, dataSize);
            break;
        }
    }

    jassert(audioData != nullptr);
    shouldLoadImpulseResponse = true;
}

void ELEV8AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    if (shouldLoadImpulseResponse) {
        conv.loadImpulseResponse(audioData.load(), dataSize,
                                 juce::dsp::Convolution::Stereo::yes,
                                 juce::dsp::Convolution::Trim::no, 0);
        shouldLoadImpulseResponse = false;
    }

    juce::ScopedNoDenormals noDenormals;

    buffer.applyGain(10.0);
    buffer.copyFrom(1, 0, buffer, 0, 0, buffer.getNumSamples());

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> ctx(block);
    conv.process(ctx);
}

//==============================================================================
bool ELEV8AudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* ELEV8AudioProcessor::createEditor()
{
    return new ELEV8AudioProcessorEditor (*this);
}

//==============================================================================
void ELEV8AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
}

void ELEV8AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ELEV8AudioProcessor();
}
