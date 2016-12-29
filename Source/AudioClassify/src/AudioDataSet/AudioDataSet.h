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
#include "../../../../JuceLibraryCode/JuceHeader.h"

#include "../AudioClassifyOptions/AudioClassifyOptions.h"

template<typename T>
class AudioDataSet
{
public:

	AudioDataSet(unsigned int initNumSounds, unsigned int initInstancePerSound, unsigned int initNumFeatures, 
		unsigned int initBufferSize, unsigned int initSTFTFramesPerBuffer = 1, unsigned int initNumDelayedBuffers = 0);

	~AudioDataSet();

	void addInstance(const arma::Col<T>& instance, unsigned int soundLabel);

	//NOTE: Potentially add setUsingFeature method in future to allow explicit on/off of features. 
	bool usingFeature(unsigned int stftFrameNumber, AudioClassifyOptions::AudioFeature feature);

	const arma::Mat<T>& getData();
	const arma::Row<unsigned int>& getSoundLabels() const;
	const arma::Col<unsigned int>& getFeatureFrameLabels() const;
	const arma::Col<unsigned int>& getFeatureLabels() const;

	static AudioDataSet<T> load(const std::string& absoluteFilePath, std::string& errorString);
	static void save(AudioDataSet<T>& dataSet, const std::string& absoluteFilePath, std::string& errorString);

	unsigned int getBufferSize() const;

	unsigned int getSTFTFramesPerBuffer() const;
	unsigned int getNumDelayedBuffers() const;

	unsigned int getNumSounds() const;

	void setInstancesPerSound(unsigned int newInstancesPerSound);
	unsigned int getInstancesPerSound() const;

	unsigned int getNumFeatures() const;

	void reset();
	bool isReady();
	
	bool checkSoundReady(const unsigned int sound) const;

private:

	bool ready = false;
	unsigned int instanceCount = 0;

	unsigned int bufferSize;
	unsigned int stftFramesPerBuffer;
	unsigned int numDelayedBuffers;

	unsigned int numSounds;
	unsigned int instancesPerSound;
	unsigned int numFeatures;

	std::vector<bool> soundsReady;

	arma::Mat<T> data;
	arma::Row<unsigned int> soundLabels;

	arma::Col<unsigned int> featureLabels;
	arma::Col<unsigned int> featureFrameLabels;

	void setData(arma::Mat<T>& newData);
	void setSoundLabels(arma::Row<unsigned int>& newLabels);
	void setFeatureFrameLabels(arma::Col<unsigned int>& newFeatureFrameLabels);
	void setFeatureLabels(arma::Col<unsigned int>& newFeatureLabels);

	void initialise();
};


#endif  // AUDIODATASET_H_INCLUDED

