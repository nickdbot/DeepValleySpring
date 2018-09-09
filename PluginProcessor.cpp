/*
  ==============================================================================

  copyright 2018 to DEEP VALLEY AUDIO BIIIIIITCH

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DeepValleySpringAudioProcessor::DeepValleySpringAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
	//initHigh(&Hcoefs);
	//initLow(&Lcoefs);
	//unfortunately, this is in processBlock now, as certain values are needed that are only accessible there

}

DeepValleySpringAudioProcessor::~DeepValleySpringAudioProcessor()
{
	//delete[] Hcoefs.DELAYLHIGH;
	//delete[] Hcoefs.inputBuffer;
	//delete[] Hcoefs.outputBuffer;
	//delete[] Lcoefs.DELAYLINEBASE;
	//delete[] Lcoefs.inputBuffer;
	//delete[] Lcoefs.outputBuffer;
}

//==============================================================================
const String DeepValleySpringAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DeepValleySpringAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DeepValleySpringAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DeepValleySpringAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DeepValleySpringAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DeepValleySpringAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DeepValleySpringAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DeepValleySpringAudioProcessor::setCurrentProgram (int index)
{
}

const String DeepValleySpringAudioProcessor::getProgramName (int index)
{
    return {};
}

void DeepValleySpringAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void DeepValleySpringAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void DeepValleySpringAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DeepValleySpringAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void DeepValleySpringAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

	//PROCESS:
	//First, LOutPtr = processLow(input block) <- MAKE SURE BOTH OF THESE DO NOT FUCK WITH INPUT SAMPLES
	//Second, HOutPtr = processHigh(input block)
	//Add untouched input samples with LOutPtr and HOutPtr <- these pointers point to the outputBuffer memory allocated in the init functions

	if (INIT_STATUS) { 
		initHigh(&Hcoefs);
		initLow(&Lcoefs);

		INIT_STATUS = false;
	}

	int blockInd;
	auto numSamples = buffer.getNumSamples();

	int channel = 0;
	auto* channelData1 = buffer.getWritePointer(channel); 
	auto* channelData2 = buffer.getWritePointer(channel + 1);
	
	/*
	for (blockInd = 0; blockInd < buffer.getNumSamples(); blockInd++) {
		SourceData_Ptr[blockInd] = (float)(channelData1[blockInd] + channelData2[blockInd]) / 2;
	}
	*/

	// HERE'S WHAT WE'RE GONNA DO: just use channelData1 (left channel) as the SourceData_Ptr because it's easier! fuck off

	float* LOutPtr = processLow(channelData1);
	float* HOutPtr = processHigh(channelData1);
	

	for (blockInd = 0; blockInd < numSamples; blockInd++) {
		channelData1[blockInd] = channelData1[blockInd] * GDRY + (HOutPtr[blockInd] * Hcoefs.GHIGH + LOutPtr[blockInd] * Lcoefs.GLOW);
		channelData2[blockInd] = channelData2[blockInd] * GDRY + (HOutPtr[blockInd] * Hcoefs.GHIGH + LOutPtr[blockInd] * Lcoefs.GLOW);
	}

}

