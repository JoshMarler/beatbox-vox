/*
   ==============================================================================

    AudioClassifier.cpp
    Created: 9 Jul 2016 3:49:01pm
    Author:  joshua

    ==============================================================================
*/

#include "AudioClassifier.h"
#include <cmath>
#include "../FeatureExtractor/FeatureExtractor.h"

//==============================================================================
template<typename T>
AudioClassifier<T>::AudioClassifier(int initBufferSize, T initSampleRate, int initNumSounds, int initNumTrainingInstances)
	: gistOSD(initBufferSize, static_cast<int>(initSampleRate)), //JWM - Eventually initialise with slice size?
	  osDetector(initBufferSize / 2, initSampleRate),
	  featureExtractor(initBufferSize, static_cast<int>(initSampleRate)),
	  nbc(initNumSounds, 21),
	  knn(21, initNumSounds, initNumTrainingInstances)
{
	bufferSize = initBufferSize;
	sampleRate = initSampleRate;

	setupStft();

	numSounds = initNumSounds;
	trainingInstancesPerSound = initNumTrainingInstances;

	//Set initial num test instance per sound to 10
	testInstancesPerSound = 10;

	currentClassfierType.store(AudioClassifyOptions::ClassifierType::naiveBayes);

	configureDataSets();
}

//==============================================================================
template<typename T>
AudioClassifier<T>::~AudioClassifier()
{

}

//==============================================================================
template<typename T>
int AudioClassifier<T>::getCurrentBufferSize() const
{
    return bufferSize;
}

//==============================================================================
template<typename T>
T AudioClassifier<T>::getCurrentSampleRate() const
{
    return sampleRate;
}

//==============================================================================
template<typename T>
void AudioClassifier<T>::setCurrentBufferSize (int newBufferSize)
{
	bufferSize = newBufferSize;

	configureDataSets();
	
	//Update STFT frame size relative to new bufferSize.
	setupStft();

	magSpectrumOSD.reset(new T[bufferSize / 2]);    
	std::fill(magSpectrumOSD.get(), (magSpectrumOSD.get() + (bufferSize / 2)), static_cast<T>(0.0));

	gistOSD.setAudioFrameSize(bufferSize);

}

//==============================================================================
template<typename T>
void AudioClassifier<T>::setCurrentSampleRate (T newSampleRate)
{
    sampleRate = newSampleRate;
    gistOSD.setSamplingFrequency(static_cast<int>(sampleRate));
	featureExtractor.setSampleRate(static_cast<int>(sampleRate));
	osDetector.setSampleRate(sampleRate);
}

//==============================================================================
template<typename T>
bool AudioClassifier<T>::saveDataSet(const std::string & fileName, AudioClassifyOptions::DataSetType dataSetType, std::string & errorString)
{
	if (dataSetType == AudioClassifyOptions::DataSetType::trainingSet)
		return trainingSet->save(fileName, errorString);
	
	if (dataSetType == AudioClassifyOptions::DataSetType::testSet)
		return testSet->save(fileName, errorString);

	//Invalid dataSetType
	errorString = "Error saving: Invalid DataSetType";
	return false;
}

//==============================================================================
template<typename T>
bool AudioClassifier<T>::loadDataSet(const std::string & fileName, AudioClassifyOptions::DataSetType dataSetType, std::string & errorString)
{
	resetClassifierState();

	if (dataSetType == AudioClassifyOptions::DataSetType::trainingSet)
	{
		auto success = trainingSet->load(fileName, errorString);

		if (success)
		{
			//JWM - Check buffer sizes match?
			stftFramesPerBuffer = trainingSet->getSTFTFramesPerBuffer();
			numDelayedBuffers = trainingSet->getNumDelayedBuffers();
			numSounds = trainingSet->getNumSounds();
			trainingInstancesPerSound = trainingSet->getInstancesPerSound();

			setupStft();

			//Some duplication here that might be good to remove in future.
			trainingSetReduced.reset(nullptr);
			testSetReduced.reset(nullptr);

			currentInstanceVector.set_size(trainingSet->getNumFeatures());

			knn.setNumFeatures(trainingSet->getNumFeatures());
			knn.setTrainingInstancesPerClass(trainingInstancesPerSound);
			nbc.setNumFeatures(trainingSet->getNumFeatures());
		}

		return success;
	}
	
	//JWM - Need to add checks to confirm test set has same structure as trainingSet
	//Could add a checkStructuresMatch or something ?
	if (dataSetType == AudioClassifyOptions::DataSetType::testSet)
	{
		//Also check buffer size same?	
		auto success = testSet->load(fileName, errorString);

		if (success)
		{
			testInstancesPerSound = testSet->getInstancesPerSound();
			testSetReduced.reset(nullptr);
		}

		return success;
	}

	//Invalid dataSetType
	errorString = "Error saving: Invalid DataSetType";
	return false;
}

