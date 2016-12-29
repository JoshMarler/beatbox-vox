/*
  ==============================================================================

    AudioDataSet.cpp
    Created: 28 Dec 2016 10:38:05pm
    Author:  Joshua Marler

  ==============================================================================
*/

#include "AudioDataSet.h"
#include <cassert>

//==============================================================================
template<typename T>
AudioDataSet<T>::AudioDataSet(unsigned int initNumSounds, unsigned int initInstancePerSound, unsigned int initNumFeatures,
	unsigned int initBufferSize, unsigned int initSTFTFramesPerBuffer, unsigned int initNumDelayedBuffers)
	: bufferSize(initBufferSize),
	  numSounds(initNumSounds), 
	  instancesPerSound(initInstancePerSound),
	  numFeatures(initNumFeatures)
{
	if (initSTFTFramesPerBuffer != 1)
		stftFramesPerBuffer = initSTFTFramesPerBuffer;

	if (initNumDelayedBuffers != 0)
		numDelayedBuffers = initNumDelayedBuffers;

	initialise();
}

//==============================================================================
template <typename T>
AudioDataSet<T>::~AudioDataSet()
{
	
}

//==============================================================================
template<typename T>
void AudioDataSet<T>::addInstance(const arma::Col<T>& instance, unsigned int soundLabel)
{
	assert(instance.n_rows == data.n_rows);
	
	auto totalInstances = numSounds * instancesPerSound;

	if (instanceCount < totalInstances * (soundLabel + 1))
	{
		data.col(instanceCount) = instance;
		soundLabels[instanceCount] = soundLabel;

		++instanceCount;
	}
	else
	{
		
	}
	
}

//==============================================================================
template<typename T>
bool AudioDataSet<T>::usingFeature(unsigned int stftFrameNumber, AudioClassifyOptions::AudioFeature feature)
{
	for (auto i = 0; i < numFeatures; ++i)
	{
		if (featureFrameLabels[i] == stftFrameNumber && featureLabels[i] == static_cast<unsigned int>(feature))
			return true;
	}

	return false;
}

//==============================================================================
template<typename T>
const arma::Mat<T>& AudioDataSet<T>::getData()
{
	return data;
}

//==============================================================================
template<typename T>
const arma::Row<unsigned int>& AudioDataSet<T>::getSoundLabels() const
{
	return soundLabels;
}

//==============================================================================
template<typename T>
const arma::Col<unsigned int>& AudioDataSet<T>::getFeatureFrameLabels() const
{
	return featureFrameLabels;
}

//==============================================================================
template<typename T>
const arma::Col<unsigned int>& AudioDataSet<T>::getFeatureLabels() const
{
	return featureLabels;
}

//==============================================================================
template<typename T>
AudioDataSet<T> AudioDataSet<T>::load(const std::string & absoluteFilePath, std::string & errorString)
{
	File file(absoluteFilePath);
	
	if(!file.exists())
		errorString = "No such file exists.";

	FileInputStream is(file);

	if (is.getStatus().failed())
		errorString = "Error opening file";

	auto vt = ValueTree::readFromStream(is);

	int numSounds = vt.getProperty("NumSounds");
	int instancesPerSound = vt.getProperty("InstancesPerSound");
	int numFeatures = vt.getProperty("NumFeatures");
	int bufferSize = vt.getProperty("BufferSize");

	auto dataBlock = vt.getProperty("Data").getBinaryData();
	auto soundLabelsBlock = vt.getProperty("SoundLabels").getBinaryData();
	auto featureFrameLabelsBlock = vt.getProperty("FeatureFrameLabels").getBinaryData();
	auto featureLabelsBlock = vt.getProperty("FeatureLabels").getBinaryData();

	AudioDataSet<T> dataSet(numSounds, instancesPerSound, numFeatures, bufferSize);

	arma::Mat<T> data(static_cast<T*>(dataBlock->getData()), numFeatures, numSounds * instancesPerSound);
	arma::Row<unsigned int> labels(static_cast<unsigned int*>(soundLabelsBlock->getData()), soundLabelsBlock->getSize());

	arma::Col<unsigned int> featureFrameLabels(static_cast<unsigned int*>(featureFrameLabelsBlock->getData()), featureFrameLabelsBlock->getSize());
	arma::Col<unsigned int> featureLabels(static_cast<unsigned int*>(featureLabelsBlock->getData()), featureLabelsBlock->getSize());

	dataSet.setData(data);
	dataSet.setSoundLabels(labels);
	dataSet.setFeatureFrameLabels(featureFrameLabels);
	dataSet.setFeatureLabels(featureLabels);

	return dataSet;
}

