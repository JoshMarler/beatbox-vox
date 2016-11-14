/*
  ==============================================================================

    TestClassifierComponent.cpp
    Created: 12 Nov 2016 10:21:28pm
    Author:  Joshua Marler

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "TestClassifierComponent.h"

String TestClassifierComponent::testSetSliderID("test_set_sld");
String TestClassifierComponent::runTestButtonID("run_test_btn");

//==============================================================================
TestClassifierComponent::TestClassifierComponent()
	: componentLookAndFeel(std::make_unique<CustomLookAndFeel>()),
	  runTestButton("Run Test"),
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

	testSetSizeLabel.setText("Test set size", NotificationType::dontSendNotification);
	testSetSizeLabel.setFont(Font("Cracked", 20.0f, Font::plain));
	testSetSizeLabel.setColour(Label::textColourId, Colours::greenyellow);
	addAndMakeVisible(testSetSizeLabel);

	testSetSizeSlider.setComponentID(testSetSliderID);
	testSetSizeSlider.setSliderStyle (Slider::IncDecButtons);
	testSetSizeSlider.setRange (10.0, 40.0, 1.0);
	testSetSizeSlider.setIncDecButtonsMode (Slider::incDecButtonsDraggable_Horizontal);
	testSetSizeSlider.setTextBoxStyle(Slider::TextBoxRight, true, 90, 20);
	testSetSizeSlider.setColour(Slider::textBoxBackgroundColourId, Colours::black);
	testSetSizeSlider.setColour(Slider::textBoxTextColourId, Colours::greenyellow);
	testSetSizeSlider.setColour(Slider::textBoxOutlineColourId, Colours::black);
	testSetSizeSlider.addListener(this);
	addAndMakeVisible (testSetSizeSlider);

	accuracyLabel.setText("Accuracy: ", NotificationType::dontSendNotification);
	accuracyLabel.setFont(Font("Cracked", 20.0f, Font::plain));
	accuracyLabel.setColour(Label::textColourId, Colours::greenyellow);
	addAndMakeVisible(accuracyLabel);

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

	auto setTestSizeArea = testControlsBoundsLeft.removeFromTop(testControlsBoundsLeft.getHeight() / 1.5f);
	setTestSizeArea.reduce(setTestSizeArea.getWidth() / 10, setTestSizeArea.getHeight() / 25);

	testSetSizeLabel.setBounds(setTestSizeArea.removeFromTop(setTestSizeArea.getHeight() / 1.5f));
	testSetSizeSlider.setBounds(setTestSizeArea);

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
}

//===============================================================================
void TestClassifierComponent::sliderValueChanged(Slider * slider)
{
}

//===============================================================================
int TestClassifierComponent::getNumRows()
{
	return numTestResults;
}

//===============================================================================
void TestClassifierComponent::paintRowBackground (Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected)
{
	if (rowIsSelected)
		g.fillAll (Colours::lightblue);
	else if (rowNumber % 2)
		g.fillAll (Colour (0xffeeeeee));
}

//===============================================================================
void TestClassifierComponent::paintCell (Graphics& g, int rowNumber, int columnId,
				int width, int height, bool /*rowIsSelected*/)
{
	g.setColour (Colours::black);
	g.setFont (Font("Cracked", 14.0f, Font::plain));

	/*if (const XmlElement* rowElement = dataList->getChildElement (rowNumber))
	{
		const String text (rowElement->getStringAttribute (getAttributeNameForColumnId (columnId)));

		g.drawText (text, 2, 0, width - 4, height, Justification::centredLeft, true);
	}*/

	g.setColour (Colours::black.withAlpha (0.2f));
	g.fillRect (width - 1, 0, 1, height);
}

//===============================================================================