float* DeepValleySpringAudioProcessor::processHigh(float* SourceData_Ptr) {	
	
	float inVal, inVal2, Mh, frach; //all for modulated delay section
	int Ih, blockInd, n; //Ih: for modulated delay, blockInd: index for sample frame, n: index for APFs


	L6SPB_High_Frequency_Spring_Data *coefs = &Hcoefs;

	int    NumSamples = getBlockSize();		
	float* DestData_Ptr = coefs->outputBuffer;
	
	if (coefs->SAMPLES_ELAPSED < MAX_DELAY_SAMPLES_HIGH) {

		for (blockInd = 0; blockInd < NumSamples; blockInd++) {
			DestData_Ptr[blockInd] = 0.0;
			coefs->DELAYLHIGH[coefs->SAMPLES_ELAPSED] = 0.0;
			coefs->SAMPLES_ELAPSED++;
		}
	}

	//process audio normally
	else
	{
		for (blockInd = 0; blockInd < NumSamples; blockInd++) {

			//coefs->CHOUT = allpassHighS(coefs->APFHIN,spring);
			coefs->OUTHIGHAPF[0] = coefs->APFHIN * coefs->AHIGH + coefs->ZHIGH[0];
			coefs->ZHIGH[0] = coefs->APFHIN - coefs->OUTHIGHAPF[0] * coefs->AHIGH;

			for (n = 1; n < N_ALLPASS_HIGH; n++) {
				coefs->OUTHIGHAPF[n] = coefs->OUTHIGHAPF[n - 1] * coefs->AHIGH + coefs->ZHIGH[n];
				coefs->ZHIGH[n] = coefs->OUTHIGHAPF[n - 1] - coefs->OUTHIGHAPF[n] * coefs->AHIGH;
			}
			DestData_Ptr[blockInd] = coefs->OUTHIGHAPF[N_ALLPASS_HIGH - 1];
			//-----------------------------------------------------------------------

			//coefs->DLHOUT = coefs->GHF * delayLineHighS(coefs->CHOUT,spring);
			if (coefs->DLYLHIGHINDEX >= coefs->LHIGH) {
				coefs->DLYLHIGHINDEX = 0;
			}

			Mh = coefs->GMODHIGH * (float)rand() / RAND_MAX;
			Mh = coefs->ALPFHIGH * Mh + (1 - coefs->ALPFHIGH) * coefs->LPFPREVVALH;
			coefs->LPFPREVVALH = Mh;

			Ih = floor(Mh);
			frach = Mh - Ih;

			if (coefs->DLYLHIGHINDEX + Ih >= coefs->LHIGH) {
				inVal = coefs->DELAYLHIGH[coefs->DLYLHIGHINDEX + Ih - coefs->LHIGH];
			}
			else {
				inVal = coefs->DELAYLHIGH[coefs->DLYLHIGHINDEX + Ih];
			}
			if (coefs->DLYLHIGHINDEX + Ih + 1 >= coefs->LHIGH) {
				inVal2 = coefs->DELAYLHIGH[coefs->DLYLHIGHINDEX + Ih + 1 - coefs->LHIGH];
			}
			else {
				inVal2 = coefs->DELAYLHIGH[coefs->DLYLHIGHINDEX + Ih + 1];
			}

			coefs->DLHOUT = coefs->GHF * (frach * inVal + (1 - frach) * inVal2);
			coefs->DELAYLHIGH[coefs->DLYLHIGHINDEX] = coefs->CHOUT;

			coefs->DLYLHIGHINDEX++;
			//-----------------------------------------------------------------------

			coefs->APFHIN = SourceData_Ptr[blockInd] - coefs->DLHOUT;
		
		}
	} 
	return DestData_Ptr;
}

