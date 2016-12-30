/*
  ==============================================================================

    AudioClassifyOptions.h
    Created: 15 Jul 2016 10:31:26pm
    Author:  joshua

  ==============================================================================
*/

#ifndef AUDIOCLASSIFYOPTIONS_H_INCLUDED
#define AUDIOCLASSIFYOPTIONS_H_INCLUDED


/**
 *
 */
struct AudioClassifyOptions
{
	static const int totalNumAudioFeatures = 21;

    enum class ClassifierType: int
    {
        naiveBayes,
        nearestNeighbour
    };

    enum class AudioFeature: int 
    {
		rms = 0,
		peakEnergy,
		zeroCrossingRate,
        spectralCentroid,
        spectralCrest, 
        spectralFlatness, 
        spectralRolloff, 
        spectralKurtois,
        mfcc_1,
		mfcc_2,
		mfcc_3,
		mfcc_4,
		mfcc_5,
		mfcc_6,
		mfcc_7,
		mfcc_8,
		mfcc_9,
		mfcc_10,
		mfcc_11,
		mfcc_12,
		mfcc_13

    };

    enum class ODFType: int
    {
        spectralDifference,
        spectralDifferenceHWR,
        //complexSpectralDifference,
        highFrequencyContent
    };

	enum class DataSetType: int
	{
		trainingSet,
		testSet
	};
};



#endif  // AUDIOCLASSIFYOPTIONS_H_INCLUDED