//==============================================================================
template<typename T>
size_t AudioClassifier<T>::getNumSounds() const
{
    return numSounds;
}

//==============================================================================
template<typename T>
int AudioClassifier<T>::getCurrentSoundRecording() const
{
	if (recordingTrainingData.load() || recordingTestData.load())
		return currentSoundRecording.load();

	//Not recording any sound currently
	return -1;
}

//==============================================================================
template<typename T>
void AudioClassifier<T>::setOSDMeanCoeff(T newMeanCoeff)
{
    osDetector.setMeanCoefficient(newMeanCoeff);
}

//==============================================================================
template<typename T>
void AudioClassifier<T>::setOSDMedianCoeff(T newMedianCoeff)
{
    osDetector.setMedianCoefficient(newMedianCoeff);
}

//==============================================================================
template<typename T>
void AudioClassifier<T>::setOSDNoiseRatio(T newNoiseRatio)
{
    osDetector.setNoiseRatio(newNoiseRatio);
}

//==============================================================================
template<typename T>
void AudioClassifier<T>::setOSDMsBetweenOnsets(int ms)
{
    osDetector.setMinMsBetweenOnsets(ms);
}

//==============================================================================
template<typename T>
void AudioClassifier<T>::setOSDDetectorFunctionType(AudioClassifyOptions::ODFType newODFType)
{
    osDetector.setCurrentODFType(newODFType);
}

//==============================================================================
template<typename T>
bool AudioClassifier<T>::getOSDUsingAdaptiveWhitening()
{
	return osDetector.getUsingAdaptiveWhitening();
}

//==============================================================================
template<typename T>
void AudioClassifier<T>::setOSDUseAdaptiveWhitening(bool use)
{
	osDetector.setUsingAdaptiveWhitening(use);
}

//==============================================================================
template<typename T>
void AudioClassifier<T>::setOSDWhitenerPeakDecayRate(unsigned int newDecayRate)
{
	osDetector.setWhitenerPeakDecayRate(newDecayRate);
}

//==============================================================================
template<typename T>
bool AudioClassifier<T>::getOSDUsingLocalMaximum()
{
	return osDetector.getUsingLocalMaximum();
}

//==============================================================================
template<typename T>
void AudioClassifier<T>::setOSDUseLocalMaximum(bool use)
{
	osDetector.setUsingLocalMaximum(use);
}

//==============================================================================
template<typename T>
void AudioClassifier<T>::setNumBuffersDelayed(unsigned int newNumDelayed)
{
	numDelayedBuffers = newNumDelayed;

	configureDataSets();
}

//==============================================================================
template<typename T>
int AudioClassifier<T>::getNumBuffersDelayed() const
{
	return numDelayedBuffers;
}

//==============================================================================
template<typename T>
void AudioClassifier<T>::setSTFTFramesPerBuffer(const unsigned int newNumSTFTFrames)
{
	stftFramesPerBuffer = newNumSTFTFrames;
	setupStft();

	configureDataSets();
}

//==============================================================================
template<typename T>
int AudioClassifier<T>::getSTFTFramesPerBuffer() const
{
	return stftFramesPerBuffer;
}

//==============================================================================
template<typename T>
int AudioClassifier<T>::getSTFTFrameSize() const
{
	return bufferSize / stftFramesPerBuffer;
}

//==============================================================================
template<typename T>
void AudioClassifier<T>::setClassifierType(AudioClassifyOptions::ClassifierType classifierType)
{
	currentClassfierType.store(classifierType);
}

//==============================================================================
template<typename T>
AudioClassifyOptions::ClassifierType AudioClassifier<T>::getClassifierType() const
{
	return currentClassfierType.load();
}

//==============================================================================
//NOTE: revist later - will need assertion if user uses sound value out of range 0 - numSounds
template<typename T>
void AudioClassifier<T>::setSoundRecording(int sound, AudioClassifyOptions::DataSetType dataSetType)
{
	classifierReady.store(false);

    currentSoundRecording.store(sound);
	
	if (dataSetType == AudioClassifyOptions::DataSetType::trainingSet)
	{
		//Cannot record test set and training set data at same time. Should be seperate instances.
		recordingTestData.store(false);
		recordingTrainingData.store(true);
	}
	else
	{
		recordingTestData.store(true);
		recordingTrainingData.store(false);
	}
	
	//Need to set AudioDataSet sound not ready ? 
	//trainingSoundsReady[currentTrainingSoundRecording.load()] = false;
}

