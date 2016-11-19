/*
  ==============================================================================

    RecordTrainingSetComponent.cpp
    Created: 21 Oct 2016 3:23:25pm
    Author:  Joshua.Marler

  ==============================================================================
*/

#include "RecordTrainingSetComponent.h"

String RecordTrainingSetComponent::headingLabelID("heading_lbl");
String RecordTrainingSetComponent::activateButtonID("activate_btn");
String RecordTrainingSetComponent::instanceSizeSliderID("instance_size_sld");
String RecordTrainingSetComponent::instanceSizeButtonID("instance_size_btn");
String RecordTrainingSetComponent::recordButtonID("record_btn");
String RecordTrainingSetComponent::trainButtonID("train_btn");
String RecordTrainingSetComponent::recordTypeCmbID("record_type_cmb");

//===============================================================================
RecordTrainingSetComponent::RecordTrainingSetComponent(BeatboxVoxAudioProcessor& p)
	: processor(p)
{
	activateButton.setComponentID(activateButtonID);
	activateButton.addListener(this);
	addAndMakeVisible(activateButton);

	headingLabel.setComponentID(headingLabelID);
	headingLabel.setText("Record Training Set", NotificationType::dontSendNotification);
	headingLabel.setFont(Font("Cracked", 14.0f, Font::plain));
	headingLabel.setColour(Label::textColourId, Colours::greenyellow);
	addAndMakeVisible(headingLabel);

	numInstancesLabel.setText("Num instances per sound", NotificationType::dontSendNotification);
	numInstancesLabel.setFont(Font("Cracked", 14.0f, Font::plain));
	numInstancesLabel.setColour(Label::textColourId, Colours::greenyellow);
	addAndMakeVisible(numInstancesLabel);

	instanceSizeSlider.setComponentID(instanceSizeSliderID);
	instanceSizeSlider.setSliderStyle (Slider::IncDecButtons);
	instanceSizeSlider.setRange (10.0, 40.0, 1.0);
	instanceSizeSlider.setIncDecButtonsMode (Slider::incDecButtonsDraggable_Horizontal);
	instanceSizeSlider.setTextBoxStyle(Slider::TextBoxRight, true, 90, 20);
	instanceSizeSlider.setColour(Slider::textBoxBackgroundColourId, Colours::black);
	instanceSizeSlider.setColour(Slider::textBoxTextColourId, Colours::greenyellow);
	instanceSizeSlider.setColour(Slider::textBoxOutlineColourId, Colours::black);
	instanceSizeSlider.addListener(this);
	addAndMakeVisible (instanceSizeSlider);

	instanceSizeButton.setComponentID(instanceSizeButtonID);
	instanceSizeButton.setButtonText("Update");
	instanceSizeButton.addListener(this);
	//instanceSizeButton.setEnabled(false);
	addAndMakeVisible(instanceSizeButton);

	selectRecordingSoundLabel.setText("Select sound to record", NotificationType::dontSendNotification);
	selectRecordingSoundLabel.setFont(Font("Cracked", 14.0f, Font::plain));
	selectRecordingSoundLabel.setColour(Label::textColourId, Colours::greenyellow);
	addAndMakeVisible(selectRecordingSoundLabel);

	recordButton.setComponentID(recordButtonID);
	recordButton.setButtonText("Record Training Sound");
	recordButton.setClickingTogglesState (true);
	recordButton.addListener(this);
	addAndMakeVisible(recordButton);

	trainButton.setComponentID(trainButtonID);
	trainButton.setButtonText("Train Classifier");
	trainButton.setClickingTogglesState (true);
	trainButton.addListener(this);
	addAndMakeVisible(trainButton);

	setupSoundButtons();
	setupRecordTypeCmb();

	activateButton.setToggleState(false, NotificationType::sendNotification);
	setActive(false);

	startTimerHz(20);
}

//===============================================================================
RecordTrainingSetComponent::~RecordTrainingSetComponent()
{
}

//===============================================================================
void RecordTrainingSetComponent::paint(Graphics & g)
{
	g.setColour(Colours::black.withAlpha(static_cast<uint8>(0xdc)));

	Rectangle<float> fillRect(getWidth(), getHeight());
	g.fillRoundedRectangle(fillRect, 10.0f);
}

