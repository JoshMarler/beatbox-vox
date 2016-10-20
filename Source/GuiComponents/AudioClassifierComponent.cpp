/*
  ==============================================================================

    AudioClassifierComponent.cpp
    Created: 17 Sep 2016 5:13:37pm
    Author:  joshua

  ==============================================================================
*/

#include "AudioClassifierComponent.h"

//===============================================================================

String AudioClassifierComponent::saveTrainingDataButtonID("SaveTrainingDataButton");

AudioClassifierComponent::AudioClassifierComponent(BeatboxVoxAudioProcessor& p)
          : processor(p),
            recordSoundButton(std::make_unique<TextButton> ("Record Training Sound")),
            trainClassifierButton(std::make_unique<TextButton> ("Train Model")),
			saveTrainingDataButton("Save Current Training Data")
{
    trainClassifierButton->setClickingTogglesState(true);
    trainClassifierButton->setColour(TextButton::buttonColourId, Colours::white);
    trainClassifierButton->setColour(TextButton::buttonOnColourId, Colours::greenyellow);
    trainClassifierButton->setEnabled(false);
    trainClassifierButton->addListener(this);
    
    addAndMakeVisible(*trainClassifierButton);

    recordSoundButton->setClickingTogglesState(true);
    recordSoundButton->setColour(TextButton::buttonColourId, Colours::white);
    recordSoundButton->setColour(TextButton::buttonOnColourId, Colours::greenyellow);
    recordSoundButton->addListener(this);
    
    addAndMakeVisible(*recordSoundButton);


	saveTrainingDataButton.setComponentID(saveTrainingDataButtonID);
    saveTrainingDataButton.setColour(TextButton::buttonColourId, Colours::white);
    saveTrainingDataButton.addListener(this);
    
    addAndMakeVisible(saveTrainingDataButton);
    

    auto numSounds = processor.getClassifier().getNumSounds();

    for (auto i = 0; i < numSounds; ++i)
    {
        soundButtons.add(new ToggleButton("Sound " + String(i + 1) + " Button"));

        auto button = soundButtons[i];
        button->setRadioGroupId(1234);
        button->addListener(this);
        button->setColour(ToggleButton::ColourIds::textColourId, Colours::greenyellow);
        button->setColour(ToggleButton::ColourIds::tickColourId, Colours::greenyellow);


		soundReadyLabels.add(new Label());

		auto label = soundReadyLabels[i];
		label->setText("Sound " + String(i + 1) + " - Not Ready", juce::NotificationType::dontSendNotification);
		label->setFont(Font("Cracked", 14.0f, Font::plain));
		label->setColour(Label::textColourId, Colours::greenyellow.withAlpha(static_cast<uint8>(0x4a)));
		
        addAndMakeVisible(button);
		addAndMakeVisible(label);
    }
	
    //Set timer for gui update callback
    startTimerHz(20);

}

//===============================================================================
AudioClassifierComponent::~AudioClassifierComponent()
{

}

//===============================================================================
void AudioClassifierComponent::paint(Graphics& g)
{
    g.fillAll (Colours::transparentBlack);
}

//===============================================================================
void AudioClassifierComponent::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
	auto r (getLocalBounds().reduced(15));

    recordSoundButton->setBounds(r.removeFromTop(25));


	auto saveLoadArea(r.removeFromTop(r.getHeight() / 8));
	saveTrainingDataButton.setBounds(saveLoadArea.removeFromLeft(saveLoadArea.getWidth() / 3));

	
	auto numSounds = processor.getClassifier().getNumSounds();

    for (auto i = 0; i < numSounds; ++i)
    {
        auto button = soundButtons[i];
		auto buttonBounds = r.removeFromTop(75 + (10 * i));
        button->setBounds(buttonBounds.removeFromLeft(buttonBounds.getWidth() / 4));

		auto label = soundReadyLabels[i];
		label->setBounds(buttonBounds);
    }

    trainClassifierButton->setBounds(r.removeFromBottom(15));
}

