/*
  ==============================================================================

    AudioDataSet.cpp
    Created: 28 Dec 2016 10:38:05pm
    Author:  Joshua Marler

  ==============================================================================
*/

#include "AudioDataSet.h"
#include <cassert>

template<typename T>
AudioDataSet<T>::AudioDataSet()
	: bufferSize(0),
	  stftFramesPerBuffer(0),
      numDelayedBuffers(0),
      numSounds(0),
	  instancesPerSound(0)
{
	initialise();
}

//==============================================================================
template<typename T>
AudioDataSet<T>::AudioDataSet(int initNumSounds, int initInstancePerSound, int initBufferSize,
		int initSTFTFramesPerBuffer, int initNumDelayedBuffers)
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
bool AudioDataSet<T>::load(const std::string & absoluteFilePath, std::string & errorString)
{
	File file(absoluteFilePath);
	
	if(!file.exists())
	{
		errorString = "No such file exists.";
		return false;
	}

	FileInputStream is(file);

	if (is.getStatus().failed())
	{
		errorString = "Error opening file";
		return false;
	}

	auto vt = ValueTree::readFromStream(is);

	numSounds = vt.getProperty("NumSounds");
	instancesPerSound = vt.getProperty("InstancesPerSound");
	bufferSize = vt.getProperty("BufferSize");
	stftFramesPerBuffer = vt.getProperty("STFTFramesPerBuffer");
	numDelayedBuffers = vt.getProperty("NumDelayedBuffers");

	auto featuresUsedLoaded = vt.getChildWithName("FeaturesUsed");

	//Going to reset features used
	featuresUsed.resize(0);

	for(auto i = 0; i < featuresUsedLoaded.getNumChildren(); ++i)
	{
		auto featureFramePair = featuresUsedLoaded.getChild(i);
		auto frame = int(featureFramePair.getProperty("Frame"));
		auto feature = static_cast<AudioClassifyOptions::AudioFeature>(int(featureFramePair.getProperty("Feature")));

		featuresUsed.push_back(std::make_pair(frame, feature));
	}
	
	auto dataBlock = vt.getProperty("Data").getBinaryData();
	arma::Mat<T> dataLoaded(static_cast<T*>(dataBlock->getData()), featuresUsed.size(), getTotalNumInstances());

	auto soundLabelsBlock = vt.getProperty("SoundLabels").getBinaryData();
	arma::Row<int> soundLabelsLoaded(static_cast<int*>(soundLabelsBlock->getData()), getTotalNumInstances());

	setData(dataLoaded);
	setSoundLabels(soundLabelsLoaded);

	//Assume sounds ready if loaded as required by save
	for (auto& v : soundsReady)
	{
		v = true;
	}

	return true;
}

//==============================================================================
template<typename T>
bool AudioDataSet<T>::save(const std::string & absoluteFilePath, std::string & errorString)
{
	if (getTotalNumInstances() <= 0)
	{
		errorString = "No data to save";
		return false;
	}

	if (!isReady())
	{
		errorString = "Data set not ready. Finish recording instances";
		return false;
	}

	File file(absoluteFilePath);
	file.deleteFile();

	ValueTree vt("AudioDataSet");

	vt.setProperty("NumSounds", var(numSounds), nullptr);
	vt.setProperty("InstancesPerSound", var(instancesPerSound), nullptr);
	vt.setProperty("NumFeatures", var(getNumFeatures()), nullptr);
	vt.setProperty("BufferSize", var(bufferSize), nullptr);
	vt.setProperty("STFTFramesPerBuffer", var(stftFramesPerBuffer), nullptr);
	vt.setProperty("NumDelayedBuffers", var(numDelayedBuffers), nullptr);

	auto dataBlockSize = static_cast<size_t>(data.size() * sizeof(T));
	MemoryBlock dataMb(data.mem, dataBlockSize);
	vt.setProperty("Data", var(dataMb), nullptr);

	auto soundLabelsBlockSize = static_cast<size_t>(soundLabels.size() * sizeof(int));
	MemoryBlock soundLabelsMb(soundLabels.mem, soundLabelsBlockSize);
	vt.setProperty("SoundLabels", var(soundLabelsMb), nullptr);


	ValueTree featuresUsedTree("FeaturesUsed");
	
	for (auto featureFramePair : featuresUsed)
	{
		ValueTree pair("FeatureFramePair");
		pair.setProperty("Frame", var(featureFramePair.first), nullptr);
		pair.setProperty("Feature", var(static_cast<int>(featureFramePair.second)), nullptr);

		featuresUsedTree.addChild(pair, -1, nullptr);
	}

	vt.addChild(featuresUsedTree, -1, nullptr);


	FileOutputStream os(file);
	
	if (os.getStatus().failed())
	{
		errorString = "Error creating file";
		os.flush();
		return false;
	}

	vt.writeToStream(os);
	os.flush();

	return true;
}

