/*
  ==============================================================================

    FeatureVariancesComponent.cpp
    Created: 7 Jan 2017 12:03:23am
    Author:  Joshua Marler

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "FeatureVariancesComponent.h"

//==============================================================================
FeatureVariancesComponent::FeatureVariancesComponent(std::vector<std::pair<FeatureFramePair, float>> initFeatureVariances)
	: featureVariances(initFeatureVariances),
	  table("Feature Variances Table", this)
{
	numFeatures = featureVariances.size();

	table.setColour(ListBox::ColourIds::outlineColourId, Colours::greenyellow);
	table.setColour(ListBox::ColourIds::backgroundColourId, Colours::black);
	table.setColour(ListBox::ColourIds::textColourId, Colours::greenyellow);
	table.setOutlineThickness(1);

	table.getHeader().addColumn("Feature", 1, 40);
	table.getHeader().addColumn("STFT Frame", 2, 40);
	table.getHeader().addColumn("Variance", 3, 40);

	table.getHeader().setStretchToFitActive(true);

	addAndMakeVisible(table);

	table.updateContent();
	table.repaint();

	setSize(550, 500);
}

FeatureVariancesComponent::~FeatureVariancesComponent()
{
}

void FeatureVariancesComponent::paint (Graphics& g)
{
	g.setColour(Colours::black.withAlpha(static_cast<uint8>(0xdc)));

	Rectangle<float> fillRect(getWidth(), getHeight());
	g.fillRoundedRectangle(fillRect, 10.0f);
}

void FeatureVariancesComponent::resized()
{
	auto bounds = getLocalBounds();

	bounds.reduce(bounds.getWidth() / 25, bounds.getHeight() / 25);
	
	table.setBounds(bounds);
}

int FeatureVariancesComponent::getNumRows()
{
	return numFeatures;
}

void FeatureVariancesComponent::paintRowBackground(Graphics & g, int rowNumber, int, int, bool rowIsSelected)
{
	g.fillAll(Colour(0xffeeeeee));
}

void FeatureVariancesComponent::paintCell(Graphics & g, int rowNumber, int columnId, int width, int height, bool)
{
	auto featureFramePair = featureVariances[rowNumber].first;

	auto frameNumber = featureFramePair.first;
	auto feature = static_cast<AudioClassifyOptions::AudioFeature>(featureFramePair.second);

	auto varianceValue = featureVariances[rowNumber].second;

	g.setColour (Colours::black);
	g.setFont (Font("Cracked", 14.0f, Font::plain));

	String text;
	switch (columnId)
	{
		case 1:
			text = AudioClassifyOptions::getFeatureName(feature);
			break;
		case 2:
			text = String::formatted("%d", frameNumber);
			break;
		case 3:
			text = String::formatted("%.5f", varianceValue);
			break;
		default: break;
	}

	g.drawText (text, 2, 0, width - 4, height, Justification::centredLeft, true);

	g.setColour (Colours::black.withAlpha(0.2f));
	g.fillRect (width - 1, 0, 1, height);
}

