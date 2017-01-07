/*
  ==============================================================================

    AudioClassifyOptions.h
    Created: 15 Jul 2016 10:31:26pm
    Author:  joshua

  ==============================================================================
*/

#ifndef AUDIOCLASSIFYOPTIONS_H_INCLUDED
#define AUDIOCLASSIFYOPTIONS_H_INCLUDED
#include <map>


/**
 *
 */
struct AudioClassifyOptions
{
    enum class ClassifierType: int
    {
        naiveBayes = 0,
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
        spectralDifference = 0,
        spectralDifferenceHWR,
        //complexSpectralDifference,
        highFrequencyContent
    };

	enum class DataSetType: int
	{
		trainingSet = 0,
		testSet
	};

	static std::string getFeatureName(AudioFeature feature)
	{
		switch (feature)
		{
			case AudioFeature::rms:
				return "RMS";
			case AudioFeature::peakEnergy:
				return "Peak Energy";
			case AudioFeature::zeroCrossingRate:
				return "Zero Crossing Rate";
			case AudioFeature::spectralCentroid:
				return "Spectral Centroid";
			case AudioFeature::spectralCrest:
				return "Spectral Crest";
			case AudioFeature::spectralFlatness:
				return "Spectral Flatness";
			case AudioFeature::spectralRolloff:
				return "Spectral Rolloff";
			case AudioFeature::spectralKurtois:
				return "Spectral Kurtois";
			case AudioFeature::mfcc_1:
				return "MFCC 1";
			case AudioFeature::mfcc_2:
				return "MFCC 2";
			case AudioFeature::mfcc_3:
				return "MFCC 3";
			case AudioFeature::mfcc_4:
				return "MFCC 4";
			case AudioFeature::mfcc_5:
				return "MFCC 5";
			case AudioFeature::mfcc_6:
				return "MFCC 6";
			case AudioFeature::mfcc_7:
				return "MFCC 7";
			case AudioFeature::mfcc_8:
				return "MFCC 8";
			case AudioFeature::mfcc_9:
				return "MFCC 9";
			case AudioFeature::mfcc_10:
				return "MFCC 10";
			case AudioFeature::mfcc_11:
				return "MFCC 11";
			case AudioFeature::mfcc_12:
				return "MFCC 12";
			case AudioFeature::mfcc_13:
				return "MFCC 13";
			default: return "";
		}
	}

	static const int totalNumAudioFeatures = 21;
};



#endif  // AUDIOCLASSIFYOPTIONS_H_INCLUDED
