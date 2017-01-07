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
String SelectClassifierComponent::loadTrainingDateButtonID("loadTraining_btn");
String SelectClassifierComponent::numNeighboursSliderID("numNeighbours_sld");
String SelectClassifierComponent::testClassifierButtonID("test_classifier_btn");
String SelectClassifierComponent::trainClassifierButtonID("train_classifier_btn");

//===============================================================================
SelectClassifierComponent::SelectClassifierComponent(BeatboxVoxAudioProcessor& p)
	: processor(p),
	  classifierCmbBox("ClassifierSelector"),
	  loadTrainingDataButton("Load Training Data Set"),
	  testClassifierButton("Test"),
	  trainClassifierButton("Train")
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

    
	loadTrainingDataButton.setComponentID(loadTrainingDateButtonID);
    loadTrainingDataButton.addListener(this);
	addAndMakeVisible(loadTrainingDataButton);

	numNeighboursLabel.setText("KNN Num Neighbours", NotificationType::dontSendNotification);
	numNeighboursLabel.setFont(Font("Cracked", 14.0f, Font::plain));
	numNeighboursLabel.setColour(Label::textColourId, Colours::greenyellow);
	numNeighboursLabel.setEnabled(false);
	addAndMakeVisible(numNeighboursLabel);

	numNeighboursSlider.setComponentID(numNeighboursSliderID);
	numNeighboursSlider.setSliderStyle (Slider::IncDecButtons);
	numNeighboursSlider.setRange (3.0, 11.0, 2.0);
	numNeighboursSlider.setIncDecButtonsMode (Slider::incDecButtonsDraggable_Horizontal);
	numNeighboursSlider.setTextBoxStyle(Slider::TextBoxRight, true, 90, 20);
	numNeighboursSlider.setColour(Slider::textBoxBackgroundColourId, Colours::black);
	numNeighboursSlider.setColour(Slider::textBoxTextColourId, Colours::greenyellow);
	numNeighboursSlider.setColour(Slider::textBoxOutlineColourId, Colours::black);
	numNeighboursSlider.addListener(this);
	numNeighboursSlider.setValue(processor.getClassifier().getKNNNumNeighbours(), NotificationType::dontSendNotification);
	numNeighboursSlider.setEnabled(false);
	addAndMakeVisible (numNeighboursSlider);

	auto classifierType = processor.getClassifier().getClassifierType();
	if (classifierType == AudioClassifyOptions::ClassifierType::nearestNeighbour)
	{
		numNeighboursLabel.setEnabled(true);
		numNeighboursSlider.setEnabled(true);
	}

	testClassifierButton.setComponentID(testClassifierButtonID);
	testClassifierButton.addListener(this);
	addAndMakeVisible(testClassifierButton);

	trainClassifierButton.setComponentID(trainClassifierButtonID);
	trainClassifierButton.addListener(this);
	trainClassifierButton.setEnabled(false);
	addAndMakeVisible(trainClassifierButton);

	initialiseTrainingDataChooser();

	startTimerHz(20);
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
	
	auto loadArea = boundsLeft.removeFromBottom(bounds.getHeight() / 2);
	
	loadArea.reduce(0, loadArea.getHeight() / 10);

	auto loadTrainingBounds = loadArea.removeFromLeft(bounds.getWidth() / 3);
	loadTrainingDataButton.setBounds(loadTrainingBounds);
	
	auto boundsRight = bounds;
	boundsRight.reduce(boundsRight.getWidth() / 20, 0);

	auto rightTopArea = boundsRight.removeFromBottom(boundsRight.getHeight() / 2);
	rightTopArea.reduce(0, rightTopArea.getHeight() / 8);

	auto trainClassifierBounds = rightTopArea.removeFromLeft(boundsRight.getWidth() / 2);
	trainClassifierBounds.reduce(trainClassifierBounds.getWidth() / 10, 0);
	trainClassifierButton.setBounds(trainClassifierBounds);

	auto testClassifierBounds = rightTopArea;
	testClassifierBounds.reduce(testClassifierBounds.getWidth() / 10, 0);
	testClassifierButton.setBounds(testClassifierBounds);

	auto rightBottomArea = boundsRight;
	rightBottomArea.reduce(rightBottomArea.getWidth() / 26, 0);

	auto neighboursBounds = rightBottomArea.removeFromBottom(rightBottomArea.getHeight() / 1.5f);
	numNeighboursLabel.setBounds(neighboursBounds.removeFromTop(neighboursBounds.getHeight() / 4));

	neighboursBounds.reduce(0, neighboursBounds.getHeight() / 10);
	numNeighboursSlider.setBounds(neighboursBounds);

}

//===============================================================================
void SelectClassifierComponent::timerCallback()
{
	auto trainingSetReady = processor.getClassifier().checkDataSetReady(AudioClassifyOptions::DataSetType::trainingSet);
	auto classifierReady = processor.getClassifier().getClassifierReady();

	if (trainingSetReady)
	{
		trainClassifierButton.setEnabled(true);

		if (!classifierReady)
		{
			trainClassifierButton.setColour(TextButton::buttonColourId, Colours::greenyellow);
			trainClassifierButton.setColour(TextButton::textColourOffId, Colours::black);
			testClassifierButton.setEnabled(false);
		}
		else
		{
			trainClassifierButton.setColour(TextButton::buttonColourId, Colours::black);
			trainClassifierButton.setColour(TextButton::textColourOffId, Colours::greenyellow);
			testClassifierButton.setEnabled(true);
		}
	}
	else
		trainClassifierButton.setEnabled(false);
}

//===============================================================================
void SelectClassifierComponent::buttonClicked(Button * button)
{
	const auto buttonID = button->getComponentID();

	if(buttonID == loadTrainingDateButtonID)
	{
		auto fileSelected = trainingDataChooser->browseForFileToOpen();
		if (fileSelected)
		{
			auto fileChosen = trainingDataChooser->getResult();
			auto filePath = fileChosen.getFullPathName();

			std::string errorString;
			auto success = processor.getClassifier().loadDataSet(filePath.toStdString(), AudioClassifyOptions::DataSetType::trainingSet ,errorString);
			if (!success)
			{
				auto icon = AlertWindow::AlertIconType::WarningIcon;
				AlertWindow::showMessageBox(icon, "Error Loading", errorString, "Close", this);
			}
			else
				sendChangeMessage();
		}
	}
	else if (buttonID == trainClassifierButtonID)
	{
		processor.getClassifier().train();
	}
	else if (buttonID == testClassifierButtonID)
	{
		testComponent.reset(new TestClassifierComponent(processor));

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
		auto classifierType = static_cast<AudioClassifyOptions::ClassifierType>(comboBoxThatHasChanged->getSelectedId() - 1);

		classifier.setClassifierType(classifierType);

		if (classifierType == AudioClassifyOptions::ClassifierType::nearestNeighbour)
		{
			numNeighboursLabel.setEnabled(true);
			numNeighboursSlider.setEnabled(true);
		}
	}
}

//===============================================================================
void SelectClassifierComponent::sliderValueChanged(Slider * slider)
{
	if (slider->getComponentID() == numNeighboursSliderID)
	{
		auto numNeighbours = static_cast<int>(slider->getValue());
		processor.getClassifier().setKNNNumNeighbours(numNeighbours);
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
void SelectClassifierComponent::initialiseTrainingDataChooser()
{
	File directory(trainingSetsDirectory);

	if (!directory.exists())
		directory.createDirectory();

	trainingDataChooser = std::make_unique<FileChooser>("Load Training Data Set", directory, "*.bin");
}

//===============================================================================