//===============================================================================
void RecordTrainingSetComponent::resized()
{
	auto bounds = getLocalBounds();

	//Indent remaining components
	bounds.reduce((bounds.getWidth() / 50), (bounds.getHeight()) / 50);

	auto headingSection = bounds.removeFromTop(bounds.getHeight() / 15);
	activateButton.setBounds(headingSection.removeFromLeft(headingSection.getWidth() / 14));
	headingLabel.setBounds(headingSection);

	auto comboBoxArea = bounds.removeFromTop(bounds.getHeight() / 5);
	comboBoxArea.reduce(0, comboBoxArea.getHeight() / 4);
	recordTypeCmb.setBounds(comboBoxArea);
	
	auto instanceSizeArea = bounds.removeFromTop(bounds.getHeight() / 3);
	auto instanceSliderArea = instanceSizeArea.removeFromLeft(bounds.getWidth() / 2);
	instanceSliderArea.reduce(0, instanceSliderArea.getHeight() / 3.5f);
	numInstancesLabel.setBounds(instanceSliderArea.removeFromTop(instanceSliderArea.getHeight() / 2));
	instanceSizeSlider.setBounds(instanceSliderArea);

	auto instanceButtonArea = instanceSizeArea;
	instanceButtonArea.reduce(instanceButtonArea.getWidth() / 5, instanceButtonArea.getHeight() / 5);
	instanceSizeButton.setBounds(instanceButtonArea.removeFromBottom(instanceButtonArea.getHeight() / 1.5f));

	auto numSounds = processor.getClassifier().getNumSounds();
	auto soundsArea = bounds.removeFromTop(bounds.getHeight() / 1.5f);

	selectRecordingSoundLabel.setBounds(soundsArea.removeFromTop(soundsArea.getHeight() / 8));

	for (auto i = 0; i < numSounds; ++i)
	{
		auto button = soundButtons[i];
		auto bHeight = soundsArea.getHeight() / (numSounds - i);
		auto buttonBounds = soundsArea.removeFromTop(bHeight);
		button->setBounds(buttonBounds.removeFromLeft(buttonBounds.getWidth() / 4));

		auto label = soundStatusLabels[i];
		label->setBounds(buttonBounds);
	} 

	auto btnBottomArea = bounds.removeFromBottom(bounds.getHeight() / 2);
	auto recordButtonBounds = btnBottomArea.removeFromLeft(bounds.getWidth() / 3);
	recordButton.setBounds(recordButtonBounds);

	auto trainButtonBounds = btnBottomArea.removeFromRight(btnBottomArea.getWidth() / 2);
	trainButton.setBounds(trainButtonBounds);	

}

//===============================================================================
void RecordTrainingSetComponent::timerCallback()
{
	 auto isRecording = processor.getClassifier().isRecording();
     auto trainingSetReady = processor.getClassifier().checkTrainingSetReady();
     auto classifierReady = processor.getClassifier().getClassifierReady();
	 auto numSounds = processor.getClassifier().getNumSounds();

     if (trainingSetReady)
         trainButton.setEnabled(true);
	 else 
		 trainButton.setEnabled(false);

     if (trainButton.getToggleState())
     {
        if (classifierReady)
            trainButton.setToggleState(false, NotificationType::dontSendNotification);
     }
         
     //JWM - A little sketchy way of doing things maybe but works for prototype stage. 
     if (recordButton.getToggleState())
     {
        if (isRecording == false)
           recordButton.setToggleState(false, NotificationType::dontSendNotification);
     }

	 //Update individual sounds ready labels
	 for (auto i = 0; i < numSounds; i++)
	 {
		 auto currentSoundRecording = processor.getClassifier().getCurrentSoundRecording();
		 auto soundReady = false;

		 switch (currentDataSetType)
		 {
			 case AudioClassifyOptions::DataSetType::trainingSet:
				 soundReady = processor.getClassifier().checkTrainingSoundReady(i);
				 break;
			 case AudioClassifyOptions::DataSetType::testSet:
				 soundReady = processor.getClassifier().checkTestSoundReady(i);
				 break;
			 default:
				 break;
		 }

		 auto label = soundStatusLabels[i];
		 String soundName;

		 switch (i)
		 {
				case BeatboxVoxAudioProcessor::soundLabel::KickDrum :
						soundName = "Kick";
						break;
				case BeatboxVoxAudioProcessor::soundLabel::SnareDrum :
						soundName = "Snare";
						break;
				case BeatboxVoxAudioProcessor::soundLabel::HiHat : 
						soundName = "HiHat";
						break;
				default: break;
		 }

		 if (soundReady)
		 {
			 label->setColour(Label::textColourId, Colours::greenyellow);
			 label->setText(soundName + " Ready", juce::NotificationType::dontSendNotification);
		 }
		 else if (soundReady != currentSoundRecording)
		 {
			label->setText(soundName + " - Not ready", juce::NotificationType::dontSendNotification);
			label->setColour(Label::textColourId, Colours::greenyellow.withAlpha(static_cast<uint8>(0x4a)));
		 }
	 }
}

