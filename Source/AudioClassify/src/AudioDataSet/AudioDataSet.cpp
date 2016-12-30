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
AudioDataSet<T>::AudioDataSet(unsigned int initNumSounds, unsigned int initInstancePerSound, unsigned int initBufferSize,
	unsigned int initSTFTFramesPerBuffer, unsigned int initNumDelayedBuffers)
	: bufferSize(initBufferSize),
	  numSounds(initNumSounds), 
	  instancesPerSound(initInstancePerSound)
{
	stftFramesPerBuffer = initSTFTFramesPerBuffer;
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
		soundsReady[soundLabel] = true;
	
}

//==============================================================================
template<typename T>
bool AudioDataSet<T>::usingFeature(unsigned int stftFrameNumber, AudioClassifyOptions::AudioFeature feature)
{
	for (auto featureFramePair : featuresUsed)
	{
		if (featureFramePair.first == stftFrameNumber && featureFramePair.second == feature)
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
//JWM - NOTE: Potentially change to return bool for success and take in output data set ? 
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
	int bufferSize = vt.getProperty("BufferSize");
	int stftFramesPerBuffer = vt.getProperty("STFTFramesPerBuffer");
	int numDelayedBuffers = vt.getProperty("NumDelayedBuffers");


	AudioDataSet<T> dataSet(numSounds, instancesPerSound, bufferSize, stftFramesPerBuffer, numDelayedBuffers);

	auto featuresUsed = vt.getChildWithName("FeaturesUsed");

	//Going to reset features used
	dataSet.featuresUsed.resize(0);

	for(auto i = 0; i < featuresUsed.getNumChildren(); ++i)
	{
		auto featureFramePair = featuresUsed.getChild(i);
		auto frame = int(featureFramePair.getProperty("Frame"));
		auto feature = static_cast<AudioClassifyOptions::AudioFeature>(int(featureFramePair.getProperty("Feature")));

		dataSet.featuresUsed.push_back(std::make_pair(frame, feature));
	}
	
	auto dataBlock = vt.getProperty("Data").getBinaryData();
	arma::Mat<T> data(static_cast<T*>(dataBlock->getData()), dataSet.featuresUsed.size(), numSounds * instancesPerSound);

	auto soundLabelsBlock = vt.getProperty("SoundLabels").getBinaryData();
	arma::Row<unsigned int> soundLabels(static_cast<unsigned int*>(soundLabelsBlock->getData()), soundLabelsBlock->getSize());

	dataSet.setData(data);
	dataSet.setSoundLabels(soundLabels);

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
	vt.setProperty("STFTFramesPerBuffer", var(static_cast<int>(dataSet.getSTFTFramesPerBuffer())), nullptr);
	vt.setProperty("NumDelayedBuffers", var(static_cast<int>(dataSet.getNumDelayedBuffers())), nullptr);

	auto dataBlockSize = static_cast<size_t>(dataSet.getData().size() * sizeof(T));
	MemoryBlock dataMb(dataSet.getData().mem, dataBlockSize);
	vt.setProperty("Data", var(dataMb), nullptr);

	auto soundLabelsBlockSize = static_cast<size_t>(dataSet.getSoundLabels().size() * sizeof(unsigned int));
	MemoryBlock soundLabelsMb(dataSet.getSoundLabels().mem, soundLabelsBlockSize);
	vt.setProperty("SoundLabels", var(soundLabelsMb), nullptr);


	ValueTree featuresUsedTree("FeaturesUsed");
	
	for (auto featureFramePair : dataSet.featuresUsed)
	{
		ValueTree pair("FeatureFramePair");
		pair.setProperty("Frame", var(static_cast<int>(featureFramePair.first)), nullptr);
		pair.setProperty("Feature", var(static_cast<int>(featureFramePair.second)), nullptr);

		featuresUsedTree.addChild(pair, -1, nullptr);
	}

	vt.addChild(featuresUsedTree, -1, nullptr);


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
	return featuresUsed.size();
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
void AudioDataSet<T>::initialise()
{
	auto totalInstances = numSounds * instancesPerSound;
	auto totalFrames = stftFramesPerBuffer * (numDelayedBuffers + 1);

	soundLabels.set_size(totalInstances);
	soundsReady.resize(numSounds, false);

	for (auto i = 0; i < totalFrames; ++i)
	{
		for (auto j = 0; j < AudioClassifyOptions::totalNumAudioFeatures; ++j)
		{
			auto frame = i + 1;
			auto feature = static_cast<AudioClassifyOptions::AudioFeature>(j);
			featuresUsed.push_back(std::make_pair(frame, feature));
		}
	}

	data.set_size(featuresUsed.size(), totalInstances);
	data.fill(static_cast<T>(0.0));

}

//==============================================================================
template class AudioDataSet<float>;
template class AudioDataSet<double>;

//==============================================================================