float* DeepValleySpringAudioProcessor::processLow(float* SourceData_Ptr) {

	float M, frac, inVal, inVal2, L0Output, LrippleIn, dcInVal;
	int   I, blockInd, n;

	int    NumSamples = getBlockSize(); //TODO: READ HERE!!! YOU NEED TO FIGURE OUT HOW/WHERE TO INITIALIZE MEMORY FOR OUTPUT BUFFER TO RETURN TO processBlock
	L6SPB_Low_Frequency_Spring_Data *coefs = &Lcoefs;
	float* DestData_Ptr = coefs->outputBuffer;

	if (coefs->SAMPLES_ELAPSED < MAX_DELAY_SAMPLES_LOW) {

		for (blockInd = 0; blockInd < NumSamples; blockInd++) {
			DestData_Ptr[blockInd] = 0.0;
			coefs->DELAYLINEBASE[coefs->SAMPLES_ELAPSED] = 0.0;
			coefs->SAMPLES_ELAPSED++;
		}
	}

	else {
		for (blockInd = 0; blockInd < NumSamples; blockInd++) {
			n = 0;
			DestData_Ptr[blockInd] = coefs->CLOUT * coefs->GLOW;

			//coefs->CLOUT = HlpLowS(coefs->HEQOUT);
			coefs->CLOUT = coefs->ALP * coefs->HEQOUT + (1 - coefs->ALP) * coefs->PREVVALLP;
			coefs->PREVVALLP = coefs->CLOUT;
			//-----------------------------------------------------------------------

			//coefs->HEQOUT = HeqLowS(coefs->APFLOUT);
			if (coefs->DELAYINDEX >= coefs->KEQ) coefs->DELAYINDEX = 0;

			coefs->HEQOUT = coefs->A0 * coefs->APFLOUT + coefs->DELAY1[coefs->DELAYINDEX];
			coefs->DELAY1[coefs->DELAYINDEX] = coefs->DELAY2[coefs->DELAYINDEX] - coefs->AEQ1*coefs->HEQOUT;
			coefs->DELAY2[coefs->DELAYINDEX] = -coefs->AEQ2 * coefs->HEQOUT - coefs->A0 * coefs->APFLOUT;

			coefs->DELAYINDEX++;
			//-----------------------------------------------------------------------

			//coefs->APFLOUT = allpassLowS(coefs->HDCOUT);
			if (coefs->DLYAPF >= (coefs->K1 + 1)) {
				coefs->DLYAPF = 0;
			}

			coefs->V[n] = coefs->Z2[n] + (coefs->Z1[n] * coefs->A2);
			coefs->Z2[n] = coefs->Z1[n];
			coefs->Z1[n] = coefs->ZK1[n*(coefs->K1 + 1) + coefs->DLYAPF] - (coefs->Z1[n] * coefs->A2);
			coefs->ZK1[n*(coefs->K1 + 1) + coefs->DLYAPF] = coefs->HDCOUT - (coefs->A1 * coefs->V[n]);

			for (n = 1; n < N_ALLPASS_LOW; n++) {
				coefs->V[n] = coefs->Z2[n] + (coefs->Z1[n] * coefs->A2);
				coefs->Z2[n] = coefs->Z1[n];
				coefs->Z1[n] = coefs->ZK1[n*(coefs->K1 + 1) + coefs->DLYAPF] - (coefs->Z1[n] * coefs->A2);
				coefs->ZK1[n*(coefs->K1 + 1) + coefs->DLYAPF] = coefs->A1 * (coefs->ZK1[(n - 1)*(coefs->K1 + 1) + coefs->DLYAPF] - coefs->V[n]) + coefs->V[n - 1];
			}

			coefs->APFLOUT = coefs->A1 * coefs->ZK1[(n - 1)*(coefs->K1 + 1) + coefs->DLYAPF] + coefs->V[n - 1]; //n-1 because the for loop ends with n = 80, but max index is 79

			coefs->DLYAPF++;
			//-----------------------------------------------------------------------

			//coefs->MTDLLout = coefs->GLF * MTDLLowS(coefs->APFLOUT);

			// check circular buffer indices within bounds
			if (coefs->DELAYLINEIND >= coefs->L) { 	// DLYECHOINDEX and DLYRIPPLEINDEX are defined w.r.t. DELAYLINEIND 
				coefs->DELAYLINEIND = 0; 			// by subtracting (L0) and (L0+Lecho) respectively. DLI will always
			}										// be "first" and incremented up (so check for exceeding L) and 
			if (coefs->DLYLECHOINDEX < 0) {			// DEI/DRI will always be subtracted from DLI, so check they do not
				coefs->DLYLECHOINDEX += coefs->L;	// drop below 0.
			}
			if (coefs->DLYLRIPPLEINDEX < 0) {
				coefs->DLYLRIPPLEINDEX += coefs->L;
			}
			// modulate L0 segment of delay line
			M = coefs->GMOD * (float)rand() / RAND_MAX;
			M = coefs->ALPF * M + (1 - coefs->ALPF) * coefs->LPFPREVVAL;
			coefs->LPFPREVVAL = M;

			I = floor(M);
			frac = M - I;

			if (coefs->DELAYLINEIND + I >= coefs->L0) {
				inVal = coefs->DELAYLINEBASE[coefs->DELAYLINEIND + I - coefs->L0];
			}
			else {
				inVal = coefs->DELAYLINEBASE[coefs->DELAYLINEIND + I];
			}
			if (coefs->DELAYLINEIND + I + 1 >= coefs->L0) {
				inVal2 = coefs->DELAYLINEBASE[coefs->DELAYLINEIND + I + 1 - coefs->L0];
			}
			else {
				inVal2 = coefs->DELAYLINEBASE[coefs->DELAYLINEIND + I + 1];
			}

			L0Output = frac * inVal + (1 - frac) * inVal2;

			// perform Lecho and Lripple modifications using clever indexing terms

			//Define LrippleIn = L0Out * gecho + LrippleIn
			LrippleIn = coefs->DELAYLINEBASE[coefs->DLYLECHOINDEX] + L0Output * coefs->GECHO;
			//Release LrippleOut = GLF * (LrippleOut + LrippleIn * gripple)
			coefs->MTDLOUT = coefs->GRIPPLE * LrippleIn + coefs->DELAYLINEBASE[coefs->DLYLRIPPLEINDEX];
			coefs->MTDLOUT *= coefs->GLF;
			//Define LrippleOut = input
			coefs->DELAYLINEBASE[coefs->DELAYLINEIND] = coefs->APFLOUT;

			coefs->DELAYLINEIND++;
			coefs->DLYLECHOINDEX = coefs->DELAYLINEIND - coefs->L0;
			coefs->DLYLRIPPLEINDEX = coefs->DELAYLINEIND - (coefs->L0 + coefs->LECHO);
			//-----------------------------------------------------------------------

			//coefs->HDCOUT = HdcLow(input[blockInd] - coefs->MTDLOUT)
			dcInVal = SourceData_Ptr[blockInd] - coefs->MTDLOUT;
			coefs->HDCOUT = dcInVal - coefs->PREVINVAL + coefs->RDC * coefs->PREVOUTVAL;
			//Out = in - zin + RDC*zout
			coefs->PREVINVAL = dcInVal; //redefine filter states
			coefs->PREVOUTVAL = coefs->HDCOUT;
		}
	}
	return DestData_Ptr;
}