//==============================================================================
template<typename T>
void AudioClassifier<T>::train()
{
    //If all sound samples collected for training set train model.
    if (trainingSet->isReady())
    {
		if (reducedVarianceSize > 0)
		{
			nbc.Train(trainingSetReduced->getData(), trainingSetReduced->getSoundLabels());
			knn.train(trainingSetReduced->getData(), trainingSetReduced->getSoundLabels());
		}
		else
		{
			nbc.Train(trainingSet->getData(), trainingSet->getSoundLabels()); 
			knn.train(trainingSet->getData(), trainingSet->getSoundLabels());
		}


        classifierReady.store(true);    
    }

    //JWM - Potentially return boolean and return false if checkTrainingSetReady() returns false.
}

//==============================================================================
template<typename T>
void AudioClassifier<T>::setInstancesPerSound(int newNumInstances, AudioClassifyOptions::DataSetType dataSetType)
{

	if (dataSetType == AudioClassifyOptions::DataSetType::trainingSet)
	{
		resetClassifierState();
		trainingInstancesPerSound = newNumInstances;
		trainingSet.reset(new AudioDataSet<T>(numSounds, trainingInstancesPerSound, bufferSize, stftFramesPerBuffer, numDelayedBuffers));
		trainingSetReduced.reset(nullptr);
		knn.setTrainingInstancesPerClass(newNumInstances);
	}

	if (dataSetType == AudioClassifyOptions::DataSetType::testSet)
	{
		testInstancesPerSound = newNumInstances;
		testSet.reset(new AudioDataSet<T>(numSounds, testInstancesPerSound, bufferSize, stftFramesPerBuffer, numDelayedBuffers));
		testSetReduced.reset(nullptr);
	}

}

//==============================================================================
template<typename T>
int AudioClassifier<T>::getInstancesPerSound(AudioClassifyOptions::DataSetType dataSetType)
{
	if (dataSetType == AudioClassifyOptions::DataSetType::trainingSet)
		return trainingSet->getInstancesPerSound();

	if (dataSetType == AudioClassifyOptions::DataSetType::testSet)
		return testSet->getInstancesPerSound();

	//Invalid dataSetType
	return -1;
}

//==============================================================================
template<typename T>
int AudioClassifier<T>::getDataSetSize(AudioClassifyOptions::DataSetType dataSetType) const
{
	if (dataSetType == AudioClassifyOptions::DataSetType::trainingSet)
		return trainingSet->getTotalNumInstances();

	if (dataSetType == AudioClassifyOptions::DataSetType::testSet)
		return testSet->getTotalNumInstances();

	//Invalid dataSetType
	return -1;
}

//==============================================================================
template<typename T>
bool AudioClassifier<T>::getClassifierReady() const
{
    return classifierReady.load();
}

//==============================================================================
template<typename T>
bool AudioClassifier<T>::isRecording() const
{
	if (recordingTrainingData.load())
		return true;
	if (recordingTestData.load())
		return true;

	return false;
}

//==============================================================================
template<typename T> 
void AudioClassifier<T>::processAudioBuffer (const T* buffer, const int numSamples)
{
    const auto currentBufferSize = getCurrentBufferSize();
	
	if (numDelayedBuffers == 0 || delayedProcessedCount == 0)
		 hasOnset = false;

    /** if (bufferSize != numSamples) */

    /** { */
    /**     //setCurrentFrameSize() needs to be called before continuing processing - training set/model will be invalid strictly speaking. */
    /**     return; */
    /** } */
	
	if (delayedProcessedCount == 0)
	{
		gistOSD.processAudioFrame(buffer, bufferSize);
		gistOSD.getMagnitudeSpectrum(magSpectrumOSD.get());
		hasOnset = osDetector.checkForOnset(magSpectrumOSD.get(), bufferSize / 2);
	}

	if (hasOnset)
	{
		while (stftProcessedCount < stftFramesPerBuffer)
		{
			//Later allow use of hopSize/overlap ?
			auto stftFrameSize = bufferSize / stftFramesPerBuffer;
			auto readPosition = stftFrameSize * stftProcessedCount;
			auto* readPtr = buffer + readPosition;
				
			featureExtractor.processFrame(readPtr, stftFrameSize);

			if (reducedVarianceSize > 0 && !isRecording())
				processCurrentInstanceReduced();
			else
				processCurrentInstance();

			++stftProcessedCount;
		}

		//Reset for next instance/onset
		stftProcessedCount = 0;

		if (numDelayedBuffers != 0 && delayedProcessedCount < numDelayedBuffers)
			++delayedProcessedCount;
		else
			delayedProcessedCount = 0;
	}

}

