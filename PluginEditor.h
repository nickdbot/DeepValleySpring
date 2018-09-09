/*
  ==============================================================================

  copyright 2018 to DEEP VALLEY AUDIO BIIIIIITCH

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class DeepValleySpringAudioProcessorEditor  : public AudioProcessorEditor, public Slider::Listener
{
public:
    DeepValleySpringAudioProcessorEditor (DeepValleySpringAudioProcessor&);
    ~DeepValleySpringAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DeepValleySpringAudioProcessor& processor;

	void sliderValueChanged(Slider* slider) override;

	//Low Freq section interface stuff
	Slider ChirpFCSlider;
	Label ChirpFCLabel;
	Slider NumRippleSlider;
	Label NumRippleLabel;
	Slider DelayTimeLSlider;
	Label DelayTimeLLabel;
	Slider DelayLineFBLSlider;
	Label DelayLineFBLLabel;
	Slider OutputLSlider;
	Label OutputLLabel;
	Slider DelayModLSlider;
	Label DelayModLLabel;
	Slider PreechoGainSlider;
	Label PreechoGainLabel;

	//High Freq section interface stuff
	//Slider DelayTimeHSlider;
	//Label DelayTimeHLabel;
	Slider DelayLineFBHSlider;
	Label DelayLineFBHLabel;
	Slider OutputHSlider;
	Label OutputHLabel;
	Slider DelayModHSlider;
	Label DelayModHLabel;

	//dry gain
	Slider DryGainSlider;
	Label DryGainLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeepValleySpringAudioProcessorEditor)
};
