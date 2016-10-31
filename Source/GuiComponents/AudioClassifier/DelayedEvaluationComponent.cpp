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
String DelayedEvaluationComponent::headingLabelID("heading_lbl");
String DelayedEvaluationComponent::activateButtonID("activate_btn");
String DelayedEvaluationComponent::bufferDelaySliderID("bufferDelay_sld");
String DelayedEvaluationComponent::useEarlyAttackButtonID("useEarlyAttack_btn");
//==============================================================================
DelayedEvaluationComponent::DelayedEvaluationComponent(BeatboxVoxAudioProcessor& p)
	: processor(p)
{
	activateButton.setComponentID(activateButtonID);
	activateButton.addListener(this);
	addAndMakeVisible(activateButton);

	headingLabel.setComponentID(headingLabelID);
	headingLabel.setText("Delayed Evaluation", NotificationType::dontSendNotification);
	headingLabel.setFont(Font("Cracked", 14.0f, Font::plain));
	headingLabel.setColour(Label::textColourId, Colours::greenyellow);
	addAndMakeVisible(headingLabel);


	bufferDelayLabel.setText("Num buffers delayed", NotificationType::dontSendNotification);
	bufferDelayLabel.setFont(Font("Cracked", 14.0f, Font::plain));
	bufferDelayLabel.setColour(Label::textColourId, Colours::greenyellow);
	addAndMakeVisible(bufferDelayLabel);

	bufferDelaySlider.setComponentID(bufferDelaySliderID);
	bufferDelaySlider.setSliderStyle (Slider::IncDecButtons);
	bufferDelaySlider.setRange (0.0, 5.0, 1.0);
	bufferDelaySlider.setIncDecButtonsMode (Slider::incDecButtonsDraggable_Horizontal);
	bufferDelaySlider.setTextBoxStyle(Slider::TextBoxRight, true, 90, 20);
	bufferDelaySlider.setColour(Slider::textBoxBackgroundColourId, Colours::black);
	bufferDelaySlider.setColour(Slider::textBoxTextColourId, Colours::greenyellow);
	bufferDelaySlider.setColour(Slider::textBoxOutlineColourId, Colours::black);
	addAndMakeVisible (bufferDelaySlider);


	numSamplesUsedLbl.setText("Samples used per instance: ", NotificationType::dontSendNotification);
	numSamplesUsedLbl.setFont(Font("Cracked", 14.0f, Font::plain));
	numSamplesUsedLbl.setColour(Label::textColourId, Colours::greenyellow);
	addAndMakeVisible(numSamplesUsedLbl);

	//JWM - Eventually replace hard coded num samples with val calculated in timer callback
	numSamplesUsedVal.setText("256", NotificationType::dontSendNotification);
	numSamplesUsedVal.setFont(Font("Cracked", 14.0f, Font::plain));
	numSamplesUsedVal.setColour(Label::textColourId, Colours::greenyellow);
	addAndMakeVisible(numSamplesUsedVal);

	numBuffersUsedLbl.setText("Buffers used per instance: ", NotificationType::dontSendNotification);
	numBuffersUsedLbl.setFont(Font("Cracked", 14.0f, Font::plain));
	numBuffersUsedLbl.setColour(Label::textColourId, Colours::greenyellow);
	addAndMakeVisible(numBuffersUsedLbl);


	//JWM - Eventually replace hard coded num buffers with val calculated in timer callback based on if local max used etc in OSD
	numBuffersUsedVal.setText("1", NotificationType::dontSendNotification);
	numBuffersUsedVal.setFont(Font("Cracked", 14.0f, Font::plain));
	numBuffersUsedVal.setColour(Label::textColourId, Colours::greenyellow);
	addAndMakeVisible(numBuffersUsedVal);


	earlyAttackLbl.setText("Trigger early attack sound", NotificationType::dontSendNotification);
	earlyAttackLbl.setFont(Font("Cracked", 14.0f, Font::plain));
	earlyAttackLbl.setColour(Label::textColourId, Colours::greenyellow);
	addAndMakeVisible(earlyAttackLbl);

	activateButton.setToggleState(false, NotificationType::sendNotification);
	setActive(false);
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
	bounds.reduce((bounds.getWidth() / 70), (bounds.getHeight()) / 50);

	auto headingSection = bounds.removeFromTop(bounds.getHeight() / 8);
	activateButton.setBounds(headingSection.removeFromLeft(headingSection.getWidth() / 25));
	headingLabel.setBounds(headingSection);

	auto leftCenter = bounds.removeFromLeft(bounds.getWidth() / 2);
	auto rightCenter = bounds;
	
	//Set buffer delay component bounds
	auto setDelayArea = leftCenter.removeFromTop(leftCenter.getHeight() / 1.75f)
								  .removeFromLeft(leftCenter.getWidth() / 2);

	setDelayArea.reduce(0, setDelayArea.getHeight() / 10);

	bufferDelayLabel.setBounds(setDelayArea.removeFromTop(setDelayArea.getHeight() / 2));
	bufferDelaySlider.setBounds(setDelayArea.removeFromTop(setDelayArea.getHeight() / 1.5f));

	auto bufferDelayLblArea = leftCenter.removeFromTop(leftCenter.getHeight() / 2);
	numBuffersUsedLbl.setBounds(bufferDelayLblArea.removeFromLeft(bufferDelayLblArea.getWidth() / 2));
	numBuffersUsedVal.setBounds(bufferDelayLblArea);

	auto samplesDelayLblArea = leftCenter;
	numSamplesUsedLbl.setBounds(samplesDelayLblArea.removeFromLeft(samplesDelayLblArea.getWidth() / 2));
	numSamplesUsedVal.setBounds(samplesDelayLblArea);

	auto enableAttackArea = rightCenter.removeFromTop(rightCenter.getHeight() / 1.5f);

	enableAttackArea.reduce(0, enableAttackArea.getHeight() / 8);

	earlyAttackLbl.setBounds(enableAttackArea.removeFromTop(enableAttackArea.getHeight() / 3));

}

