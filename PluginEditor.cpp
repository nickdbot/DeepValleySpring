/*
  ==============================================================================

  copyright 2018 to DEEP VALLEY AUDIO BIIIIIITCH

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DeepValleySpringAudioProcessorEditor::DeepValleySpringAudioProcessorEditor (DeepValleySpringAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setSize (400, 400);

	//initialize low freq interface things!!
	addAndMakeVisible(&ChirpFCSlider);
	ChirpFCSlider.setRange(2500.0, 8000.0);
	ChirpFCSlider.setTextValueSuffix(" Hz");
	ChirpFCSlider.addListener(this);
	ChirpFCSlider.setValue(3500.0);
	addAndMakeVisible(&ChirpFCLabel);
	ChirpFCLabel.setText("Chirp Cutoff Freq", dontSendNotification);
	ChirpFCLabel.attachToComponent(&ChirpFCSlider, true);

	addAndMakeVisible(&NumRippleSlider);
	NumRippleSlider.setRange(0.0, 6.0);
	NumRippleSlider.addListener(this);
	NumRippleSlider.setValue(0.5);
	addAndMakeVisible(&NumRippleLabel);
	NumRippleLabel.setText("Ripple", dontSendNotification);
	NumRippleLabel.attachToComponent(&NumRippleSlider, true);

	addAndMakeVisible(&DelayTimeLSlider);
	DelayTimeLSlider.setRange(0.0, 0.064);
	DelayTimeLSlider.setTextValueSuffix(" s");
	DelayTimeLSlider.addListener(this);
	DelayTimeLSlider.setValue(0.056);
	addAndMakeVisible(&DelayTimeLLabel);
	DelayTimeLLabel.setText("Low Predelay", dontSendNotification);
	DelayTimeLLabel.attachToComponent(&DelayTimeLSlider, true);

	addAndMakeVisible(&DelayLineFBLSlider);
	DelayLineFBLSlider.setRange(0.0, 1.0);
	DelayLineFBLSlider.addListener(this);
	DelayLineFBLSlider.setValue(0.5);
	addAndMakeVisible(&DelayLineFBLLabel);
	DelayLineFBLLabel.setText("Low Feedback Amount", dontSendNotification);
	DelayLineFBLLabel.attachToComponent(&DelayLineFBLSlider, true);

	addAndMakeVisible(&OutputLSlider);
	OutputLSlider.setRange(0.0, 1.0);
	OutputLSlider.addListener(this);
	OutputLSlider.setValue(1.0);
	addAndMakeVisible(&OutputLLabel);
	OutputLLabel.setText("Low Section Gain", dontSendNotification);
	OutputLLabel.attachToComponent(&OutputLSlider, true);

	addAndMakeVisible(&DelayModLSlider);
	DelayModLSlider.setRange(0.0, 40.0);
	DelayModLSlider.addListener(this);
	DelayModLSlider.setValue(12.0);
	addAndMakeVisible(&DelayModLLabel);
	DelayModLLabel.setText("Low Delay Line Modulation", dontSendNotification);
	DelayModLLabel.attachToComponent(&DelayModLSlider, true);

	addAndMakeVisible(&PreechoGainSlider);
	PreechoGainSlider.setRange(0.0, 1.0);
	PreechoGainSlider.addListener(this);
	PreechoGainSlider.setValue(0.5);
	addAndMakeVisible(&PreechoGainLabel);
	PreechoGainLabel.setText("Preecho Gain", dontSendNotification);
	PreechoGainLabel.attachToComponent(&PreechoGainSlider, true);

	//initialize high freq interface things!!
	/*addAndMakeVisible(&DelayTimeHSlider);
	DelayTimeHSlider.setRange(0.0, 0.032);
	DelayTimeHSlider.setTextValueSuffix(" s");
	DelayTimeHSlider.addListener(this);
	DelayTimeHSlider.setValue(0.021);
	addAndMakeVisible(&DelayTimeHLabel);
	DelayTimeHLabel.setText("High Predelay", dontSendNotification);
	DelayTimeHLabel.attachToComponent(&DelayTimeHSlider, true);
	*/

	addAndMakeVisible(&DelayLineFBHSlider);
	DelayLineFBHSlider.setRange(0.0, 1.0);
	DelayLineFBHSlider.addListener(this);
	DelayLineFBHSlider.setValue(0.5);
	addAndMakeVisible(&DelayLineFBHLabel);
	DelayLineFBHLabel.setText("High Feedback Amount", dontSendNotification);
	DelayLineFBHLabel.attachToComponent(&DelayLineFBHSlider, true);

	addAndMakeVisible(&OutputHSlider);
	OutputHSlider.setRange(0.0, 1.0);
	OutputHSlider.addListener(this);
	addAndMakeVisible(&OutputHLabel);
	OutputHSlider.setValue(0.1);
	OutputHLabel.setText("High Section Gain", dontSendNotification);
	OutputHLabel.attachToComponent(&OutputHSlider, true);

	addAndMakeVisible(&DelayModHSlider);
	DelayModHSlider.setRange(0.0, 40.0);
	DelayModHSlider.addListener(this);
	DelayModHSlider.setValue(12.0);
	addAndMakeVisible(&DelayModHLabel);
	DelayModHLabel.setText("High Delay Line Modulation", dontSendNotification);
	DelayModHLabel.attachToComponent(&DelayModHSlider, true);

	//Dry signal gain
	addAndMakeVisible(&DryGainSlider);
	DryGainSlider.setRange(0.0, 1.0);
	DryGainSlider.addListener(this);
	DryGainSlider.setValue(1.0);
	addAndMakeVisible(&DryGainLabel);
	DryGainLabel.setText("Dry Signal Gain", dontSendNotification);
	DryGainLabel.attachToComponent(&DryGainSlider, true);
}