//===============================================================================
void RecordTrainingSetComponent::buttonClicked(Button * button)
{
	const auto id = button->getComponentID();

	if (id == activateButtonID)
	{
		setActive(button->getToggleState());
	}
	else if (id == instanceSizeButtonID)
	{
		auto numInstances = static_cast<int>(instanceSizeSlider.getValue());
		
		switch (currentDataSetType)
		{
			case AudioClassifyOptions::DataSetType::trainingSet:
				processor.getClassifier().setTrainingInstancesPerSound(numInstances);
				break;
			case AudioClassifyOptions::DataSetType::testSet:
				processor.getClassifier().setTestInstancesPerSound(numInstances);
				break;
			default:
				break;
		}
		
		setNeedsUpdate(false);
	}
	else if(id == recordButtonID)
	{
		if (button->getToggleState())
		{
			
			switch (currentDataSetType)
			{
				case AudioClassifyOptions::DataSetType::trainingSet:
					processor.getClassifier().recordTrainingData(currentRecordingSound);
					break;
				case AudioClassifyOptions::DataSetType::testSet:
					processor.getClassifier().recordTestData(currentRecordingSound);
					break;
				default:
					break;
			}
			
			auto label = soundStatusLabels[currentRecordingSound];
			String soundName;

			switch (currentRecordingSound)
			{
				case BeatboxVoxAudioProcessor::soundLabel::KickDrum :
						soundName = "Kick";
						break;
				case BeatboxVoxAudioProcessor::soundLabel::SnareDrum :
						soundName = "Snare";
						break;
				case BeatboxVoxAudioProcessor::soundLabel::HiHat : 
						soundName = "HiHat";
						break;
				default: break;
			}

			label->setText( soundName + " - Recording instances", juce::NotificationType::dontSendNotification);
		}	
	}
	else if (id == trainButtonID)
	{
		 if (button->getToggleState())
				processor.getClassifier().train();	
	}
	else if (button->getRadioGroupId() == soundButtonsGroupID)
	{
		if (button->getToggleState())
        {
            currentRecordingSound = soundButtons.indexOf(button);
        }		
	}
}

//===============================================================================
void RecordTrainingSetComponent::sliderValueChanged(Slider * slider)
{

	auto id = slider->getComponentID();

	if (id == instanceSizeSliderID)
	{
		auto currentVal = 0;

		switch (currentDataSetType)
		{
		case AudioClassifyOptions::DataSetType::trainingSet:
			currentVal = processor.getClassifier().getTrainingInstancesPerSound();
			break;
		case AudioClassifyOptions::DataSetType::testSet:
			currentVal = processor.getClassifier().getTestInstancesPerSound();
			break;
		default:
			break;
		}

		auto newVal = static_cast<int>(slider->getValue());

		if (newVal != currentVal)
			setNeedsUpdate(true);
		else
			setNeedsUpdate(false);
	}
}