//==============================================================================
void DelayedEvaluationComponent::buttonClicked(Button * button)
{
	const auto id = button->getComponentID();

	if (id == activateButtonID)
	{
		setActive(button->getToggleState());
	}
	else if (id == useEarlyAttackButtonID)
	{
		setUseEarlyAttack(button->getToggleState());
	}
}

//==============================================================================
void DelayedEvaluationComponent::sliderValueChanged(Slider * slider)
{
	const auto id = slider->getComponentID();

	if (id == bufferDelaySliderID)
	{
		//Update num buffers delay	
		processor.getClassifier().setNumBuffersDelayed(static_cast<unsigned int>(slider->getValue()));
	}
}

//==============================================================================
void DelayedEvaluationComponent::setActive(bool active)
{
	/** The below could be improved upon. This logic would need to be 
	 *  duplicated for all child components to be switched on / off for active states.
	 *  Would be better so subclass Component with another class which acts as an activatable 
	 *  child component and has an activation header/component with an on/off toggle button and title. 
	 *  
	 *	Worth looking into whether this sould be done with composition or inheritance.  
	 *  Laying out heading and activation button logic in header section is also duplicated so a base class
	 *  could be created and implement this resize and setActive logic, maybe with component name / heading text passed
	 *  into the constructor ? May be overkill though.	   
	 */
	for (auto i = 0; i < getNumChildComponents(); ++i)
	{
		auto child = getChildComponent(i);
		auto childID = child->getComponentID();

		if (!active)
		{
			if (childID != activateButtonID && childID != headingLabelID) 
				child->setEnabled(false);
		}
		else
		{
			child->setEnabled(true);
		}
	}
}

//==============================================================================
void DelayedEvaluationComponent::setUseEarlyAttack(bool use)
{
}

//==============================================================================
