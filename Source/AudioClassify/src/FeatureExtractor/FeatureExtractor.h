/*
  ==============================================================================

    FeatureExtractor.h
    Created: 26 Nov 2016 10:24:12pm
    Author:  Joshua Marler

  ==============================================================================
*/

#ifndef FEATUREEXTRACTOR_H_INCLUDED
#define FEATUREEXTRACTOR_H_INCLUDED

#include "../../Gist/src/Gist.h";
#include "../AudioClassifyOptions/AudioClassifyOptions.h";

template<typename T>
class FeatureExtractor
{
public:
	FeatureExtractor(int initBufferSize, int initSampleRate);
	~FeatureExtractor();

	void setSampleRate(int newSampleRate);
	void setFrameSize(int newFrameSize);

	void processFrame(const T* audioFrame, const int frameSize);

	T getFeature(AudioClassifyOptions::AudioFeature feature);

private:
    std::unique_ptr<T[]> mfccs;

	Gist<T> gist;
};



#endif  // FEATUREEXTRACTOR_H_INCLUDED

