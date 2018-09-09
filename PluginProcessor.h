/*
  ==============================================================================

  copyright 2018 to DEEP VALLEY AUDIO BIIIIIITCH

  ==============================================================================
*/

#define N_ALLPASS_HIGH 			80
#define MAX_DELAY_SAMPLES_HIGH	1536
#define N_ALLPASS_LOW			100
#define MAX_K 					(float)48000./(2*2500.0)	//48000 max FS, 3000 min FC
#define MAX_K1					(int)9						//round(48000./(2*2500.)) - 1	
#define MAX_KEQ					(int)9						//floor(48000./(2*2500.))
#define MAX_DELAY_SAMPLES_LOW	3072

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/**
*/
class DeepValleySpringAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    DeepValleySpringAudioProcessor();
    ~DeepValleySpringAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;
	float* processHigh(float* SourceData_Ptr);
	float* processLow(float* SourceData_Ptr);
	float GDRY = 1.0;
	bool INIT_STATUS = true;
	float highDelay[MAX_DELAY_SAMPLES_HIGH];
	float lowDelay[MAX_DELAY_SAMPLES_LOW];

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

	void L6SPB_High_Frequency_Spring_PChg(int paramID, float value);
	void L6SPB_Low_Frequency_Spring_PChg(int paramID, float value);

    //==============================================================================
	typedef struct
	{
		//for allpassHighS
		float 		AHIGH;
		float 		ZHIGH[N_ALLPASS_HIGH];
		float 		OUTHIGHAPF[N_ALLPASS_HIGH];

		//for delayLineHighS
		int 		LHIGH;
		int 		DLYLHIGHINDEX;
		float		DELAYTIMEHIGH;
		float* 		DELAYLHIGH;
		//for noise/LPF
		float		GMODHIGH;
		float		ALPFHIGH;
		float 		LPFPREVVALH;

		//structural states
		float 		GHIGH;
		float 		GHF;
		float 		CHOUT;
		float 		APFHIN;
		float 		DLHOUT;

		int			SAMPLES_ELAPSED;
		//float		inputBuffer[480];
		float		outputBuffer[480];
	} L6SPB_High_Frequency_Spring_Data;

	typedef struct {

		//for allpassLowS
		int 		FCCHIRP;
		int 		K1;
		int 		DLYAPF;
		float 		K;
		float 		D;
		float 		A1;
		float 		A2;
		float 		Z1[N_ALLPASS_LOW];
		float 		Z2[N_ALLPASS_LOW];
		float 		ZK1[N_ALLPASS_LOW * (MAX_K1 + 1)]; //for nested APF, calc max possible size
		float 		V[N_ALLPASS_LOW];

		//for HdcLowS
		float 		RDC;
		float 		PREVOUTVAL;
		float 		PREVINVAL;

		//for HeqLowS
		int 		KEQ;
		int 		B;
		int 		FPEAK;
		int 		DELAYINDEX;
		float 		R;
		float 		POLEANGLE;
		float 		AEQ1;
		float 		AEQ2;
		float 		A0;
		float 		DELAY1[MAX_KEQ];
		float 		DELAY2[MAX_KEQ];

		//for HlpLowS
		int 		FC;
		float 		ALP;
		float 		PREVVALLP;

		//for delayLineLowS
		int 		L0;
		int 		LRIPPLE;
		int 		LECHO;
		int 		L;
		int 		DELAYLINEIND;
		int 		DLYLECHOINDEX;
		int 		DLYLRIPPLEINDEX;
		float 		NRIPPLE;
		float 		ECHOTIME;
		float 		GECHO;
		float 		GRIPPLE;
		float* 		DELAYLINEBASE;
		//for noise/LPF
		float		GMOD;
		float 		ALPF;
		float 		LPFPREVVAL;

		//structural states
		float 		GLOW;
		float 		GLF;
		float 		CLOUT;
		float 		APFLOUT;
		float 		HEQOUT;
		float 		HDCOUT;
		float 		MTDLOUT;

		int 		SAMPLES_ELAPSED;
		//float		inputBuffer[480];
		float		outputBuffer[480];
	} L6SPB_Low_Frequency_Spring_Data;

	L6SPB_Low_Frequency_Spring_Data Lcoefs;
	L6SPB_High_Frequency_Spring_Data Hcoefs;

private:

	void initLow(L6SPB_Low_Frequency_Spring_Data* coefs);
	void initHigh(L6SPB_High_Frequency_Spring_Data* coefs);

	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeepValleySpringAudioProcessor)
};
