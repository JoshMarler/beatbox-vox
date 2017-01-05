/*
  ==============================================================================

    OSDOptionsComponent.h
    Created: 4 Jan 2017 8:02:11pm
    Author:  Joshua Marler

  ==============================================================================
*/

#ifndef OSDOPTIONSCOMPONENT_H_INCLUDED
#define OSDOPTIONSCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "../../PluginProcessor.h"

//==============================================================================
/*
*/
class OSDOptionsComponent    : public Component,
							   ButtonListener,
							   ComboBoxListener
{
public:
    OSDOptionsComponent(BeatboxVoxAudioProcessor& p);
    ~OSDOptionsComponent();

    void paint (Graphics&) override;
    void resized() override;

	void buttonClicked(Button* button) override;
	void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;

	static String useLocalMaximumButtonID;
	static String useOSDTestSoundButtonID;
	static String odfComboBoxID;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OSDOptionsComponent)

	BeatboxVoxAudioProcessor& processor;

	Label headingLabel;

	Label useLocalMaxLabel;
	Label useOSDTestSoundLabel;
	Label odfTypeLabel;

	ToggleButton useLocalMaximumButton;
	ToggleButton useOSDTestSoundButton;
	ComboBox odfTypeSelector;

	void populateODFTypeSelector();
};


#endif  // OSDOPTIONSCOMPONENT_H_INCLUDED