void DeepValleySpringAudioProcessor::initHigh(L6SPB_High_Frequency_Spring_Data* coefs) {
	float FS = getSampleRate(); //TODO: figure out a better way to get the sample rate
	int numSamples = getBlockSize(); //TODO: make sure getBlockSize returns the value you want! may have to be divided by 2
	//ACTUALLY don't do this at all. This value is only used for the input/output buffers, which are now allocated to the stack within the processX functions

	memset(coefs, 0, sizeof(L6SPB_High_Frequency_Spring_Data));

	//initialize spring values
	int FCNOISE = 100;

	//allpassHighS
	coefs->AHIGH = -0.6;

	//DelayLineHighS
	coefs->DELAYTIMEHIGH = 0.056 / 2.3; //seconds
	coefs->LHIGH = round(FS*coefs->DELAYTIMEHIGH);
	coefs->DELAYLHIGH = highDelay;
	coefs->DLYLHIGHINDEX = 0;
	//for noise/LPF
	coefs->ALPFHIGH = (2 * 3.14159*FCNOISE / FS) / (2 * 3.14159*FCNOISE / FS + 1);
	coefs->LPFPREVVALH = 0.;
	coefs->GMODHIGH = 12;

	//structural states
	coefs->GHIGH = 0.001;
	coefs->GHF = 0.7;
	coefs->CHOUT = 0.;
	coefs->APFHIN = 0.;
	coefs->DLHOUT = 0.;

	coefs->SAMPLES_ELAPSED = 0;

	//coefs->inputBuffer = new float(numSamples);
	//coefs->outputBuffer = new float(numSamples);
}


