/*
  ==============================================================================

    OSDOptionsComponent.cpp
    Created: 4 Jan 2017 8:02:11pm
    Author:  Joshua Marler

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "OSDOptionsComponent.h"

#include "../../AudioClassify/src/AudioClassifyOptions/AudioClassifyOptions.h"

String OSDOptionsComponent::useLocalMaximumButtonID("useLocalMaximum_btn");
String OSDOptionsComponent::useOSDTestSoundButtonID("useOSDTestSound_btn");
String OSDOptionsComponent::odfComboBoxID("odf_cmb");

//==============================================================================
OSDOptionsComponent::OSDOptionsComponent(BeatboxVoxAudioProcessor& p) 
	: processor(p)
{
	headingLabel.setText("Onset Detector Options", NotificationType::dontSendNotification);
	headingLabel.setFont(Font("Cracked", 14.0f, Font::plain));
	headingLabel.setColour(Label::textColourId, Colours::greenyellow);
	addAndMakeVisible(headingLabel);

	odfTypeLabel.setText("Onset Detection Function", NotificationType::dontSendNotification);
    odfTypeLabel.setFont(Font("Cracked", 14.0f, Font::plain));
    odfTypeLabel.setColour(Label::textColourId, Colours::greenyellow);
	addAndMakeVisible(odfTypeLabel);

	useLocalMaxLabel.setText("Use Local Maximum", NotificationType::dontSendNotification);
    useLocalMaxLabel.setFont(Font("Cracked", 14.0f, Font::plain));
    useLocalMaxLabel.setColour(Label::textColourId, Colours::greenyellow);
	addAndMakeVisible(useLocalMaxLabel);

	useLocalMaximumButton.addListener(this);
	useLocalMaximumButton.setComponentID(useLocalMaximumButtonID);
	useLocalMaximumButton.setToggleState(false, NotificationType::dontSendNotification);
	addAndMakeVisible(useLocalMaximumButton);

	useOSDTestSoundLabel.setText("Use Test Sound", NotificationType::dontSendNotification);
    useOSDTestSoundLabel.setFont(Font("Cracked", 14.0f, Font::plain));
    useOSDTestSoundLabel.setColour(Label::textColourId, Colours::greenyellow);
	addAndMakeVisible(useOSDTestSoundLabel);

	useOSDTestSoundButton.addListener(this);
	useOSDTestSoundButton.setComponentID(useOSDTestSoundButtonID);
	useOSDTestSoundButton.setToggleState(false, NotificationType::dontSendNotification);
	addAndMakeVisible(useOSDTestSoundButton);

	populateODFTypeSelector();
	addAndMakeVisible(odfTypeSelector);
}

OSDOptionsComponent::~OSDOptionsComponent()
{
}

void OSDOptionsComponent::paint (Graphics& g)
{
	g.setColour(Colours::black.withAlpha(static_cast<uint8>(0xdc)));

	Rectangle<float> fillRect(getWidth(), getHeight());
	g.fillRoundedRectangle(fillRect, 10.0f);
}

void OSDOptionsComponent::resized()
{
	auto bounds(getLocalBounds());

	auto headerSection = bounds.removeFromTop(bounds.getHeight() / 10);
	headerSection.reduce(headerSection.getWidth() / 30, 0);
	headingLabel.setBounds(headerSection);

	auto leftSection = bounds.removeFromLeft(bounds.getWidth() / 2);
	auto rightSection = bounds;

	leftSection.reduce(leftSection.getWidth() / 10, leftSection.getHeight() / 8);
	rightSection.reduce(rightSection.getWidth() / 10, rightSection.getHeight() / 8);

	auto useLocalMaxBounds = leftSection.removeFromTop(leftSection.getHeight() / 2);
	useLocalMaximumButton.setBounds(useLocalMaxBounds.removeFromLeft(useLocalMaxBounds.getWidth() / 8));
	useLocalMaxLabel.setBounds(useLocalMaxBounds);

	auto useOSDTestSoundBounds = leftSection;
	useOSDTestSoundButton.setBounds(useOSDTestSoundBounds.removeFromLeft(useOSDTestSoundBounds.getWidth() / 8));
	useOSDTestSoundLabel.setBounds(useOSDTestSoundBounds);

	odfTypeLabel.setBounds(rightSection.removeFromTop(rightSection.getHeight() / 5));
	odfTypeSelector.setBounds(rightSection.removeFromTop(rightSection.getHeight() / 2));

}

void OSDOptionsComponent::buttonClicked(Button * button)
{
	auto id = button->getComponentID();

	if (id == useLocalMaximumButtonID)
		processor.getClassifier().setOSDUseLocalMaximum(button->getToggleState());
	else if (id == useOSDTestSoundButtonID)
		processor.setUsingOSDTestSound(button->getToggleState());
}

void OSDOptionsComponent::comboBoxChanged(ComboBox * comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged->getComponentID() == odfComboBoxID)
    {
        auto& classifier = processor.getClassifier();

        auto odfType = odfTypeSelector.getSelectedId() - 1;
        classifier.setOSDDetectorFunctionType(static_cast<AudioClassifyOptions::ODFType>(odfType));
    }
}

void OSDOptionsComponent::populateODFTypeSelector()
{
	//odf enumerated values in AudioClassifyOptions begin at 0 so adding 1 to ID's - can't use 0 value for combobox id 
    odfTypeSelector.addItem("Spectral Difference", static_cast<int>(AudioClassifyOptions::ODFType::spectralDifference) + 1); 
    odfTypeSelector.addItem("Spectral Difference HWR", static_cast<int>(AudioClassifyOptions::ODFType::spectralDifferenceHWR) + 1);
    odfTypeSelector.addItem("High Frequency Content", static_cast<int>(AudioClassifyOptions::ODFType::highFrequencyContent) + 1);

    odfTypeSelector.setSelectedId(static_cast<int>(AudioClassifyOptions::ODFType::spectralDifference) + 1);
}
