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
String AudioClassifierComponent::loadTrainingDateButtonID("LoadTrainingDataButton");

AudioClassifierComponent::AudioClassifierComponent(BeatboxVoxAudioProcessor& p)
          : processor(p),
            recordSoundButton(std::make_unique<TextButton> ("Record Training Sound")),
            trainClassifierButton(std::make_unique<TextButton> ("Train Model")),
			saveTrainingDataButton("Save Current Training Data"),
			loadTrainingDataButton("Load Training Data Set")
{

	//Initialise training sets directory path
	auto path = File::getSpecialLocation(File::SpecialLocationType::userApplicationDataDirectory).getFullPathName()
																									  + "\\" 
																									  + processor.getName()
																									  + "\\TrainingSets";
	trainingSetsDirectory = path.toStdString();


	initialiseTrainingDataChooser();

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
    

	loadTrainingDataButton.setComponentID(loadTrainingDateButtonID);
	loadTrainingDataButton.setColour(TextButton::buttonColourId, Colours::white);
    loadTrainingDataButton.addListener(this);
    
    addAndMakeVisible(loadTrainingDataButton);

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
	saveLoadArea.reduce(0, saveLoadArea.getHeight() / 10);
	saveTrainingDataButton.setBounds(saveLoadArea.removeFromLeft(saveLoadArea.getWidth() / 3));
	loadTrainingDataButton.setBounds(saveLoadArea.removeFromRight(saveLoadArea.getWidth() / 2));

	
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
            processor.getClassifier().recordTrainingSample(currentTrainingSound);
			auto label = soundReadyLabels[currentTrainingSound];

			label->setText("Sound " + String(currentTrainingSound + 1) + " Recording Training Set", juce::NotificationType::dontSendNotification);
        }
    }
	else if(button->getComponentID() == loadTrainingDateButtonID)
	{
		auto fileSelected = trainingDataChooser->browseForFileToOpen();
		if (fileSelected)
		{
			auto fileChosen = trainingDataChooser->getResult();
			auto filePath = fileChosen.getFullPathName();

			std::string errorString;
			processor.getClassifier().loadTrainingSet(filePath.toStdString(), errorString);

			//JWM - added as test - may remove
			processor.getClassifier().trainModel();
		}
	}

}


//===============================================================================
void AudioClassifierComponent::saveTrainingSet()
{
	std::string errorString = "";

	File directory(trainingSetsDirectory);

	if (!directory.exists())
		directory.createDirectory();

	auto fileName = trainingSetsDirectory + "\\" + processor.getName().toStdString() + "TestMatrix.csv";
	auto successful = processor.getClassifier().saveTrainingSet(fileName, errorString);

	if (!successful)
	{
		auto icon = AlertWindow::AlertIconType::WarningIcon;
		AlertWindow::showMessageBox(icon, "Error Saving", errorString, "Close", this);
	}
}

//===============================================================================
void AudioClassifierComponent::initialiseTrainingDataChooser()
{
	File directory(trainingSetsDirectory);

	if (!directory.exists())
		directory.createDirectory();
	
	trainingDataChooser = std::make_unique<FileChooser>("Load Training Data Set", directory, "*.csv", false, false);
}

//===============================================================================