void DeepValleySpringAudioProcessor::initLow(L6SPB_Low_Frequency_Spring_Data* coefs) {
	float FS = getSampleRate(); //TODO: figure out a better way to get the sample rate
	
	int numSamples = getBlockSize(); //TODO: make sure getBlockSize returns the value you want! may have to be divided by 2
	//ACTUALLY don't do this at all. This value is only used for the input/output buffers, which are now allocated to the stack within the processX functions

	memset(coefs, 0, sizeof(L6SPB_Low_Frequency_Spring_Data));

	//initialize spring values
	int FCNOISE = 100;
	//allpassLowS
	coefs->FCCHIRP = 3500;
	coefs->K = FS / (2 * coefs->FCCHIRP);
	coefs->K1 = roundf(coefs->K) - 1;
	coefs->D = coefs->K - coefs->K1;
	coefs->DLYAPF = 0;
	coefs->A1 = 0.75;
	coefs->A2 = (1 - coefs->D) / (1 + coefs->D);


	//HdcLowS
	coefs->RDC = 0.95;
	coefs->PREVOUTVAL = 0.;
	coefs->PREVINVAL = 0.;

	//HeqLowS
	coefs->KEQ = floor(coefs->K);
	coefs->B = 130;
	coefs->FPEAK = 95;
	coefs->DELAYINDEX = 0;
	coefs->R = 1 - (3.14159 * coefs->B * coefs->KEQ / FS);
	coefs->POLEANGLE = (1 + (coefs->R*coefs->R)) / (2 * coefs->R)*cos(2 * 3.14159*coefs->FPEAK*coefs->KEQ / FS);
	coefs->AEQ1 = -2 * coefs->R*(coefs->POLEANGLE);
	coefs->AEQ2 = coefs->R*coefs->R;
	coefs->A0 = (1 - coefs->AEQ2) / 2;

	//HlpLowS
	coefs->FC = 6000;
	coefs->ALP = (2 * 3.14159*coefs->FC / FS) / (2 * 3.14159*coefs->FC / FS + 1);
	coefs->PREVVALLP = 0.;

	//MTDLLowS
	coefs->NRIPPLE = 0.5;
	coefs->ECHOTIME = 0.056;
	coefs->L = round(FS * coefs->ECHOTIME - coefs->K * N_ALLPASS_LOW * (1 - coefs->A1) / (1 + coefs->A1));
	coefs->LRIPPLE = round(2 * coefs->K*(float)coefs->NRIPPLE);
	coefs->LECHO = round(coefs->L / 5);
	coefs->L0 = coefs->L - coefs->LRIPPLE - coefs->LECHO;
	coefs->GECHO = 0.1;
	coefs->GRIPPLE = -0.25;
	coefs->DELAYLINEBASE = lowDelay;
	coefs->DELAYLINEIND = 0;
	//coefs->DLYLECHOINDEX = 0;
	//coefs->DLYLRIPPLEINDEX = 0;
	//for noise/LPF
	coefs->ALPF = (2 * 3.14159*FCNOISE / FS) / (2 * 3.14159*FCNOISE / FS + 1);
	coefs->LPFPREVVAL = 0.;
	coefs->GMOD = 12;


	//structural states
	coefs->GLOW = 1.;
	coefs->GLF = -0.7;
	coefs->CLOUT = 0.;
	coefs->APFLOUT = 0.;
	coefs->HEQOUT = 0.;
	coefs->HDCOUT = 0.;
	coefs->MTDLOUT = 0.;

	//coefs->inputBuffer = new float(numSamples);
	//coefs->outputBuffer = new float(numSamples);
}

