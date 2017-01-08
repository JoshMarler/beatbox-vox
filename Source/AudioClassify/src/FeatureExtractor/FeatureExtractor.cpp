/*
  ==============================================================================

    FeatureExtractor.cpp
    Created: 26 Nov 2016 10:24:12pm
    Author:  Joshua Marler

  ==============================================================================
*/

#include "FeatureExtractor.h"
#include <cassert>

//==============================================================================
template<typename T>
FeatureExtractor<T>::FeatureExtractor(int initFrameSize, int initSampleRate)
	: gist(initFrameSize, initSampleRate)
{
	auto mfccNumCoefficients = gist.getMFCCNumCoefficients();
	mfccs.reset(new T[mfccNumCoefficients]);
}

//==============================================================================
template<typename T>
FeatureExtractor<T>::~FeatureExtractor()
{
}

//==============================================================================
template<typename T>
void FeatureExtractor<T>::setSampleRate(int newSampleRate)
{
	gist.setSamplingFrequency(newSampleRate);
}

//==============================================================================
template<typename T>
void FeatureExtractor<T>::setFrameSize(int newFrameSize)
{
	gist.setAudioFrameSize(newFrameSize);
}

//==============================================================================
template<typename T>
void FeatureExtractor<T>::processFrame(const T* audioFrame, const int frameSize)
{
	//May remove
	assert(gist.getAudioFrameSize() == frameSize);

	gist.processAudioFrame(audioFrame, frameSize);

	gist.melFrequencyCepstralCoefficients(mfccs.get());
}

//==============================================================================
template<typename T>
T FeatureExtractor<T>::getFeature(AudioClassifyOptions::AudioFeature feature)
{
	T featureValue = static_cast<T>(0.0);

	switch (feature)
	{
		case AudioClassifyOptions::AudioFeature::rms :
			featureValue = gist.rootMeanSquare();
			break;
		case AudioClassifyOptions::AudioFeature::peakEnergy :
			featureValue = gist.peakEnergy();
			break;
		case AudioClassifyOptions::AudioFeature::zeroCrossingRate :
			featureValue = gist.zeroCrossingRate();
			break;
		case AudioClassifyOptions::AudioFeature::spectralCentroid :
			featureValue = gist.spectralCentroid();
			break;
		case AudioClassifyOptions::AudioFeature::spectralCrest :
			featureValue = gist.spectralCrest();
			break;
		case AudioClassifyOptions::AudioFeature::spectralFlatness :
			featureValue = gist.spectralFlatness();
			break;
		case AudioClassifyOptions::AudioFeature::spectralRolloff :
			featureValue = gist.spectralRolloff();
			break;
		case AudioClassifyOptions::AudioFeature::spectralKurtois :
			featureValue = gist.spectralKurtosis();
			break;
		case AudioClassifyOptions::AudioFeature::mfcc_1 :
			featureValue = mfccs[0];
			break;
		case AudioClassifyOptions::AudioFeature::mfcc_2 :
			featureValue = mfccs[1];
			break;
		case AudioClassifyOptions::AudioFeature::mfcc_3 :
			featureValue = mfccs[2];
			break;
		case AudioClassifyOptions::AudioFeature::mfcc_4 :
			featureValue = mfccs[3];
			break;
		case AudioClassifyOptions::AudioFeature::mfcc_5 :
			featureValue = mfccs[4];
			break;
		case AudioClassifyOptions::AudioFeature::mfcc_6 :
			featureValue = mfccs[5];
			break;
		case AudioClassifyOptions::AudioFeature::mfcc_7 :
			featureValue = mfccs[6];
			break;
		case AudioClassifyOptions::AudioFeature::mfcc_8 :
			featureValue = mfccs[7];
			break;
		case AudioClassifyOptions::AudioFeature::mfcc_9 :
			featureValue = mfccs[8];
			break;
		case AudioClassifyOptions::AudioFeature::mfcc_10 :
			featureValue = mfccs[9];
			break;
		case AudioClassifyOptions::AudioFeature::mfcc_11 :
			featureValue = mfccs[10];
			break;
		case AudioClassifyOptions::AudioFeature::mfcc_12 :
			featureValue = mfccs[11];
			break;
		case AudioClassifyOptions::AudioFeature::mfcc_13 :
			featureValue = mfccs[12];
			break;
		default: break;
	}

	return featureValue;
}

//==============================================================================
template class FeatureExtractor<float>;
template class FeatureExtractor<double>;
//==============================================================================
