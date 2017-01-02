/*
  ==============================================================================

    BufferHandlingComponent.cpp
    Created: 28 Oct 2016 9:00:48pm
    Author:  Joshua Marler

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "BufferHandlingComponent.h"

//==============================================================================
String BufferHandlingComponent::headingLabelID("heading_lbl");
String BufferHandlingComponent::activateButtonID("activate_btn");
String BufferHandlingComponent::bufferDelaySliderID("bufferDelay_sld");
String BufferHandlingComponent::bufferDelayUpdateButtonID("buffer_delay_update_btn");
String BufferHandlingComponent::stftNumFramesSliderID("stft_num_frames_sld");
String BufferHandlingComponent::stftFramesUpdateButtonID("stft_frames_update_btn");
//==============================================================================
BufferHandlingComponent::BufferHandlingComponent(BeatboxVoxAudioProcessor& p)
	: processor(p)
{
	activateButton.setComponentID(activateButtonID);
	activateButton.addListener(this);
	addAndMakeVisible(activateButton);

	headingLabel.setComponentID(headingLabelID);
	headingLabel.setText("Buffer Handling", NotificationType::dontSendNotification);
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
	bufferDelaySlider.addListener(this);
	addAndMakeVisible (bufferDelaySlider);

	bufferDelayUpdateButton.setComponentID(bufferDelayUpdateButtonID);
	bufferDelayUpdateButton.setButtonText("Update");
	bufferDelayUpdateButton.addListener(this);
	addAndMakeVisible(bufferDelayUpdateButton);

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

	stftNumFramesLabel.setText("Num STFT Frames Per Buffer", NotificationType::dontSendNotification);
	stftNumFramesLabel.setFont(Font("Cracked", 14.0f, Font::plain));
	stftNumFramesLabel.setColour(Label::textColourId, Colours::greenyellow);
	addAndMakeVisible(stftNumFramesLabel);

	stftNumFramesSlider.setComponentID(stftNumFramesSliderID);
	stftNumFramesSlider.setSliderStyle(Slider::IncDecButtons);
	stftNumFramesSlider.setRange(1.0, 16.0, 1.0);
	stftNumFramesSlider.setIncDecButtonsMode(Slider::incDecButtonsDraggable_Horizontal);
	stftNumFramesSlider.setTextBoxStyle(Slider::TextBoxRight, true, 90, 20);
	stftNumFramesSlider.setColour(Slider::textBoxBackgroundColourId, Colours::black);
	stftNumFramesSlider.setColour(Slider::textBoxTextColourId, Colours::greenyellow);
	stftNumFramesSlider.setColour(Slider::textBoxOutlineColourId, Colours::black);
	stftNumFramesSlider.addListener(this);
	addAndMakeVisible(stftNumFramesSlider);


	stftFrameSizeLbl.setText("STFT Frame Size: ", NotificationType::dontSendNotification);
	stftFrameSizeLbl.setFont(Font("Cracked", 14.0f, Font::plain));
	stftFrameSizeLbl.setColour(Label::textColourId, Colours::greenyellow);
	addAndMakeVisible(stftFrameSizeLbl);

	stftFrameSizeVal.setText("", NotificationType::dontSendNotification);
	stftFrameSizeVal.setFont(Font("Cracked", 14.0f, Font::plain));
	stftFrameSizeVal.setColour(Label::textColourId, Colours::greenyellow);
	addAndMakeVisible(stftFrameSizeVal);

	stftFramesUpdateButton.setComponentID(stftFramesUpdateButtonID);
	stftFramesUpdateButton.setButtonText("Update");
	stftFramesUpdateButton.addListener(this);
	addAndMakeVisible(stftFramesUpdateButton);


	activateButton.setToggleState(false, NotificationType::sendNotification);
	setActive(false);
}

//==============================================================================
BufferHandlingComponent::~BufferHandlingComponent()
{
}

//==============================================================================
void BufferHandlingComponent::paint (Graphics& g)
{
	g.setColour(Colours::black.withAlpha(static_cast<uint8>(0xdc)));

	Rectangle<float> fillRect(getWidth(), getHeight());
	g.fillRoundedRectangle(fillRect, 10.0f);
}

//==============================================================================
void BufferHandlingComponent::resized()
{
	auto bounds = getLocalBounds();

	//Indent remaining components
	bounds.reduce((bounds.getWidth() / 70), (bounds.getHeight()) / 50);

	auto headingSection = bounds.removeFromTop(bounds.getHeight() / 8);
	activateButton.setBounds(headingSection.removeFromLeft(headingSection.getWidth() / 25));
	headingLabel.setBounds(headingSection);

	auto leftCenter = bounds.removeFromLeft(bounds.getWidth() / 2);
	auto rightCenter = bounds;

	leftCenter.reduce(leftCenter.getWidth() / 30, 0);
	rightCenter.reduce(rightCenter.getWidth() / 30, 0);
	
	//Set buffer delay component bounds
	auto setDelayArea = leftCenter.removeFromTop(leftCenter.getHeight() / 1.75f);
	setDelayArea.reduce(0, setDelayArea.getHeight() / 10);
	
	auto setDelaySliderArea = setDelayArea.removeFromLeft(setDelayArea.getWidth() / 2);
	bufferDelayLabel.setBounds(setDelaySliderArea.removeFromTop(setDelaySliderArea.getHeight() / 2));
	bufferDelaySlider.setBounds(setDelaySliderArea.removeFromTop(setDelaySliderArea.getHeight() / 1.5f));

	auto setDelayButtonArea = setDelayArea;
	setDelayButtonArea.reduce(setDelayArea.getWidth() / 5, setDelayArea.getHeight() / 9);
	bufferDelayUpdateButton.setBounds(setDelayButtonArea.removeFromBottom(setDelayButtonArea.getHeight() / 1.5f));

	auto bufferDelayLblArea = leftCenter.removeFromTop(leftCenter.getHeight() / 2);
	numBuffersUsedLbl.setBounds(bufferDelayLblArea.removeFromLeft(bufferDelayLblArea.getWidth() / 2));
	numBuffersUsedVal.setBounds(bufferDelayLblArea);

	auto samplesDelayLblArea = leftCenter;
	numSamplesUsedLbl.setBounds(samplesDelayLblArea.removeFromLeft(samplesDelayLblArea.getWidth() / 2));
	numSamplesUsedVal.setBounds(samplesDelayLblArea);


	//Set stft component bounds
	auto stftArea = rightCenter;

	auto setNumFramesArea = rightCenter.removeFromTop(rightCenter.getHeight() / 1.75f);
	setNumFramesArea.reduce(0, setNumFramesArea.getHeight() / 10);

	auto setNumFramesSliderArea = setNumFramesArea.removeFromLeft(setNumFramesArea.getWidth() / 2);
	stftNumFramesLabel.setBounds(setNumFramesSliderArea.removeFromTop(setNumFramesSliderArea.getHeight() / 2));
	stftNumFramesSlider.setBounds(setNumFramesSliderArea.removeFromTop(setNumFramesSliderArea.getHeight() / 1.5f));

	auto setNumFramesButtonArea = setNumFramesArea;
	setNumFramesButtonArea.reduce(setNumFramesArea.getWidth() / 5, setNumFramesArea.getHeight() / 9);
	stftFramesUpdateButton.setBounds(setNumFramesButtonArea.removeFromBottom(setNumFramesButtonArea.getHeight() / 1.5f));

	auto stftFrameSizeLblArea = rightCenter.removeFromTop(rightCenter.getHeight() / 2);
	stftFrameSizeLbl.setBounds(stftFrameSizeLblArea.removeFromLeft(stftFrameSizeLblArea.getWidth() / 2));
	stftFrameSizeVal.setBounds(stftFrameSizeLblArea);
}

//==============================================================================
void BufferHandlingComponent::buttonClicked(Button * button)
{
	const auto id = button->getComponentID();

	if (id == activateButtonID)
		setActive(button->getToggleState());
	else if (id == bufferDelayUpdateButtonID)
		processor.getClassifier().setNumBuffersDelayed(static_cast<unsigned int>(bufferDelaySlider.getValue()));
	else if (id == stftFramesUpdateButtonID)
		processor.getClassifier().setSTFTFramesPerBuffer(static_cast<unsigned int>(stftNumFramesSlider.getValue()));

	setNeedsUpdate(false, *button);
}

//==============================================================================
void BufferHandlingComponent::sliderValueChanged(Slider * slider)
{
	const auto id = slider->getComponentID();
	auto currentVal = 0;
	auto newVal = static_cast<int>(slider->getValue());
	Button* buttonToUpdate = nullptr;

	if (id == bufferDelaySliderID)
	{
		currentVal = processor.getClassifier().getNumBuffersDelayed();
		buttonToUpdate = &bufferDelayUpdateButton;
	}
	else if (id == stftNumFramesSliderID)
	{
		currentVal = processor.getClassifier().getSTFTFramesPerBuffer();
		buttonToUpdate = &stftFramesUpdateButton;
	}

	if (newVal != currentVal)
		setNeedsUpdate(true, *buttonToUpdate);
	else
		setNeedsUpdate(false, *buttonToUpdate);
}

//==============================================================================
void BufferHandlingComponent::handleNewTrainingSetLoaded()
{

	auto numStftFrames = processor.getClassifier().getSTFTFramesPerBuffer();
	stftNumFramesSlider.setValue(numStftFrames, juce::NotificationType::dontSendNotification);

	auto numDelayedBuffers = processor.getClassifier().getNumBuffersDelayed();
	bufferDelaySlider.setValue(numDelayedBuffers, juce::NotificationType::dontSendNotification);
}

//==============================================================================
void BufferHandlingComponent::setActive(bool active) const
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
void BufferHandlingComponent::setNeedsUpdate(bool needsUpdate, Button& button)
{
	if (needsUpdate)
	{
		button.setColour(TextButton::buttonColourId, Colours::greenyellow);
		button.setColour(TextButton::textColourOffId, Colours::black);
	}
	else
	{
		button.setColour(TextButton::buttonColourId, Colours::black);
		button.setColour(TextButton::textColourOffId, Colours::greenyellow);
	}

}

//==============================================================================
