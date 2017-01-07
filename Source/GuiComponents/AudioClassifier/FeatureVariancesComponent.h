/*
  ==============================================================================

    FeatureVariancesComponent.h
    Created: 7 Jan 2017 12:03:23am
    Author:  Joshua Marler

  ==============================================================================
*/

#ifndef FEATUREVARIANCESCOMPONENT_H_INCLUDED
#define FEATUREVARIANCESCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "../../AudioClassify/src/AudioClassify.h"
//==============================================================================
/*
*/
using FeatureFramePair = std::pair<int, AudioClassifyOptions::AudioFeature>;

class FeatureVariancesComponent : public Component,
										 TableListBoxModel
{
public:
    FeatureVariancesComponent(std::vector<std::pair<FeatureFramePair, float>> initFeatureVariances);
    ~FeatureVariancesComponent();

    void paint (Graphics&) override;
    void resized() override;

	//TableListBoxModel overrides
	int getNumRows() override;
	void paintRowBackground(Graphics & g, int rowNumber, int, int, bool rowIsSelected) override;
	void paintCell(Graphics & g, int rowNumber, int columnId, int width, int height, bool) override;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FeatureVariancesComponent)

	std::vector<std::pair<FeatureFramePair, float>> featureVariances;

	int numFeatures = 21;

	TableListBox table;

};


#endif  // FEATUREVARIANCESCOMPONENT_H_INCLUDED
