/*
  ==============================================================================

    AudioDataSet.h
    Created: 28 Dec 2016 10:38:05pm
    Author:  Joshua Marler

  ==============================================================================
*/

#ifndef AUDIODATASET_H_INCLUDED
#define AUDIODATASET_H_INCLUDED

#ifdef _WIN64
#define ARMA_64BIT_WORD
#endif

#include <armadillo.h>
#include "JuceHeader.h"

#include "../AudioClassifyOptions/AudioClassifyOptions.h"

using FeatureFramePair = std::pair<int, AudioClassifyOptions::AudioFeature>;

template<typename T>
class AudioDataSet
{
public:

	AudioDataSet();

	AudioDataSet(int initNumSounds, int initInstancePerSound, int initBufferSize,
		int initSTFTFramesPerBuffer = 1, int initNumDelayedBuffers = 0);

	~AudioDataSet();

	bool load(const std::string& absoluteFilePath, std::string& errorString);
	bool save(const std::string& absoluteFilePath, std::string& errorString);

	AudioDataSet<T> getVarianceReducedCopy(int numFeatures);

	void addInstance(const arma::Col<T>& instance, unsigned int soundLabel);

	//NOTE: Potentially add setUsingFeature method in future to allow explicit on/off of features. 
	bool usingFeature(int stftFrameNumber, AudioClassifyOptions::AudioFeature feature);
	int getFeatureIndex(int stftFrameNumber, AudioClassifyOptions::AudioFeature feature);

	const arma::Mat<T>& getData() const;
	const arma::Row<unsigned int>& getSoundLabels() const;


	int getBufferSize() const;

	int getSTFTFramesPerBuffer() const;

	int getNumDelayedBuffers() const;

	int getNumSounds() const;

	int getInstancesPerSound() const;

	int getNumFeatures() const;

	int getTotalNumInstances() const;
	int getTotalNumSTFTFrames() const;

	bool isReady() const;
	
	bool checkSoundReady(const unsigned int sound) const;


private:

	int instanceCount = 0;

	int bufferSize;
	int stftFramesPerBuffer;
	int numDelayedBuffers;

	int numSounds;
	int instancesPerSound;

	std::vector<bool> soundsReady;

	arma::Mat<T> data;
	arma::Row<unsigned int> soundLabels;

	std::vector<FeatureFramePair> featuresUsed;

	void setData(const arma::Mat<T>& newData);
	void setSoundLabels(const arma::Row<unsigned>& newLabels);
	void setFeaturesUsed(const std::vector<FeatureFramePair>& newFeaturesUsed);

	void initialise();
};


#endif  // AUDIODATASET_H_INCLUDED