//==============================================================================
template<typename T>
void AudioClassifier<T>::setupStft()
{
		auto stftFrameSize = bufferSize / stftFramesPerBuffer;
		featureExtractor.setFrameSize(stftFrameSize);
}

//==============================================================================
template<typename T>
void AudioClassifier<T>::processCurrentInstance()
{
	auto instanceReady = false;
	auto frameNumber = (stftProcessedCount + 1) + (stftFramesPerBuffer * delayedProcessedCount);

	for (auto i = 0; i < AudioClassifyOptions::totalNumAudioFeatures; ++i)
	{
		auto feature = static_cast<AudioClassifyOptions::AudioFeature>(i);
		if (trainingSet->usingFeature(frameNumber, feature))
		{
			auto featureVal = featureExtractor.getFeature(feature);
			auto featureIndex = trainingSet->getFeatureIndex(frameNumber, feature);

			currentInstanceVector[featureIndex] = featureVal;

			++featuresProcessedCount;
		}
	}


	if (featuresProcessedCount == trainingSet->getNumFeatures())
	{
		instanceReady = true;
		featuresProcessedCount = 0;
	}


	if (instanceReady)
	{
		if (recordingTrainingData.load())
		{
			if (trainingSet->checkSoundReady(currentSoundRecording.load()))
			{
				recordingTrainingData.store(false);
				currentSoundRecording.store(-1);
			}
			else
				trainingSet->addInstance(currentInstanceVector, currentSoundRecording.load());
		}

		if (recordingTestData.load())
		{
			if (testSet->checkSoundReady(currentSoundRecording.load()))
			{
				recordingTestData.store(false);
				currentSoundRecording.store(-1);
			}
			else
				testSet->addInstance(currentInstanceVector, currentSoundRecording.load());
		}
	}

}

//==============================================================================
template<typename T>
void AudioClassifier<T>::processCurrentInstanceReduced()
{
	auto frameNumber = (stftProcessedCount + 1) + (stftFramesPerBuffer * delayedProcessedCount);

	for (auto i = 0; i < AudioClassifyOptions::totalNumAudioFeatures; ++i)
	{
		auto feature = static_cast<AudioClassifyOptions::AudioFeature>(i);
		if (trainingSetReduced->usingFeature(frameNumber, feature))
		{
			auto featureVal = featureExtractor.getFeature(feature);
			auto featureIndex = trainingSetReduced->getFeatureIndex(frameNumber, feature);

			currentInstanceVectorReduced[featureIndex] = featureVal;

			++featuresProcessedCountReduced;
		}
	}

	if (featuresProcessedCountReduced == trainingSetReduced->getNumFeatures())
		featuresProcessedCountReduced = 0;
}

//==============================================================================
template<typename T>
void AudioClassifier<T>::resetClassifierState()
{
    classifierReady.store(false);

	reducedVarianceSize = 0;
	featuresProcessedCount = 0;
	featuresProcessedCountReduced = 0;

	currentSoundRecording.store(-1);

	//NOTE: May remove these as set when recording instance for sound complete. 
	recordingTrainingData.store(false);
	recordingTestData.store(false);
}

//==============================================================================

template<typename T>
bool AudioClassifier<T>::noteOnsetDetected() const
{
	//Don't want to trigger onset response in middle of delayed evaluation handling
	if (delayedProcessedCount == 0)
		return hasOnset;
	else
		return false;
}

//==============================================================================
template<typename T>
int AudioClassifier<T>::classify()
{
	auto sound = -1;

    auto ready = classifierReady.load();
      
    if (!ready || isRecording())
        return -1;
   
    if (noteOnsetDetected())
    {
	    switch (currentClassfierType.load())
	    {
			case AudioClassifyOptions::ClassifierType::nearestNeighbour:
				if (reducedVarianceSize > 0)
					sound = knn.classify(currentInstanceVectorReduced);
				else
					sound = knn.classify(currentInstanceVector);
				break;
			case AudioClassifyOptions::ClassifierType::naiveBayes:
				if (reducedVarianceSize > 0)
					sound = nbc.Classify(currentInstanceVectorReduced);
				else
					sound = nbc.Classify(currentInstanceVector);
				break;
			default: break; // Sound returned -1 (Invalid label. Valid labels are 0 to numSounds)
	    }
    }

    return sound;
}

