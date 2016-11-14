/*
  ==============================================================================

    TestClassifierComponent.h
    Created: 12 Nov 2016 10:21:28pm
    Author:  Joshua Marler

  ==============================================================================
*/

#ifndef TESTCLASSIFIERCOMPONENT_H_INCLUDED
#define TESTCLASSIFIERCOMPONENT_H_INCLUDED

#include <memory>

#include "../JuceLibraryCode/JuceHeader.h"
#include "../CustomLookAndFeel.h"

//==============================================================================
/*
*/
class TestClassifierComponent    : public Component,
								   ButtonListener,
								   SliderListener,
								   TableListBoxModel
{
public:
    TestClassifierComponent();
    ~TestClassifierComponent();

    void paint (Graphics&) override;
    void resized() override;

	void buttonClicked(Button* button) override;
	void sliderValueChanged(Slider* slider) override;

	//TableListBoxModel overrides
	int getNumRows() override;

	void paintRowBackground(Graphics & g, int rowNumber, int, int, bool rowIsSelected) override;

	void paintCell(Graphics & g, int rowNumber, int columnId, int width, int height, bool) override;

	static String runTestButtonID;
	static String testSetSliderID;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestClassifierComponent)

	//NOTE - Ideally change this at a later point to take look and feel from parent of the owning dialog. 
	std::unique_ptr<LookAndFeel> componentLookAndFeel;

	TextButton runTestButton;

	Label testSetSizeLabel;
	Slider testSetSizeSlider;

	Label headingLabel;
	Label accuracyLabel;

	TableListBox table;

	unsigned int numTestResults = 0;

};


#endif  // TESTCLASSIFIERCOMPONENT_H_INCLUDED
