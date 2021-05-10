/*
  ==============================================================================
    This file contains the basic framework code for a JUCE plugin processor.
  ==============================================================================
*/

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
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
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
    
    // Declare DSP process specifications
    juce::dsp::ProcessSpec spec;
    
    // Initialize file parameters
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    // Prepare specifications
    conv.prepare(spec);
    
    reset();
    
}

void ELEV8AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ELEV8AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
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
    int curAziAngle = 0;
    int curMin = 90;
    
    // Find closest valid elevation angle to user-selected value
    for (int i = 0; i < 14; i++) {
        if (abs(elevation - elevAngles[i]) < curMin) {
            curElevAngle = elevAngles[i];
            curMin = abs(elevation - curElevAngle);
        }
    }

    int *aziAngleSelection;
    int selSize = 0;

    // Depending on elevation angle, determine azimuth file parameters
    switch(curElevAngle) {

        case -20:
            aziAngleSelection = aziAnglesMin20to20;
            selSize = 74;
            break;
        case -10:
            aziAngleSelection = aziAnglesMin20to20;
            selSize = 74;
            break;
        case 0:
            aziAngleSelection = aziAnglesMin20to20;
            selSize = 74;
            break;
        case 10:
            aziAngleSelection = aziAnglesMin20to20;
            selSize = 74;
            break;
        case 20:
            aziAngleSelection = aziAnglesMin20to20;
            selSize = 74;
            break;
        case  -30:
            aziAngleSelection = aziAnglesMin30and30;
            selSize = 62;
            break;
        case 30:
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
            selSize = 40;
            break;
        case 70:
            aziAngleSelection = aziAngles70;
            selSize = 28;
            break;
        case 80:
            aziAngleSelection = aziAngles80;
            selSize = 14;
            break;
        case 90:
            aziAngleSelection = aziAngles90;
            selSize = 2;
            break;

        default:
            aziAngleSelection = aziAngles90;
    }
    
    // Reset current minimum to 90 degrees for calculation of closest azimuth angle
    curMin = 90;

    // Find closest valid azimuth angle to user-selected value
    for (int i = 0; i < selSize; i++) {
        if (abs(azimuth - aziAngleSelection[i]) < curMin) {
            curAziAngle = aziAngleSelection[i];
            curMin = abs(azimuth - curAziAngle);
        }
    }

    // Prepare for casting to string
    std::string aziAngleStr = "";

    // Remove negative azimuth values and add zeros as appropriate
    // to ensure filename is correct
    if (curAziAngle <= -100) {
        aziAngleStr = "-" + std::to_string(abs(curAziAngle));
    }
    else if (curAziAngle <= -10) {
        aziAngleStr = "-0" + std::to_string(abs(curAziAngle));
    }
    else if (curAziAngle < 0) {
        aziAngleStr = "-00" + std::to_string(abs(curAziAngle));
    }
    else if (curAziAngle < 10) {
        aziAngleStr = "00" + std::to_string(curAziAngle);
    }
    else if (curAziAngle < 100) {
        aziAngleStr = "0" + std::to_string(curAziAngle);
    }
    else {
        aziAngleStr = std::to_string(curAziAngle);
    }
    
    
    hrir = "H" + std::to_string(curElevAngle) + "e" + aziAngleStr + "a.wav";
    
    // Iterate through all files in binary data header
    for (int i = 0; i < BinaryData::namedResourceListSize; i++) {
        
        // Retrieve binary name of each file
        auto binaryName = BinaryData::namedResourceList[i];
        
        // Convert binary name to type juce::String name
        juce::String fileName = BinaryData::getNamedResourceOriginalFilename(binaryName);
        
        // If file name matches hrir above, correct file has been found
        if (fileName == hrir) {
            audioData = BinaryData::getNamedResource(binaryName, dataSize);
            break;
        }
    }
    
    // Check the file exists
    jassert(audioData != nullptr);
    
    // Set flag to true, allowing the impulse response to be loaded
    shouldLoadImpulseResponse = true;
    
}

void ELEV8AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    
    // If required, load impulse response into convolution object
    if (shouldLoadImpulseResponse) {
//        conv.loadImpulseResponse(fileToLoad, juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::no, 0);
        conv.loadImpulseResponse(audioData, 574, juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::no, 0);
        shouldLoadImpulseResponse = false;
        
    }
    
    // Declare variables
    juce::ScopedNoDenormals noDenormals;
    
    buffer.applyGain(10.0);
    
    // Copy left channel into right channel
    buffer.copyFrom(1, 0, buffer, 0, 0, buffer.getNumSamples());
    
    // Declare audio block
    juce::dsp::AudioBlock<float> block(buffer);
    
    // Create process context
    juce::dsp::ProcessContextReplacing<float> ctx(block);
    
    // Perform frequency-domain convolution
    conv.process(ctx);
    
    /*
     TO-DO:
     
     - Control initial gain
     
     */
    
}

//==============================================================================
bool ELEV8AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ELEV8AudioProcessor::createEditor()
{
    return new ELEV8AudioProcessorEditor (*this);
}

//==============================================================================
void ELEV8AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ELEV8AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ELEV8AudioProcessor();
}
