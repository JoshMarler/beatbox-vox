/*
  ==============================================================================

    FeaturesComponent.cpp
    Created: 28 Oct 2016 10:33:45pm
    Author:  Joshua Marler

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "FeaturesComponent.h"
#include "FeatureVariancesComponent.h"

//==============================================================================
String FeaturesComponent::headingLabelID("heading_lbl");
String FeaturesComponent::showVariancesButtonID("showVariances_btn");
String FeaturesComponent::updateFeatureVariancesButtonID("updateFeatureVariances_btn");

//==============================================================================
FeaturesComponent::FeaturesComponent(BeatboxVoxAudioProcessor& p)
	: processor(p)
{
	headingLabel.setComponentID(headingLabelID);
	headingLabel.setText("Features", NotificationType::dontSendNotification);
	headingLabel.setFont(Font("Cracked", 14.0f, Font::plain));
	headingLabel.setColour(Label::textColourId, Colours::greenyellow);
	addAndMakeVisible(headingLabel);

	numFeaturesInUseLbl.setText("Num Features In Use: ", NotificationType::dontSendNotification);
	numFeaturesInUseLbl.setFont(Font("Cracked", 14.0f, Font::plain));
	numFeaturesInUseLbl.setColour(Label::textColourId, Colours::greenyellow);
	addAndMakeVisible(numFeaturesInUseLbl);

	numFeaturesInUseVal.setFont(Font("Cracked", 14.0f, Font::plain));
	numFeaturesInUseVal.setColour(Label::textColourId, Colours::greenyellow);
	addAndMakeVisible(numFeaturesInUseVal);

	varianceReduceControlsHeadingLabel.setText("Reduce Features By Variance", NotificationType::dontSendNotification);
	varianceReduceControlsHeadingLabel.setFont(Font("Cracked", 14.0f, Font::plain));
	varianceReduceControlsHeadingLabel.setColour(Label::textColourId, Colours::greenyellow);
	addAndMakeVisible(varianceReduceControlsHeadingLabel);

	showVariancesButton.setComponentID(showVariancesButtonID);
	showVariancesButton.setButtonText("Show Variances");
	showVariancesButton.addListener(this);
	addAndMakeVisible(showVariancesButton);

	updateFeatureVariancesButton.setComponentID(updateFeatureVariancesButtonID);
	updateFeatureVariancesButton.setButtonText("Update");
	updateFeatureVariancesButton.addListener(this);
	addAndMakeVisible(updateFeatureVariancesButton);

	setupVarianceReduceValueButtons();

	startTimerHz(20);
	setActive(false);
}

//==============================================================================
FeaturesComponent::~FeaturesComponent()
{
}

//==============================================================================
void FeaturesComponent::paint (Graphics& g)
{
	g.setColour(Colours::black.withAlpha(static_cast<uint8>(0xdc)));

	Rectangle<float> fillRect(getWidth(), getHeight());
	g.fillRoundedRectangle(fillRect, 10.0f);
}

//==============================================================================
void FeaturesComponent::resized()
{
	auto bounds = getLocalBounds();

	//Indent remaining components
	bounds.reduce((bounds.getWidth() / 50), (bounds.getHeight()) / 50);

	auto headingSection = bounds.removeFromTop(bounds.getHeight() / 15);
	headingSection.reduce(headingSection.getWidth() / 60, 0);
	headingLabel.setBounds(headingSection);

	auto topSection = bounds.removeFromTop(bounds.getHeight() / 8);
	auto topSectionLeft = topSection.removeFromLeft(topSection.getWidth() / 1.5f);
	auto topSectionRight = topSection;

	topSectionLeft.reduce(topSectionLeft.getWidth() / 30, 0);
	topSectionRight.reduce(topSectionRight.getWidth() / 30, 0);

	auto bottomSection = bounds.removeFromBottom(bounds.getHeight() / 8);
	auto bottomSectionLeft = bottomSection.removeFromLeft(bottomSection.getWidth() / 1.5f);
	auto bottomSectionRight = bottomSection;

	bottomSectionLeft.reduce(bottomSectionLeft.getWidth() / 30, 0);
	bottomSectionRight.reduce(bottomSectionRight.getWidth() / 30, 0);
	

	auto centerSection = bounds;
	centerSection.reduce(centerSection.getWidth() / 60, 0);

	auto varianceReduceArea = centerSection;
	varianceReduceArea.reduce(0, varianceReduceArea.getHeight() / 8);

	varianceReduceControlsHeadingLabel.setBounds(varianceReduceArea.removeFromTop(varianceReduceArea.getHeight() / 8));

	auto numControls = varianceReduceValueButtons.size();
	for (auto i = 0; i < numControls; ++i)
	{
		auto button = varianceReduceValueButtons[i];
		auto bHeight = varianceReduceArea.getHeight() / (numControls - i);
		auto buttonBounds = varianceReduceArea.removeFromTop(bHeight);

		button->setBounds(buttonBounds.removeFromLeft(buttonBounds.getWidth() / 10));

		auto label = varianceReduceLabels[i];
		label->setBounds(buttonBounds);
	}


	auto numFeaturesInUseArea = topSectionLeft;
	numFeaturesInUseLbl.setBounds(topSectionLeft.removeFromLeft(topSectionLeft.getWidth() / 2));
	numFeaturesInUseVal.setBounds(topSectionLeft);

	showVariancesButton.setBounds(topSectionRight);

	updateFeatureVariancesButton.setBounds(bottomSectionRight);

}

//==============================================================================
void FeaturesComponent::timerCallback()
{
	auto trainingSetReady = processor.getClassifier().checkDataSetReady(AudioClassifyOptions::DataSetType::trainingSet);
	if (trainingSetReady)
		setActive(true);
	else
		setActive(false);

	auto numFeaturesInUse = processor.getClassifier().getNumFeaturesUsed();
	numFeaturesInUseVal.setText(String::formatted("%d", numFeaturesInUse), NotificationType::dontSendNotification);
}

//==============================================================================
void FeaturesComponent::buttonClicked(Button * button)
{
	if (button->getRadioGroupId() == varianceReduceButtonsGroupID)
	{
		auto currentNumFeatures = processor.getClassifier().getNumFeaturesUsed();
		numFeaturesToUse = varianceReduceValueButtons.indexOf(button) * 5;

		if (numFeaturesToUse == 0)
			setNeedsUpdate(currentNumFeatures <= 20);
		else if (numFeaturesToUse != currentNumFeatures)
			setNeedsUpdate(true);
	}
	else if (button->getComponentID() == showVariancesButtonID)
	{
		DialogWindow::LaunchOptions dialog;
		dialog.dialogTitle = "Feature Variances";
		dialog.dialogBackgroundColour = Colours::black;
		dialog.content.setOwned(new FeatureVariancesComponent(processor.getClassifier().getFeatureVariances()));
		dialog.componentToCentreAround = this->getParentComponent();
		dialog.useNativeTitleBar = false;
		dialog.resizable = false;
		dialog.escapeKeyTriggersCloseButton = true;
		dialog.launchAsync();
	}
	else if (button->getComponentID() == updateFeatureVariancesButtonID)
	{
		//Reduce variances	
		processor.getClassifier().reduceFeaturesByVariance(numFeaturesToUse);
		setNeedsUpdate(false);
	}
}

//==============================================================================
void FeaturesComponent::setupVarianceReduceValueButtons()
{
	for (auto i = 0; i < 5; ++i)
	{
		auto value = i * 5;
		varianceReduceValueButtons.add(new ToggleButton());

		auto button = varianceReduceValueButtons[i];
		button->setRadioGroupId(varianceReduceButtonsGroupID);
		button->addListener(this);
		button->setColour(ToggleButton::ColourIds::textColourId, Colours::greenyellow);
		button->setColour(ToggleButton::ColourIds::tickColourId, Colours::greenyellow);

		varianceReduceLabels.add(new Label());

		auto label = varianceReduceLabels[i];

		if (i == 0)
			label->setText("All", NotificationType::dontSendNotification);
		else
			label->setText("Use Top " + String::formatted("%d", i * 5), NotificationType::dontSendNotification);

		label->setFont(Font("Cracked", 14.0f, Font::plain));
		label->setColour(Label::textColourId, Colours::greenyellow);

		addAndMakeVisible(button);
		addAndMakeVisible(label);
	}

	varianceReduceValueButtons[0]->setToggleState(true, NotificationType::dontSendNotification);
}

//==============================================================================
void FeaturesComponent::setNeedsUpdate(bool needsUpdate)
{
	if (needsUpdate)
	{
		updateFeatureVariancesButton.setColour(TextButton::buttonColourId, Colours::greenyellow);
		updateFeatureVariancesButton.setColour(TextButton::textColourOffId, Colours::black);
	}
	else
	{
		updateFeatureVariancesButton.setColour(TextButton::buttonColourId, Colours::black);
		updateFeatureVariancesButton.setColour(TextButton::textColourOffId, Colours::greenyellow);
	}
}

//==============================================================================
void FeaturesComponent::setActive(bool active)
{
	/** The below could be improved upon. This logic would need to be
	*  duplicated for all child components to be switched on / off for active states.
	*  Would be better to subclass Component with another class which acts as an activatable
	*  child component and has an activation header/component with an on/off toggle button and title.
	*
	*	Worth looking into whether this sould be done with composition or inheritance.
	*/
	for (auto i = 0; i < getNumChildComponents(); ++i)
	{
		auto child = getChildComponent(i);
		auto childID = child->getComponentID();

		if (!active)
		{
			if (childID != headingLabelID)
				child->setEnabled(false);
		}
		else
		{
			child->setEnabled(true);
		}
	}
}

//==============================================================================