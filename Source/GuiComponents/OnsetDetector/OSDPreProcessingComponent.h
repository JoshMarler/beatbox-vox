/*
  ==============================================================================

    OSDPreProcessingComponent.h
    Created: 4 Jan 2017 8:02:42pm
    Author:  Joshua Marler

  ==============================================================================
*/

#ifndef OSDPREPROCESSINGCOMPONENT_H_INCLUDED
#define OSDPREPROCESSINGCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "../../PluginProcessor.h"

//==============================================================================
/*
*/
class OSDPreProcessingComponent    : public Component,
									 ButtonListener
{
public:
    OSDPreProcessingComponent(BeatboxVoxAudioProcessor& p);
    ~OSDPreProcessingComponent();

    void paint (Graphics&) override;
    void resized() override;

	void buttonClicked(Button* button) override;

	static String useAdaptWhitenButtonID;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OSDPreProcessingComponent)

	BeatboxVoxAudioProcessor& processor;
	
	Label headingLabel;

	Label useAdaptWhitenLabel;
	ToggleButton useAdaptWhitenButton;
};


#endif  // OSDPREPROCESSINGCOMPONENT_H_INCLUDED
