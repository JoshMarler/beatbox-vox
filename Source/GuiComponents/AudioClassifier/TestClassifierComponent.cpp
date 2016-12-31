/*
  ==============================================================================

    TestClassifierComponent.cpp
    Created: 12 Nov 2016 10:21:28pm
    Author:  Joshua Marler

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "TestClassifierComponent.h"

//==============================================================================
String TestClassifierComponent::runTestButtonID("run_test_btn");
String TestClassifierComponent::loadTestSetButtonID("load_test_set_btn");

//==============================================================================
TestClassifierComponent::TestClassifierComponent(BeatboxVoxAudioProcessor& p)
	: processor(p),
	  componentLookAndFeel(std::make_unique<CustomLookAndFeel>()),
	  runTestButton("Run Test"),
	  loadTestSetButton("Load Test Set"),
	  table("Test Results Table", this)
{
	setLookAndFeel(componentLookAndFeel.get());

	componentLookAndFeel->setColour (TextButton::buttonColourId, Colours::black);
    componentLookAndFeel->setColour (TextButton::textColourOffId, Colours::greenyellow);

    componentLookAndFeel->setColour (TextButton::buttonOnColourId, componentLookAndFeel->findColour (TextButton::textColourOffId));
    componentLookAndFeel->setColour (TextButton::textColourOnId, componentLookAndFeel->findColour (TextButton::buttonColourId));	


	runTestButton.setComponentID(runTestButtonID);
	runTestButton.addListener(this);
	addAndMakeVisible(runTestButton);

	loadTestSetButton.setComponentID(loadTestSetButtonID);
	loadTestSetButton.addListener(this);
	addAndMakeVisible(loadTestSetButton);

	auto ready = processor.getClassifier().checkDataSetReady(AudioClassifyOptions::DataSetType::testSet);

	if (ready)
		testSetStatusLabel.setText("Test set status: Ready", NotificationType::dontSendNotification);
	else
		testSetStatusLabel.setText("Test set status: Not Available", NotificationType::dontSendNotification);

	testSetStatusLabel.setFont(Font("Cracked", 20.0f, Font::plain));
	testSetStatusLabel.setColour(Label::textColourId, Colours::greenyellow);
	addAndMakeVisible(testSetStatusLabel);


	accuracyLabel.setText("Accuracy: ", NotificationType::dontSendNotification);
	accuracyLabel.setFont(Font("Cracked", 20.0f, Font::plain));
	accuracyLabel.setColour(Label::textColourId, Colours::greenyellow);
	addAndMakeVisible(accuracyLabel);


	auto numTestResults = processor.getClassifier().getDataSetSize(AudioClassifyOptions::DataSetType::testSet);
	
	//Set current test results to uninitialized state
	currentTestResults.resize(numTestResults, std::make_pair(-1, -1));

	//Set up test results table
	table.setColour(ListBox::ColourIds::outlineColourId, Colours::greenyellow);
	table.setColour(ListBox::ColourIds::backgroundColourId, Colours::black);
	table.setColour(ListBox::ColourIds::textColourId, Colours::greenyellow);
	table.setOutlineThickness(1);

	table.getHeader().addColumn("Test Instance", 1, 40);
	table.getHeader().addColumn("Actual Class", 2, 40);
	table.getHeader().addColumn("Predicted Class", 3, 40);

	table.getHeader().setStretchToFitActive(true);

	addAndMakeVisible(table);

	setSize(550, 500);
}

//===============================================================================
TestClassifierComponent::~TestClassifierComponent()
{
}

//===============================================================================
void TestClassifierComponent::paint (Graphics& g)
{
	g.setColour(Colours::black.withAlpha(static_cast<uint8>(0xdc)));

	Rectangle<float> fillRect(getWidth(), getHeight());
	g.fillRoundedRectangle(fillRect, 10.0f);
}

//===============================================================================
void TestClassifierComponent::resized()
{
	auto bounds = getLocalBounds();

	auto testControlsBounds = bounds.removeFromTop(bounds.getHeight() / 5);
	auto testControlsBoundsLeft = testControlsBounds.removeFromLeft(testControlsBounds.getWidth() / 2);
	auto testControlBoundsRight = testControlsBounds;

	auto loadTestSetArea = testControlsBoundsLeft.removeFromTop(testControlsBoundsLeft.getHeight() / 1.5f);
	loadTestSetArea.reduce(loadTestSetArea.getWidth() / 8, 0);
	testSetStatusLabel.setBounds(loadTestSetArea.removeFromTop(loadTestSetArea.getHeight() / 2));
	loadTestSetButton.setBounds(loadTestSetArea);

	auto testButtonArea = testControlBoundsRight.removeFromTop(testControlBoundsRight.getHeight() / 1.5f);
	testButtonArea.reduce(testButtonArea.getWidth() / 8, 0);
	runTestButton.setBounds(testButtonArea.removeFromBottom(testButtonArea.getHeight() / 2));

	auto accuracyLabelBounds = bounds.removeFromTop(bounds.getHeight() / 15);
	accuracyLabelBounds.reduce(accuracyLabelBounds.getWidth() / 30, 0);
	accuracyLabel.setBounds(accuracyLabelBounds.removeFromLeft(accuracyLabelBounds.getWidth() / 2));

	auto tableBounds = bounds;
	tableBounds.reduce(tableBounds.getWidth() / 25, tableBounds.getHeight() / 25);

	table.setBounds(tableBounds);
}

//===============================================================================
void TestClassifierComponent::buttonClicked(Button * button)
{
	auto id = button->getComponentID();

	if (id == loadTestSetButtonID)
	{
		auto success = loadTestSet();

		if (success)
			testSetStatusLabel.setText("Test set status: Ready", NotificationType::dontSendNotification);
		else
			testSetStatusLabel.setText("Test set status: Not Available", NotificationType::dontSendNotification);
	}
	else if (id == runTestButtonID)
	{
		auto ready = processor.getClassifier().getClassifierReady();

		if (ready)
		{
			std::vector<std::pair<unsigned int, unsigned int>> testResults;

			auto accuracy = processor.getClassifier().test(testResults);
			auto accuracyString = String::formatted("%.2f", accuracy) + "%";
			accuracyLabel.setText(accuracyString, NotificationType::dontSendNotification);

			populateResultsTable(testResults);
		}
		else
		{
			auto icon = AlertWindow::AlertIconType::WarningIcon;
			AlertWindow::showMessageBox(icon, "Error Running Test", "The classifier has not been trained. Train before testing", "Close", this);
		}
	}
}

//===============================================================================
int TestClassifierComponent::getNumRows()
{
	return processor.getClassifier().getDataSetSize(AudioClassifyOptions::DataSetType::testSet);
}

//===============================================================================
void TestClassifierComponent::paintRowBackground (Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected)
{
	g.fillAll(Colour(0xffeeeeee));
}

//===============================================================================
void TestClassifierComponent::paintCell (Graphics& g, int rowNumber, int columnId,
				int width, int height, bool /*rowIsSelected*/)
{
	auto valOne = std::get<0>(currentTestResults[rowNumber]);
	auto valTwo = std::get<1>(currentTestResults[rowNumber]);
	auto ready = false;
	String actualName = "";
	String predictedName = "";

	if (valOne != -1 && valTwo != -1)
	{
		ready = true;
		actualName = processor.getSoundName(static_cast<BeatboxVoxAudioProcessor::soundLabel>(valOne));
		predictedName = processor.getSoundName(static_cast<BeatboxVoxAudioProcessor::soundLabel>(valTwo));
	}

	g.setColour (Colours::black);
	g.setFont (Font("Cracked", 14.0f, Font::plain));

	String text;
	switch (columnId)
	{
		case 1:
			if (ready)
				text = String::formatted("%d", rowNumber + 1);
			else
				text = "n/a";
			break;
		case 2:
			if (ready)
				text = actualName;
			else
				text = "n/a";
			break;
		case 3:
			if (ready)
				text = predictedName;
			else
				text = "n/a";
			break;
		default: break;
	}

	g.drawText (text, 2, 0, width - 4, height, Justification::centredLeft, true);

	g.setColour (Colours::black.withAlpha(0.2f));
	g.fillRect (width - 1, 0, 1, height);
}