//==============================================================================
template<typename T>
void AudioDataSet<T>::save(AudioDataSet<T>& dataSet, const std::string & absoluteFilePath, std::string & errorString)
{
	File file(absoluteFilePath);
	file.deleteFile();

	ValueTree vt("AudioDataSet");

	vt.setProperty("NumSounds", var(static_cast<int>(dataSet.getNumSounds())), nullptr);
	vt.setProperty("InstancesPerSound", var(static_cast<int>(dataSet.getInstancesPerSound())), nullptr);
	vt.setProperty("NumFeatures", var(static_cast<int>(dataSet.getNumFeatures())), nullptr);
	vt.setProperty("BufferSize", var(static_cast<int>(dataSet.getBufferSize())), nullptr);

	auto dataBlockSize = static_cast<size_t>(dataSet.getData().size() * sizeof(T));
	MemoryBlock dataMb(dataSet.getData().mem, dataBlockSize);
	vt.setProperty("Data", var(dataMb), nullptr);

	auto soundLabelsBlockSize = static_cast<size_t>(dataSet.getSoundLabels().size() * sizeof(unsigned int));
	MemoryBlock soundLabelsMb(dataSet.getSoundLabels().mem, soundLabelsBlockSize);
	vt.setProperty("SoundLabels", var(soundLabelsMb), nullptr);

	auto featureFrameLabelsBlockSize = static_cast<size_t>(dataSet.getNumFeatures() * sizeof(unsigned int));
	MemoryBlock featureFrameLabelsMb(dataSet.getFeatureFrameLabels().mem, featureFrameLabelsBlockSize);
	vt.setProperty("FeatureFrameLabels", var(featureFrameLabelsMb), nullptr);

	auto featureLabelsBlockSize = static_cast<size_t>(dataSet.getNumFeatures() * sizeof(unsigned int));
	MemoryBlock featureLabelsMb(dataSet.getFeatureLabels().mem, featureLabelsBlockSize);
	vt.setProperty("FeatureLabels", var(featureLabelsMb), nullptr);


	FileOutputStream os(file);
	
	if (os.getStatus().failed())
	{
		errorString = "Error creating file";
		os.flush();
		return;
	}

	vt.writeToStream(os);
	os.flush();

	//Add check here to see if saved successfully ? 
}

//==============================================================================
template<typename T>
unsigned int AudioDataSet<T>::getBufferSize() const
{
	return bufferSize;
}


//==============================================================================
template<typename T>
unsigned int AudioDataSet<T>::getSTFTFramesPerBuffer() const
{
	return stftFramesPerBuffer;
}

//==============================================================================
template<typename T>
unsigned int AudioDataSet<T>::getNumDelayedBuffers() const
{
	return numDelayedBuffers;
}

//==============================================================================
template<typename T>
unsigned int AudioDataSet<T>::getNumSounds() const
{
	return numSounds;
}

//==============================================================================
template<typename T>
void AudioDataSet<T>::setInstancesPerSound(unsigned int newInstancesPerSound)
{
	instancesPerSound = newInstancesPerSound;

	//Reset containers and state
	initialise();
}

//==============================================================================
template<typename T>
unsigned int AudioDataSet<T>::getInstancesPerSound() const
{
	return instancesPerSound;
}

//==============================================================================
template<typename T>
unsigned int AudioDataSet<T>::getNumFeatures() const
{
	return numFeatures;
}

//==============================================================================
template<typename T>
void AudioDataSet<T>::reset()
{
}

//==============================================================================
template<typename T>
bool AudioDataSet<T>::isReady()
{
	auto readyCount = 0;
	auto ready = false;

	for (auto v : soundsReady)
	{
		if (v == true)
			++readyCount;
	}

	if (readyCount == numSounds)
		ready = true;

	return ready;
}

//==============================================================================
template<typename T>
bool AudioDataSet<T>::checkSoundReady(const unsigned int sound) const
{
	return soundsReady[sound];
}

//==============================================================================
template<typename T>
void AudioDataSet<T>::setData(arma::Mat<T>& newData)
{
	data = newData;
}

//==============================================================================
template<typename T>
void AudioDataSet<T>::setSoundLabels(arma::Row<unsigned int>& newLabels)
{
	soundLabels = newLabels;
}

//==============================================================================
template<typename T>
void AudioDataSet<T>::setFeatureFrameLabels(arma::Col<unsigned int>& newFeatureFrameLabels)
{
	featureFrameLabels = newFeatureFrameLabels;
}

//==============================================================================
template<typename T>
void AudioDataSet<T>::setFeatureLabels(arma::Col<unsigned int>& newFeatureLabels)
{
	featureLabels = newFeatureLabels;
}

//==============================================================================
template<typename T>
void AudioDataSet<T>::initialise()
{
	auto totalInstances = numSounds * instancesPerSound;

	soundLabels.set_size(totalInstances);

	featureLabels.set_size(numFeatures);
	featureFrameLabels.set_size(numFeatures);

	//Initialise sound ready states
	soundsReady.resize(numSounds, false);
}

//==============================================================================
template class AudioDataSet<float>;
template class AudioDataSet<double>;

//==============================================================================
