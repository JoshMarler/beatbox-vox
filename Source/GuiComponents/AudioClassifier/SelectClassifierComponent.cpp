/*
  ==============================================================================

    SelectClassifierComponent.cpp
    Created: 28 Oct 2016 3:13:59pm
    Author:  Joshua.Marler

  ==============================================================================
*/

#include "SelectClassifierComponent.h"

//===============================================================================
String SelectClassifierComponent::classifierCmbBoxID("classifier_cmb");
String SelectClassifierComponent::saveTrainingDataButtonID("saveTraining_btn");
String SelectClassifierComponent::loadTrainingDateButtonID("loadTraining_btn");
String SelectClassifierComponent::testClassifierButtonID("test_classifier_btn");
String SelectClassifierComponent::trainClassifierButtonID("train_classifier_btn");

//===============================================================================
SelectClassifierComponent::SelectClassifierComponent(BeatboxVoxAudioProcessor& p)
	: processor(p),
	  classifierCmbBox("ClassifierSelector"),
	  loadTrainingDataButton("Load Training Data Set"),
	  saveTrainingDataButton("Save Current Training Data"),
	  testClassifierButton("Test"),
	  trainClassifierButton("Train"),
	  testComponent(std::make_unique<TestClassifierComponent>(processor))
{
	//Initialise training sets directory path
	auto path = File::getSpecialLocation(File::SpecialLocationType::userApplicationDataDirectory).getFullPathName()
																									  + "\\" 
																									  + processor.getName()
																									  + "\\TrainingSets";
	trainingSetsDirectory = path.toStdString();


	headingLabel.setText("Select classifier type", NotificationType::dontSendNotification);
	headingLabel.setFont(Font("Cracked", 14.0f, Font::plain));
	headingLabel.setColour(Label::textColourId, Colours::greenyellow);
	addAndMakeVisible(headingLabel);

	setupClassifierCmbBox();

	saveTrainingDataButton.setComponentID(saveTrainingDataButtonID);
    saveTrainingDataButton.addListener(this);
    addAndMakeVisible(saveTrainingDataButton);
    
	loadTrainingDataButton.setComponentID(loadTrainingDateButtonID);
    loadTrainingDataButton.addListener(this);
	addAndMakeVisible(loadTrainingDataButton);

	testClassifierButton.setComponentID(testClassifierButtonID);
	testClassifierButton.addListener(this);
	addAndMakeVisible(testClassifierButton);

	trainClassifierButton.setComponentID(trainClassifierButtonID);
	trainClassifierButton.addListener(this);
	addAndMakeVisible(trainClassifierButton);

	initialiseTrainingDataChooser();
	initialiseSaveDataChooser();


	//setSize(800, 150);
}

//===============================================================================
SelectClassifierComponent::~SelectClassifierComponent()
{
}

//===============================================================================
void SelectClassifierComponent::paint(Graphics & g)
{
	g.setColour(Colours::black.withAlpha(static_cast<uint8>(0xdc)));

	Rectangle<float> fillRect(getWidth(), getHeight());
	g.fillRoundedRectangle(fillRect, 10.0f);
}

//===============================================================================
void SelectClassifierComponent::resized()
{
	auto bounds = getLocalBounds();

	//Indent remaining components
	bounds.reduce((bounds.getWidth() / 50), (bounds.getHeight()) / 50);

	auto boundsLeft = bounds.removeFromLeft(bounds.getWidth() / 2);
	
	auto headingSection = boundsLeft.removeFromTop(boundsLeft.getHeight() / 5);

	headingLabel.setBounds(headingSection);


	auto cmbBoxArea = boundsLeft.removeFromTop(bounds.getHeight() / 2);

	cmbBoxArea.reduce(0, cmbBoxArea.getHeight() / 10);
	classifierCmbBox.setBounds(cmbBoxArea);
	
	auto loadSaveArea = boundsLeft.removeFromBottom(bounds.getHeight() / 2);
	
	loadSaveArea.reduce(0, loadSaveArea.getHeight() / 10);

	auto loadTrainingBounds = loadSaveArea.removeFromLeft(bounds.getWidth() / 3);
	loadTrainingDataButton.setBounds(loadTrainingBounds);

	auto saveTrainingBounds = loadSaveArea.removeFromRight(loadSaveArea.getWidth() / 1.5f);
	saveTrainingDataButton.setBounds(saveTrainingBounds);
	
	auto boundsRight = bounds;
	boundsRight.reduce(boundsRight.getWidth() / 20, 0);

	auto trainClassifierBounds = boundsRight.removeFromLeft(boundsRight.getWidth() / 2);
	trainClassifierBounds.reduce(trainClassifierBounds.getWidth() / 10, trainClassifierBounds.getHeight() / 3);
	trainClassifierButton.setBounds(trainClassifierBounds);

	auto testClassifierBounds = boundsRight;
	testClassifierBounds.reduce(testClassifierBounds.getWidth() / 10, testClassifierBounds.getHeight() / 3);
	testClassifierButton.setBounds(testClassifierBounds);	

}

