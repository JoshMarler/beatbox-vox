/*
  ==============================================================================

    DelayedEvaluationComponent.cpp
    Created: 28 Oct 2016 9:00:48pm
    Author:  Joshua Marler

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "DelayedEvaluationComponent.h"

//==============================================================================
DelayedEvaluationComponent::DelayedEvaluationComponent()
{
	setSize(400, 300);

	headingLabel.setText("Delayed Evaluation Settings", NotificationType::dontSendNotification);
	headingLabel.setFont(Font("Cracked", 14.0f, Font::plain));
	headingLabel.setColour(Label::textColourId, Colours::greenyellow);
	addAndMakeVisible(headingLabel);
}

//==============================================================================
DelayedEvaluationComponent::~DelayedEvaluationComponent()
{
}

//==============================================================================
void DelayedEvaluationComponent::paint (Graphics& g)
{
	g.setColour(Colours::black.withAlpha(static_cast<uint8>(0xdc)));

	Rectangle<float> fillRect(getWidth(), getHeight());
	g.fillRoundedRectangle(fillRect, 10.0f);
}

//==============================================================================
void DelayedEvaluationComponent::resized()
{
	auto bounds = getLocalBounds();

	//Indent remaining components
	bounds.reduce((bounds.getWidth() / 50), (bounds.getHeight()) / 50);

	auto headingSection = bounds.removeFromTop(bounds.getHeight() / 15);
	headingLabel.setBounds(headingSection);
}

//==============================================================================