//==============================================================================
template<typename T>
AudioDataSet<T> AudioDataSet<T>::getVarianceReducedCopy(int numFeatures)
{
	arma::Col<T> variances = arma::var(data, 0, 1);

	arma::uvec sorted = arma::sort_index(variances);
	
	arma::Mat<T> reducedData(numFeatures, data.n_cols);
	std::vector<FeatureFramePair> reducedFeaturesUsed;

	for (auto i = 0; i < numFeatures; ++i)
	{
		reducedData.row(i) = data.row(sorted[i]);
		reducedFeaturesUsed.push_back(featuresUsed[sorted[i]]);
	}

	AudioDataSet reduced(numSounds, instancesPerSound, bufferSize,
		stftFramesPerBuffer, numDelayedBuffers);

	reduced.data = reducedData;
	reduced.soundLabels = soundLabels;
	reduced.featuresUsed = reducedFeaturesUsed;

	return reduced;
}

//==============================================================================
template<typename T>
void AudioDataSet<T>::addInstance(const arma::Col<T>& instance, int soundLabel)
{
	assert(instance.n_rows == data.n_rows);
	
	auto totalInstances = numSounds * instancesPerSound;

	//May change to assertion. 
	if (totalInstances == 0)
		return;

	if (instanceCount == 0)
		instanceCount = soundLabel * instancesPerSound;

	if (instanceCount < instancesPerSound * (soundLabel + 1))
	{
		data.col(instanceCount) = instance;
		soundLabels[instanceCount] = soundLabel;

		++instanceCount;
	}
	else
	{
		soundsReady[soundLabel] = true;
		instanceCount = 0;
	}
	
}

//==============================================================================
template<typename T>
bool AudioDataSet<T>::usingFeature(int stftFrameNumber, AudioClassifyOptions::AudioFeature feature)
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
int AudioDataSet<T>::getFeatureRowIndex(int stftFrameNumber, AudioClassifyOptions::AudioFeature feature)
{
	for (auto i = 0; i < featuresUsed.size(); ++i)
	{
		if (featuresUsed[i].first == stftFrameNumber && featuresUsed[i].second == feature)
			return i;
	}

	//Feature not being used.
	return -1;
}

//==============================================================================
template<typename T>
std::vector<FeatureFramePair> AudioDataSet<T>::getFeaturesUsed() const
{
	return featuresUsed;
}

//==============================================================================
template<typename T>
void AudioDataSet<T>::setFeaturesUsed(const std::vector<FeatureFramePair>& newFeaturesUsed)
{
	auto numFeatures = newFeaturesUsed.size();
	arma::Mat<T> reducedData(numFeatures, getTotalNumInstances());

	for (auto i = 0; i < numFeatures; ++i)
	{
		auto feature = newFeaturesUsed[i];
		auto index = getFeatureRowIndex(feature.first, feature.second);
		
		reducedData.row(i) = data.row(index);
	}

	data.copy_size(reducedData);
	data = reducedData;

	featuresUsed.resize(0);
	featuresUsed = newFeaturesUsed;
}
//==============================================================================
template<typename T>
const arma::Mat<T>& AudioDataSet<T>::getData() const
{
	return data;
}

//==============================================================================
template<typename T>
const arma::Row<int>& AudioDataSet<T>::getSoundLabels() const
{
	return soundLabels;
}

//==============================================================================
template<typename T>
int AudioDataSet<T>::getBufferSize() const
{
	return bufferSize;
}


//==============================================================================
template<typename T>
int AudioDataSet<T>::getSTFTFramesPerBuffer() const
{
	return stftFramesPerBuffer;
}

//==============================================================================
template<typename T>
int AudioDataSet<T>::getNumDelayedBuffers() const
{
	return numDelayedBuffers;
}

//==============================================================================
template<typename T>
int AudioDataSet<T>::getNumSounds() const
{
	return numSounds;
}

//==============================================================================
template<typename T>
int AudioDataSet<T>::getInstancesPerSound() const
{
	return instancesPerSound;
}

//==============================================================================
template<typename T>
int AudioDataSet<T>::getNumFeatures() const
{
	return static_cast<int>(featuresUsed.size());
}

//==============================================================================
template<typename T>
int AudioDataSet<T>::getTotalNumInstances() const
{
	return numSounds * instancesPerSound;
}

//==============================================================================
template<typename T>
int AudioDataSet<T>::getTotalNumSTFTFrames() const
{
	return stftFramesPerBuffer * (numDelayedBuffers + 1);
}

//==============================================================================
template<typename T>
bool AudioDataSet<T>::isReady() const
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
bool AudioDataSet<T>::checkSoundReady(const int sound) const
{
	return soundsReady[sound];
}

//==============================================================================
template<typename T>
void AudioDataSet<T>::setData(const arma::Mat<T>& newData)
{
	data.clear();
	data = newData;
}

//==============================================================================
template<typename T>
void AudioDataSet<T>::setSoundLabels(const arma::Row<int>& newLabels)
{
	soundLabels.clear();
	soundLabels = newLabels;
}

//==============================================================================
template<typename T>
void AudioDataSet<T>::initialise()
{
	auto totalInstances = numSounds * instancesPerSound;
	auto totalFrames = stftFramesPerBuffer * (numDelayedBuffers + 1);

	soundLabels.set_size(totalInstances);
	soundLabels.fill(-1);

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