void RecordTrainingSetComponent::comboBoxChanged(ComboBox * comboBoxThatHasChanged)
{
	auto id = comboBoxThatHasChanged->getComponentID();

	if (id == recordTypeCmbID)
	{
		auto dataSet = comboBoxThatHasChanged->getSelectedId() - 1;

		switch (dataSet)
		{
			case static_cast<int>(AudioClassifyOptions::DataSetType::trainingSet) :
				currentDataSetType = AudioClassifyOptions::DataSetType::trainingSet;
				break;

			case static_cast<int>(AudioClassifyOptions::DataSetType::testSet) :
				currentDataSetType = AudioClassifyOptions::DataSetType::testSet;
				break;

			default:
				break;
		}
	}
}

//===============================================================================
void RecordTrainingSetComponent::setupSoundButtons()
{
	//Replace with classifier.getNumSounds() later
	auto numSounds = processor.getClassifier().getNumSounds();

    for (auto i = 0; i < numSounds; ++i)
    {
		String soundName;

    	switch (i)
    	{
			case BeatboxVoxAudioProcessor::soundLabel::KickDrum :
					soundName = "Kick";
					break;
			case BeatboxVoxAudioProcessor::soundLabel::SnareDrum :
					soundName = "Snare";
					break;
			case BeatboxVoxAudioProcessor::soundLabel::HiHat : 
					soundName = "HiHat";
					break;
			default: break;
	    }

        soundButtons.add(new ToggleButton(soundName));

        auto button = soundButtons[i];
        button->setRadioGroupId(soundButtonsGroupID);
        button->addListener(this);
        button->setColour(ToggleButton::ColourIds::textColourId, Colours::greenyellow);
        button->setColour(ToggleButton::ColourIds::tickColourId, Colours::greenyellow);

		soundStatusLabels.add(new Label());

		auto label = soundStatusLabels[i];
		label->setText(soundName + " - Not ready", juce::NotificationType::dontSendNotification);
		label->setFont(Font("Cracked", 14.0f, Font::plain));
		label->setColour(Label::textColourId, Colours::greenyellow.withAlpha(static_cast<uint8>(0x4a)));
		
        addAndMakeVisible(button);
		addAndMakeVisible(label);
    }
}

//===============================================================================
void RecordTrainingSetComponent::setActive(bool active)
{
	/** The below could be improved upon. This logic would need to be 
	 *  duplicated for all child components to be switched on / off for active states.
	 *  Would be better so subclass Component with another class which acts as an activatable 
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
			if (childID != activateButtonID && childID != headingLabelID) 
				child->setEnabled(false);
		}
		else
		{
			child->setEnabled(true);
		}
	}
}

//===============================================================================
void RecordTrainingSetComponent::setNeedsUpdate(bool needsUpdate)
{
	if (needsUpdate)
	{
		instanceSizeButton.setColour(TextButton::buttonColourId, Colours::greenyellow);
		instanceSizeButton.setColour(TextButton::textColourOffId, Colours::black);
	}
	else
	{
		instanceSizeButton.setColour(TextButton::buttonColourId, Colours::black);
		instanceSizeButton.setColour(TextButton::textColourOffId, Colours::greenyellow);
	}

}

//===============================================================================
void RecordTrainingSetComponent::setupRecordTypeCmb()
{
	//Use AudioClassifyOptions enum in combobox selection
	recordTypeCmb.addItem("Record Training Instances", static_cast<int>(AudioClassifyOptions::DataSetType::trainingSet) + 1);
	recordTypeCmb.addItem("Record Test Instances", static_cast<int>(AudioClassifyOptions::DataSetType::testSet) + 1);

	recordTypeCmb.addListener(this);

	recordTypeCmb.setColour(ComboBox::backgroundColourId, Colours::black);
	recordTypeCmb.setColour(ComboBox::outlineColourId, Colours::greenyellow);
	recordTypeCmb.setColour(ComboBox::textColourId, Colours::greenyellow);
	recordTypeCmb.setColour(ComboBox::arrowColourId, Colours::greenyellow);

	recordTypeCmb.setSelectedId(1);

	currentDataSetType = AudioClassifyOptions::DataSetType::trainingSet;

	addAndMakeVisible(recordTypeCmb);
}