//===============================================================================
void SelectClassifierComponent::buttonClicked(Button * button)
{
	const auto buttonID = button->getComponentID();

	if (buttonID == saveTrainingDataButtonID)
	{
		auto result = saveDataChooser->browseForFileToSave(false);
		if (result)
		{
			auto fileSaved = saveDataChooser->getResult();
			auto filePath = fileSaved.getFullPathName();
			saveTrainingSet(filePath.toStdString());
		}
	
	}
	else if(buttonID == loadTrainingDateButtonID)
	{
		auto fileSelected = trainingDataChooser->browseForFileToOpen();
		if (fileSelected)
		{
			auto fileChosen = trainingDataChooser->getResult();
			auto filePath = fileChosen.getFullPathName();

			std::string errorString;
			processor.getClassifier().loadTrainingSet(filePath.toStdString(), errorString);
			processor.getClassifier().train();
		}
	}
	else if (buttonID == testClassifierButtonID)
	{
		DialogWindow::LaunchOptions dialog;
		dialog.dialogTitle = "Test Classifier";
		dialog.dialogBackgroundColour = Colours::black;
		dialog.content.setNonOwned(testComponent.get());
		dialog.componentToCentreAround = this->getParentComponent();
		dialog.useNativeTitleBar = false;
		dialog.resizable = false;
		dialog.escapeKeyTriggersCloseButton = true;
		dialog.launchAsync();
	}
}

//===============================================================================
void SelectClassifierComponent::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
	auto id = comboBoxThatHasChanged->getComponentID();

	if (id == classifierCmbBoxID)
	{
		auto& classifier = processor.getClassifier();
		auto classifierType = comboBoxThatHasChanged->getSelectedId() - 1;

		classifier.setClassifierType(static_cast<AudioClassifyOptions::ClassifierType>(classifierType));
	}
}

//===============================================================================
void SelectClassifierComponent::setupClassifierCmbBox()
{
	classifierCmbBox.setComponentID(classifierCmbBoxID);

	//Use AudioClassifyOptions enum in combobox selection
	classifierCmbBox.addItem("Naive Bayes", static_cast<int>(AudioClassifyOptions::ClassifierType::naiveBayes) + 1);
	classifierCmbBox.addItem("K-Nearest Neighbour", static_cast<int>(AudioClassifyOptions::ClassifierType::nearestNeighbour) + 1);

	classifierCmbBox.addListener(this);

	classifierCmbBox.setColour(ComboBox::backgroundColourId, Colours::black);
	classifierCmbBox.setColour(ComboBox::outlineColourId, Colours::greenyellow);
	classifierCmbBox.setColour(ComboBox::textColourId, Colours::greenyellow);
	classifierCmbBox.setColour(ComboBox::arrowColourId, Colours::greenyellow);

	//Set selected item to the current classifier type
	auto currentType = processor.getClassifier().getClassifierType();
	classifierCmbBox.setSelectedId(static_cast<int>(currentType) + 1);

	addAndMakeVisible(classifierCmbBox);
}

//===============================================================================
void SelectClassifierComponent::saveTrainingSet(std::string fileName)
{
	std::string errorstring = "";

	auto successful = processor.getClassifier().saveTrainingSet(fileName, errorstring);

	if (!successful)
	{
		auto icon = AlertWindow::AlertIconType::WarningIcon;
		AlertWindow::showMessageBox(icon, "Error Saving", errorstring, "Close", this);
	}
}

//===============================================================================
void SelectClassifierComponent::initialiseTrainingDataChooser()
{
	File directory(trainingSetsDirectory);

	if (!directory.exists())
		directory.createDirectory();

	trainingDataChooser = std::make_unique<FileChooser>("Load Training Data Set", directory, "*.csv");
}

//===============================================================================
void SelectClassifierComponent::initialiseSaveDataChooser()
{
	File directory(trainingSetsDirectory);

	if (!directory.exists())
		directory.createDirectory();

	saveDataChooser = std::make_unique<FileChooser>("Save Training Data Set", File(directory.getFullPathName() + "\\untitled.csv"), "*.csv");
	
}
