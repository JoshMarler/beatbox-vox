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

//===============================================================================
SelectClassifierComponent::SelectClassifierComponent()
	: classifierCmbBox("ClassifierSelector"),
	  loadTrainingDataButton("Load Training Data Set"),
	  saveTrainingDataButton("Save Current Training Data")
{
	//JWM - Initialize later with directory from plugin
	trainingSetsDirectory = "Test";

	headingLabel.setText("Select classifier type", NotificationType::dontSendNotification);
	headingLabel.setFont(Font("Cracked", 14.0f, Font::plain));
	headingLabel.setColour(Label::textColourId, Colours::greenyellow);
	addAndMakeVisible(headingLabel);

	classifierParamsLabel.setText("Classifier Parameters", NotificationType::dontSendNotification);
	classifierParamsLabel.setFont(Font("Cracked", 14.0f, Font::plain));
	classifierParamsLabel.setColour(Label::textColourId, Colours::greenyellow);
	addAndMakeVisible(classifierParamsLabel);

	setupClassifierCmbBox();

	saveTrainingDataButton.setComponentID(saveTrainingDataButtonID);
	saveTrainingDataButton.setClickingTogglesState (true);
    saveTrainingDataButton.addListener(this);
    addAndMakeVisible(saveTrainingDataButton);
    
	loadTrainingDataButton.setComponentID(loadTrainingDateButtonID);
	loadTrainingDataButton.setClickingTogglesState (true);
    loadTrainingDataButton.addListener(this);
	addAndMakeVisible(loadTrainingDataButton);


	setSize(800, 150);
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
	classifierParamsLabel.setBounds(boundsRight.removeFromTop(boundsRight.getHeight() / 5));

}

//===============================================================================
void SelectClassifierComponent::buttonClicked(Button * button)
{
	//const auto buttonID = button->getComponentID();

	//if (buttonID == saveTrainingDataButtonID)
	//{
	//	saveTrainingSet();
	//}
	//else if(buttonID == loadTrainingDateButtonID)
	//{
	//	auto fileSelected = trainingDataChooser->browseForFileToOpen();
	//	if (fileSelected)
	//	{
	//		auto fileChosen = trainingDataChooser->getResult();
	//		auto filePath = fileChosen.getFullPathName();

	//		std::string errorString;
	//		processor.getClassifier().loadTrainingSet(filePath.toStdString(), errorString);
	//		processor.getClassifier().trainModel();
	//	}
	//}
}

//===============================================================================
void SelectClassifierComponent::comboBoxChanged(ComboBox * comboBOxThatHasChanged)
{
}

//===============================================================================
void SelectClassifierComponent::setupClassifierCmbBox()
{
	//Use AudioClassifyOptions enum in combobox selection
	classifierCmbBox.addItem("Naive Bayes", 1);
	classifierCmbBox.addItem("K-Nearest Neighbour", 2);

	classifierCmbBox.addListener(this);

	classifierCmbBox.setColour(ComboBox::backgroundColourId, Colours::black);
	classifierCmbBox.setColour(ComboBox::outlineColourId, Colours::greenyellow);
	classifierCmbBox.setColour(ComboBox::textColourId, Colours::greenyellow);
	classifierCmbBox.setColour(ComboBox::arrowColourId, Colours::greenyellow);

	classifierCmbBox.setSelectedId(1);
	addAndMakeVisible(classifierCmbBox);
}

//===============================================================================
//void selectclassifiercomponent::savetrainingset()
//{
//	std::string errorstring = "";
//
//	file directory(trainingsetsdirectory);
//
//	if (!directory.exists())
//		directory.createdirectory();
//
//	auto filename = trainingsetsdirectory + "\\" + processor.getname().tostdstring() + "testmatrix.csv";
//	auto successful = processor.getclassifier().savetrainingset(filename, errorstring);
//
//	if (!successful)
//	{
//		auto icon = alertwindow::alerticontype::warningicon;
//		alertwindow::showmessagebox(icon, "error saving", errorstring, "close", this);
//	}
//}

//===============================================================================
//void SelectClassifierComponent::initialiseTrainingDataChooser()
//{
//	File directory(trainingSetsDirectory);
//
//	if (!directory.exists())
//		directory.createDirectory();
//
//	trainingDataChooser = std::make_unique<FileChooser>("Load Training Data Set", directory, "*.csv", false, false);
//
//}


//===============================================================================
//void SelectClassifierComponent::saveTrainingSet()
//{
//	std::string errorString = "";
//
//	File directory(trainingSetsDirectory);
//
//	if (!directory.exists())
//		directory.createDirectory();
//
//	auto fileName = trainingSetsDirectory + "\\" + processor.getName().toStdString() + "TestMatrix.csv";
//	auto successful = processor.getClassifier().saveTrainingSet(fileName, errorString);
//
//	if (!successful)
//	{
//		auto icon = AlertWindow::AlertIconType::WarningIcon;
//		AlertWindow::showMessageBox(icon, "Error Saving", errorString, "Close", this);
//	}
//}