//===============================================================================
void AudioClassifierComponent::timerCallback()
{
     auto isTraining = processor.getClassifier().isTraining();
     auto trainingSetReady = processor.getClassifier().checkTrainingSetReady();
     auto classifierReady = processor.getClassifier().getClassifierReady();
	 auto numSounds = processor.getClassifier().getNumSounds();

     if (trainingSetReady)
         trainClassifierButton->setEnabled(true);

     if (trainClassifierButton->getToggleState())
     {
        if (classifierReady)
            trainClassifierButton->setToggleState(false, NotificationType::dontSendNotification);
     }
         
     //JWM - A little sketchy way of doing things maybe but works for prototype stage. 
     if (recordSoundButton->getToggleState())
     {
        if (isTraining == false)
           recordSoundButton->setToggleState(false, NotificationType::dontSendNotification);
     }

	 //Update individual sounds ready labels
	 for (auto i = 0; i < numSounds; i++)
	 {
		 auto soundReady = processor.getClassifier().checkTrainingSoundReady(i);
		 auto label = soundReadyLabels[i];

		 if (soundReady)
		 {
			 label->setColour(Label::textColourId, Colours::greenyellow);
			 label->setText("Sound " + String(i + 1) + " Ready", juce::NotificationType::dontSendNotification);
		 }
	 }
	 
}

//===============================================================================
void AudioClassifierComponent::buttonClicked(Button* button)
{

    if (button->getRadioGroupId() == 1234)
    {
        if (button->getToggleState())
        {
            currentTrainingSound = soundButtons.indexOf(button);
        }
    }
	else if (button->getComponentID() == saveTrainingDataButtonID)
	{
		saveTrainingSet();
	}
    else if (button == std::addressof(*trainClassifierButton))
    {
        if (button->getToggleState())
            processor.getClassifier().trainModel();    
    }
    else if (button == std::addressof(*recordSoundButton))
    {
        if (button->getToggleState())
        {
			/** JWM - NOTE:
			 *  At the moment this code is synchronous. So on the call to recordTrainingSample
			 *  we could display a "Training" progress bar / spinner and then have the classifiers
			 *  recordTrainingSample return true/false based on whether the sound was recorded and added
			 *  to the training set succesfully. 
			 *  Then the spinner/progress bar could be hidden and the currently selected radio button unticked for GUI/Display
			 *  purposes. 
			 *  Would then be worth prompting the user if they go to record the same sound again to check if this is
			 *  overwriting the previously recorded training/model data or whether it is being added as additional training data to the 
			 *  model. This would require a call to setTrainingSetSize or something similar. Training data would need to be stored to 
			 *  then be appended to and the model probably re-trained. Alternativley incremental training could be implemented in 
			 *  a similar fashion to the mlpack code. 
			 */ 
            processor.getClassifier().recordTrainingSample(currentTrainingSound);
			auto label = soundReadyLabels[currentTrainingSound];

			label->setText("Sound " + String(currentTrainingSound + 1) + " Recording Training Set", juce::NotificationType::dontSendNotification);
        }
    }

}

//===============================================================================
void AudioClassifierComponent::saveTrainingSet()
{
	std::string errorString = "";

	//JWM _ Note: can probably replace this directory path with compile time constance or something ? 
	auto pathName = File::getSpecialLocation(File::SpecialLocationType::userApplicationDataDirectory).getFullPathName()
																									  + "\\" 
																									  + processor.getName();
	File modelsDirectory(pathName);

	if (!modelsDirectory.exists())
		modelsDirectory.createDirectory();

	auto fileName = modelsDirectory.getFullPathName().toStdString() + "\\" + processor.getName().toStdString() + "TestMatrix.csv";
	auto successful = processor.getClassifier().saveTrainingSet(fileName, errorString);
}

//===============================================================================
