/*
  ==============================================================================

    OSDPreProcessingComponent.cpp
    Created: 4 Jan 2017 8:02:42pm
    Author:  Joshua Marler

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "OSDPreProcessingComponent.h"
#include "../../PluginProcessor.h"

String OSDPreProcessingComponent::useAdaptWhitenButtonID("useAdaptWhiten_btn");

//==============================================================================
OSDPreProcessingComponent::OSDPreProcessingComponent(BeatboxVoxAudioProcessor& p)
	: processor(p)
{
	headingLabel.setText("Pre-Processing", NotificationType::dontSendNotification);
	headingLabel.setFont(Font("Cracked", 14.0f, Font::plain));
	headingLabel.setColour(Label::textColourId, Colours::greenyellow);
	addAndMakeVisible(headingLabel);

	useAdaptWhitenLabel.setText("Use Adaptive Whitening", juce::NotificationType::dontSendNotification);
	useAdaptWhitenLabel.setFont(Font("Cracked", 14.0f, Font::plain));
	useAdaptWhitenLabel.setColour(Label::textColourId, Colours::greenyellow);
	addAndMakeVisible(useAdaptWhitenLabel);

	useAdaptWhitenButton.addListener(this);
	useAdaptWhitenButton.setComponentID(useAdaptWhitenButtonID);
	useAdaptWhitenButton.setToggleState(false, juce::NotificationType::dontSendNotification);
	addAndMakeVisible(useAdaptWhitenButton);
}

OSDPreProcessingComponent::~OSDPreProcessingComponent()
{
}

void OSDPreProcessingComponent::paint (Graphics& g)
{
	g.setColour(Colours::black.withAlpha(static_cast<uint8>(0xdc))); 
	Rectangle<float> fillRect(getWidth(), getHeight());
	g.fillRoundedRectangle(fillRect, 10.0f);
}

void OSDPreProcessingComponent::resized()
{
	auto bounds = getLocalBounds();

	bounds.reduce((bounds.getWidth() / 50), (bounds.getHeight()) / 50);

	auto headerSection = bounds.removeFromTop(bounds.getHeight() / 20);
	headingLabel.setBounds(headerSection);

	auto useAdaptWhitenArea = bounds.removeFromTop(bounds.getHeight() / 10);
	useAdaptWhitenButton.setBounds(useAdaptWhitenArea.removeFromLeft(useAdaptWhitenArea.getWidth() / 10));
	useAdaptWhitenLabel.setBounds(useAdaptWhitenArea);
}

void OSDPreProcessingComponent::buttonClicked(Button * button)
{
	auto id = button->getComponentID();

	if (id == useAdaptWhitenButtonID)
		processor.getClassifier().setOSDUseAdaptiveWhitening(button->getToggleState());
}