//===============================================================================
bool TestClassifierComponent::loadTestSet()
{
	auto success = false;
	auto path = File::getSpecialLocation(File::SpecialLocationType::userApplicationDataDirectory).getFullPathName()
																								  + "\\" 
																								  + processor.getName()
																								  + "\\TestSets";
	
	File directory(path.toStdString());

	if (!directory.exists())
		directory.createDirectory();

	FileChooser browser("Load Test Data Set", directory, "*.bin");
	auto fileSelected = browser.browseForFileToOpen();

	if (fileSelected)
	{
		auto fileChosen = browser.getResult();
		auto filePath = fileChosen.getFullPathName();

		std::string errorString;
		success = processor.getClassifier().loadDataSet(filePath.toStdString(), AudioClassifyOptions::DataSetType::testSet, errorString);

		if (!success)
		{
			auto icon = AlertWindow::AlertIconType::WarningIcon;
			AlertWindow::showMessageBox(icon, "Error Loading Test Set", errorString, "Close", this);
		}
	}

	return success;
}

//===============================================================================
void TestClassifierComponent::populateResultsTable(std::vector<std::pair<unsigned int, unsigned int>>& results)
{
	std::copy(results.begin(), results.end(), currentTestResults.begin());
	table.updateContent();
	table.repaint();
}

//===============================================================================