//==============================================================================
template<typename T>
void AudioClassifier<T>::reduceFeaturesByVariance(unsigned numFeaturesToTake)
{
	//NOTE: Should we check whether training set is ready and return void?

	auto numFeaturesToUse = 0;

	resetClassifierState();

	//NOTE: - Need to check this. Torn state issues etc.
	trainingSetReduced.reset(new AudioDataSet<T>(trainingSet->getVarianceReducedCopy(numFeaturesToTake)));


	//Set Current Instance Vector;
	if (numFeaturesToTake > 0)
	{
		numFeaturesToUse = numFeaturesToTake;
		currentInstanceVectorReduced.set_size(numFeaturesToTake);
		currentInstanceVectorReduced.zeros();
	}
	else
	{
		numFeaturesToUse = trainingSet->getNumFeatures();
		currentInstanceVectorReduced.clear();
	}


	reducedVarianceSize = numFeaturesToTake;

	nbc.setNumFeatures(numFeaturesToUse);
	knn.setNumFeatures(numFeaturesToUse);
}

//==============================================================================
template<typename T>
int AudioClassifier<T>::getNumFeaturesUsed()
{
	if (reducedVarianceSize > 0)
		return reducedVarianceSize;
	else
		return trainingSet->getNumFeatures();
}

//==============================================================================
template<typename T>
bool AudioClassifier<T>::checkDataSetReady(AudioClassifyOptions::DataSetType dataSetType) const
{
	if (dataSetType == AudioClassifyOptions::DataSetType::trainingSet)
		return trainingSet->isReady();

	if (dataSetType == AudioClassifyOptions::DataSetType::testSet)
		return testSet->isReady();

	return false;
}

//==============================================================================
template<typename T>
bool AudioClassifier<T>::checkSoundReady(int sound, AudioClassifyOptions::DataSetType dataSetType) const
{
	if (dataSetType == AudioClassifyOptions::DataSetType::trainingSet)
		return trainingSet->checkSoundReady(sound);

	if (dataSetType == AudioClassifyOptions::DataSetType::testSet)
		return testSet->checkSoundReady(sound);

	return false;
}

//==============================================================================
template<typename T>
void AudioClassifier<T>::configureDataSets()
{
	resetClassifierState();

	trainingSet.reset(new AudioDataSet<T>(numSounds, trainingInstancesPerSound, bufferSize, stftFramesPerBuffer, numDelayedBuffers));
	testSet.reset(new AudioDataSet<T>(numSounds, testInstancesPerSound, bufferSize, stftFramesPerBuffer, numDelayedBuffers));

	currentInstanceVector.set_size(trainingSet->getNumFeatures());
	currentInstanceVector.zeros();

	trainingSetReduced.reset(nullptr);
	testSetReduced.reset(nullptr);
	currentInstanceVectorReduced.clear();


	knn.setNumFeatures(trainingSet->getNumFeatures());
	nbc.setNumFeatures(trainingSet->getNumFeatures());
}

//==============================================================================
template<typename T>
float AudioClassifier<T>::test(std::vector<std::pair<unsigned int, unsigned int>>& outputResults)
{
	//NOTE: Possibly add an error string input param if test set not ready
	if (testSet->isReady())
		return 0.0f;

	unsigned int numCorrect = 0;

	for (auto i = 0; i < testSet->getTotalNumInstances(); ++i)
	{
		arma::Col<T> testInstance = testSet->getData().col(i);
		auto actual = testSet->getSoundLabels()[i];
		auto predicted = -1;
		
	    switch (currentClassfierType.load())
	    {
			case AudioClassifyOptions::ClassifierType::nearestNeighbour:
				predicted = knn.classify(testInstance);
				break;
			case AudioClassifyOptions::ClassifierType::naiveBayes:
				predicted = nbc.Classify(testInstance);
				break;
			default: break; 
	    }

		if (actual == predicted)
			++numCorrect;

		outputResults.push_back(std::make_pair(actual, predicted));
	}

	//Return percentage accuracy
	auto result = static_cast<float>(numCorrect) / static_cast<float>(testSet->getTotalNumInstances()) * 100.0f;
	return result;
}


//==============================================================================
template class AudioClassifier<float>;
template class AudioClassifier<double>;