DeepValleySpringAudioProcessorEditor::~DeepValleySpringAudioProcessorEditor()
{
}

//==============================================================================
void DeepValleySpringAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
	/*
    g.setColour (Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("punch me directly in the nutsack", getLocalBounds(), Justification::centred, 1);
	*/
}

void DeepValleySpringAudioProcessorEditor::resized()
{
	auto sliderLeft = 120;

	//low freq stuff
	ChirpFCSlider.setBounds(sliderLeft, 20, getWidth() - sliderLeft - 10, 20);
	NumRippleSlider.setBounds(sliderLeft, 50, getWidth() - sliderLeft - 10, 20);
	DelayTimeLSlider.setBounds(sliderLeft, 80, getWidth() - sliderLeft - 10, 20);
	DelayLineFBLSlider.setBounds(sliderLeft, 110, getWidth() - sliderLeft - 10, 20);
	OutputLSlider.setBounds(sliderLeft, 140, getWidth() - sliderLeft - 10, 20);
	DelayModLSlider.setBounds(sliderLeft, 170, getWidth() - sliderLeft - 10, 20);
	PreechoGainSlider.setBounds(sliderLeft, 200, getWidth() - sliderLeft - 10, 20);

	//high freq stuff
	//DelayTimeHSlider.setBounds(sliderLeft, 230, getWidth() - sliderLeft - 10, 20);
	DelayLineFBHSlider.setBounds(sliderLeft, 260, getWidth() - sliderLeft - 10, 20);
	OutputHSlider.setBounds(sliderLeft, 290, getWidth() - sliderLeft - 10, 20);
	DelayModHSlider.setBounds(sliderLeft, 320, getWidth() - sliderLeft - 10, 20);

	//Dry section, just gain
	DryGainSlider.setBounds(sliderLeft, 350, getWidth() - sliderLeft - 10, 20);
}

void DeepValleySpringAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
	float value = slider->getValue();
	
	//low freq section
	if (slider == &ChirpFCSlider) { 
		processor.L6SPB_Low_Frequency_Spring_PChg(0, value);
	}
	else if (slider == &NumRippleSlider) {
		processor.L6SPB_Low_Frequency_Spring_PChg(1, value);
	}
	else if (slider == &DelayTimeLSlider) {
		processor.L6SPB_Low_Frequency_Spring_PChg(2, value);
		processor.L6SPB_High_Frequency_Spring_PChg(1, value);
	}
	else if (slider == &DelayLineFBLSlider) {
		processor.L6SPB_Low_Frequency_Spring_PChg(3, value);
	}
	else if (slider == &OutputLSlider) {
		processor.L6SPB_Low_Frequency_Spring_PChg(4, value);
	}
	else if (slider == &DelayModLSlider) {
		processor.L6SPB_Low_Frequency_Spring_PChg(5, value);
	}
	else if (slider == &PreechoGainSlider) {
		processor.L6SPB_Low_Frequency_Spring_PChg(6, value);
	}
	//high freq section
	//else if (slider == &DelayTimeHSlider) {
	//	processor.L6SPB_High_Frequency_Spring_PChg(0, value);
	//}
	else if (slider == &DelayLineFBHSlider) {
		processor.L6SPB_High_Frequency_Spring_PChg(1, value);
	}
	else if (slider == &OutputHSlider) {
		processor.L6SPB_High_Frequency_Spring_PChg(2, value);
	}
	else if (slider == &DelayModHSlider) {
		processor.L6SPB_High_Frequency_Spring_PChg(3, value);
	}
	else if (slider == &DryGainSlider) {
		processor.GDRY = value;
	}

}