void DeepValleySpringAudioProcessor::L6SPB_High_Frequency_Spring_PChg(int paramID, float value)
{
	L6SPB_High_Frequency_Spring_Data *coefs = &Hcoefs;
	float FS = getSampleRate(); // TODO: figure it out

	switch (paramID)
	{

		//  Sets delay time 
	case 0://SPRING_SETTD:
		coefs->DELAYTIMEHIGH = value;
		coefs->LHIGH = round(FS*coefs->DELAYTIMEHIGH / 2.3);
		break;

		//  Sets high frequency delay feedback gain
	case 1://SPRING_SETGHF:
		coefs->GHF = value;
		break;

		//  Sets high frequency section gain
	case 2://SPRING_SETGHIGH:
		coefs->GHIGH = value;
		break;

		//  Sets modulation intensity
	case 3://SPRING_SETGMOD:
		coefs->GMODHIGH = value;
		break;

	default:
		//L6SPB_ASSERT_INVALID_PARAM();
		break;
	}
}

void DeepValleySpringAudioProcessor::L6SPB_Low_Frequency_Spring_PChg(int paramID, float value)
{
	int val = (int)value;
	float FS = getSampleRate(); //TODO: figure it out

	L6SPB_Low_Frequency_Spring_Data *coefs = &Lcoefs;

	switch (paramID)
	{
		//  Sets FCCHIRP for the all-pass filters, then falls through to change dependent params
	case 0://SPRING_SETFCCHIRP:
		coefs->FCCHIRP = val;
		coefs->K = FS / (2 * coefs->FCCHIRP);
		coefs->K1 = roundf(coefs->K) - 1;
		coefs->D = coefs->K - coefs->K1;
		coefs->A2 = (1 - coefs->D) / (1 + coefs->D);

		//  adjust lowpass EQ params
		coefs->KEQ = floor(coefs->K);
		coefs->R = 1 - (3.1415*coefs->B*coefs->KEQ / FS);
		coefs->POLEANGLE = (1 + coefs->R*coefs->R) / (2 * coefs->R)*cos(2 * 3.1415*coefs->FPEAK*coefs->KEQ / FS);
		coefs->AEQ1 = -2 * coefs->R*(coefs->POLEANGLE);
		coefs->AEQ2 = coefs->R * coefs->R;
		coefs->A0 = (1 - coefs->AEQ2) / 2;

		//  fall through to change NRIPPLE and delay times
		value = coefs->NRIPPLE;

		//  Sets number of ripples in LF delay feedback ripple filter
	case 1://SPRING_SETNRIPPLE:
		coefs->NRIPPLE = value;
		value = coefs->ECHOTIME;
		//fall through and change delay times

		//  Sets delay time 
	case 2://SPRING_SETTD:
		coefs->ECHOTIME = value;
		coefs->L = round(FS*coefs->ECHOTIME - coefs->K * N_ALLPASS_LOW * (1 - coefs->A1) / (1 + coefs->A1));
		coefs->LRIPPLE = round(2 * coefs->K * (float)coefs->NRIPPLE);
		coefs->LECHO = round(coefs->L / 5);
		coefs->L0 = coefs->L - coefs->LRIPPLE - coefs->LECHO;
		break;

		// Set the low frequency delay feedback gain        
	case 3://SPRING_SETGLF:
		coefs->GLF = -value;
		break;

		//  Sets low frequency section gain
	case 4://SPRING_SETGLOW:
		coefs->GLOW = value;
		break;

		//  Sets modulation intensity
	case 5://SPRING_SETGMOD:
		coefs->GMOD = value;
		break;

		//  Sets gain of ripple filter feedback
	case 6://SPRING_SETGRIPPLE:
		coefs->GRIPPLE = value;
		break;

		//  Sets echo feedback gain
	case 7://SPRING_SETGECHO:
		coefs->GECHO = value;
		break;

	default:
		//L6SPB_ASSERT_INVALID_PARAM();
		break;
	} 
}

//==============================================================================
bool DeepValleySpringAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* DeepValleySpringAudioProcessor::createEditor()
{
    return new DeepValleySpringAudioProcessorEditor (*this);
}

//==============================================================================
void DeepValleySpringAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DeepValleySpringAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DeepValleySpringAudioProcessor();
}
