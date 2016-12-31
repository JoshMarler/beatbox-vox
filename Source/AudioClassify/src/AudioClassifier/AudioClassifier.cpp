/*
   ==============================================================================

    AudioClassifier.cpp
    Created: 9 Jul 2016 3:49:01pm
    Author:  joshua

    ==============================================================================
*/

#include "AudioClassifier.h"
#include <fstream>
#include <cmath>
#include "../FeatureExtractor/FeatureExtractor.h"

//==============================================================================
template<typename T>
AudioClassifier<T>::AudioClassifier(int initBufferSize, T initSampleRate, int initNumSounds, int initNumInstances)
	: gistOSD(initBufferSize, static_cast<int>(initSampleRate)), //JWM - Eventually initialise with slice size?
	  osDetector(initBufferSize / 2, initSampleRate),
	  featureExtractor(initBufferSize, static_cast<int>(initSampleRate)),
	  nbc(initNumSounds, 21),
	  knn(21, initNumSounds, initNumInstances),
	  trainingSet(initNumSounds, initNumInstances, initBufferSize),
	  testSet(initNumSounds, 10, initBufferSize)

{
	bufferSize = initBufferSize;
	sampleRate = initSampleRate;

	numTrainingInstances = initNumInstances;
    numSounds = initNumSounds;

	trainingSetSize = numSounds * numTrainingInstances;

	//Set initial num test instance per sound to 10
	numTestInstances = 10;
	testSetSize = numSounds * numTestInstances;

	numFeatures = calcFeatureVecSize();

	currentClassfierType.store(AudioClassifyOptions::ClassifierType::naiveBayes);

    //Set initial sound ready states to false in training set.  
    trainingSoundsReady.resize(numSounds, false);
	testSoundsReady.resize(numSounds, false);

	configTrainingSetMatrix();
	configTestSetMatrix();
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
	
	magSpectrumOSD.reset(new T[bufferSize / 2]);    
	std::fill(magSpectrumOSD.get(), (magSpectrumOSD.get() + (bufferSize / 2)), static_cast<T>(0.0));

	gistOSD.setAudioFrameSize(bufferSize);

	//Update STFT frame size relative to new bufferSize.
	setupStft();

	//If changing num buffers current training set no longer valid so reset and require re-train.
	resetClassifierState();
}

//==============================================================================
template<typename T>
void AudioClassifier<T>::setCurrentSampleRate (T newSampleRate)
{
    sampleRate = newSampleRate;
    gistOSD.setSamplingFrequency(static_cast<int>(sampleRate));
	gistFeaturesStft.setSamplingFrequency(static_cast<int>(sampleRate));
	osDetector.setSampleRate(sampleRate);
}

//==============================================================================
template<typename T>
bool AudioClassifier<T>::saveDataSet(const std::string & fileName, AudioClassifyOptions::DataSetType dataSetType, std::string & errorString)
{
	if (dataSetType == AudioClassifyOptions::DataSetType::trainingSet)
		return trainingSet.save(fileName, errorString);
	
	if (dataSetType == AudioClassifyOptions::DataSetType::testSet)
		return testSet.save(fileName, errorString);

	//Invalid dataSetType
	errorString = "Error saving: Invalid DataSetType";
	return false;
}

//==============================================================================
template<typename T>
bool AudioClassifier<T>::loadDataSet(const std::string & fileName, AudioClassifyOptions::DataSetType dataSetType, std::string & errorString)
{

	if (dataSetType == AudioClassifyOptions::DataSetType::trainingSet)
		return trainingSet.load(fileName, errorString);
	
	if (dataSetType == AudioClassifyOptions::DataSetType::testSet)
		return testSet.load(fileName, errorString);

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
	
	//Total num features depends on numDelayedBuffers
	updateFeatures();

	//Re-configure training set matrix and reset model state as new feature/attribute rows required
	configTrainingSetMatrix();
	configTestSetMatrix();
}

//==============================================================================
template<typename T>
int AudioClassifier<T>::getNumBuffersDelayed() const
{
	return numDelayedBuffers;
}

//==============================================================================
template<typename T>
void AudioClassifier<T>::setNumSTFTFrames(const unsigned int newNumSTFTFrames)
{
	numStftFrames = newNumSTFTFrames;
	setupStft();

	updateFeatures();

	configTrainingSetMatrix();
	configTestSetMatrix();
}

//==============================================================================
template<typename T>
int AudioClassifier<T>::getNumSTFTFrames() const
{
	return numStftFrames;
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
//JWM - NOTE: revist later - will need assertion if user uses sound value out of range 0 - numSounds
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
    if (checkTrainingSetReady())
    {
        nbc.Train(trainingData, trainingLabels); 
		knn.train(trainingData, trainingLabels);

        classifierReady.store(true);    
    }

    //JWM - Potentially return boolean and return false if checkTrainingSetReady() returns false.
}

template<typename T>
void AudioClassifier<T>::setInstancesPerSound(int newNumInstances, AudioClassifyOptions::DataSetType dataSetType)
{
	if (dataSetType == AudioClassifyOptions::DataSetType::trainingSet)
		trainingSet.setInstancesPerSound(newNumInstances);

	if (dataSetType == AudioClassifyOptions::DataSetType::testSet)
		testSet.setInstancesPerSound(newNumInstances);

	knn.setTrainingInstancesPerClass(newNumInstances);

	//JWM: MAY or may not need to add this to set classifierReady = false
	resetClassifierState();
}

//==============================================================================
template<typename T>
int AudioClassifier<T>::getInstancesPerSound(AudioClassifyOptions::DataSetType dataSetType)
{
	if (dataSetType == AudioClassifyOptions::DataSetType::trainingSet)
		trainingSet.getInstancesPerSound();

	if (dataSetType == AudioClassifyOptions::DataSetType::testSet)
		testSet.getInstancesPerSound();

	//Invalid dataSetType
	return -1;
}

//==============================================================================
template<typename T>
int AudioClassifier<T>::getTrainingSetSize() const
{
	return trainingSetSize;
}

//==============================================================================
template<typename T>
int AudioClassifier<T>::getTestSetSize() const
{
	return testSetSize;
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
		if (numStftFrames > 0)
			processSTFTFrame(buffer);
		else
		{
			featureExtractor.processFrame(buffer, bufferSize);
			processCurrentInstance();
		}

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
	if (numStftFrames != 0)
	{
		stftFrameSize = bufferSize / numStftFrames;
		featureExtractor.setAudioFrameSize(stftFrameSize);
	}
}

//==============================================================================
template<typename T>
void AudioClassifier<T>::processSTFTFrame(const T* inputBuffer)
{
	
	while (stftProcessedCount < numStftFrames)
	{
		//Later allow use of hopSize/overlap ?
		auto readPosition = stftFrameSize * stftProcessedCount;
		auto* readPtr = inputBuffer + readPosition;
			
		featureExtractor.processFrame(readPtr, stftFrameSize);

		processCurrentInstance();

		++stftProcessedCount;
	}

	//Reset state for next instance/onset
	stftProcessedCount = 0;
}

//==============================================================================
template<typename T>
void AudioClassifier<T>::processCurrentInstance()
{
	auto featuresProcessed = 0;
	auto instanceReady = false;
	auto frameNumber = stftProcessedCount + 1;

	//Features used by trainingSet and testSet should always match.
	if (numStftFrames != 0)
		featuresProcessed = (stftProcessedCount + (delayedProcessedCount * numStftFrames)) * trainingSet.getNumFeatures();
	else
		featuresProcessed = delayedProcessedCount * trainingSet.getNumFeatures();


	for (auto i = 0; i < AudioClassifyOptions::totalNumAudioFeatures; ++i)
	{
		auto feature = static_cast<AudioClassifyOptions::AudioFeature>(i);
		if (trainingSet.usingFeature(frameNumber, feature))
		{
			auto featureVal = featureExtractor.getFeature(feature, frameNumber);
			auto featureIndex = trainingSet.getFeatureIndex(frameNumber, feature);

			currentInstanceVector[featureIndex] = featureVal;

			//Replaces pos
			++featuresProcessed;
		}
	}


	if (featuresProcessed == trainingSet.getNumFeatures())
		instanceReady = true;


	if (instanceReady)
	{
		if (recordingTrainingData.load())
		{
			if (trainingSet.checkSoundReady(currentSoundRecording.load()))
			{
				recordingTrainingData.store(false);
				currentSoundRecording.store(-1);
			}
			else
				trainingSet.addInstance(currentInstanceVector);
		}
		else if (recordingTestData.load())
		{
			if (testSet.checkSoundReady(currentSoundRecording.load()))
			{
				recordingTestData.store(false);
				currentSoundRecording.store(-1);
			}
			else
				testSet.addInstance();
		}
	}

}

//==============================================================================
template<typename T>
void AudioClassifier<T>::resetClassifierState()
{
    classifierReady.store(false);
	currentTrainingSoundRecording.store(-1);
	recordingTrainingData.store(false);
	trainingCount = 0;

	for (auto& v : trainingSoundsReady)
		  v = false;
}

//==============================================================================
template<typename T>
void AudioClassifier<T>::resetTestState()
{
	currentTestSoundRecording.store(-1);
	recordingTestData.store(false);
	testCount = 0; 

	for (auto& v : testSoundsReady)
		  v = false;
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
      
    if (!ready)
        return -1;
   
    if (noteOnsetDetected())
    {
	    switch (currentClassfierType.load())
	    {
			case AudioClassifyOptions::ClassifierType::nearestNeighbour:
				sound = knn.classify(currentInstanceVector);
				break;
			case AudioClassifyOptions::ClassifierType::naiveBayes:
				sound = nbc.Classify(currentInstanceVector);
				break;
			default: break; // Sound returned -1 (Invalid label. Valid labels are 0 to numSounds)
	    }
    }

    return sound;
}

//==============================================================================
template<typename T>
bool AudioClassifier<T>::checkDataSetReady(AudioClassifyOptions::DataSetType dataSetType) const
{
	if (dataSetType == AudioClassifyOptions::DataSetType::trainingSet)
		return trainingSet.isReady();

	if (dataSetType == AudioClassifyOptions::DataSetType::testSet)
		return testSet.isReady();

	return false;
}

//==============================================================================
template<typename T>
bool AudioClassifier<T>::checkSoundReady(int sound, AudioClassifyOptions::DataSetType dataSetType)
{
	if (dataSetType == AudioClassifyOptions::DataSetType::trainingSet)
		return trainingSet.checkSoundReady(sound);

	if (dataSetType == AudioClassifyOptions::DataSetType::testSet)
		return testSet.checkSoundReady(sound);

	return false;
}

//==============================================================================
template<typename T>
void AudioClassifier<T>::configTrainingSetMatrix()
{
	/** Currently a naive implementation as just discards the existing
	 * training set data and requires training set to be re-gathered/populated
	 * followed by re-train of classifier. 
	 */

	//Training set and current instance no longer valid so reset state
	resetClassifierState();

	trainingData.set_size(numFeatures, trainingSetSize);
	trainingData.zeros();

	trainingLabels.set_size(trainingSetSize);

	//Consider making trainingLabel <int> rather than unsigned to init with -1 label vals
	for (auto i = 0; i < trainingLabels.n_elem; ++i)
		 trainingLabels[i] = 0;

	currentInstanceVector.set_size(numFeatures);
	currentInstanceVector.zeros();
}

//==============================================================================
template<typename T>
void AudioClassifier<T>::configTestSetMatrix()
{
	//Test set no longer valid so reset state
	resetTestState();

	testData.set_size(numFeatures, testSetSize);
	testData.zeros();

	testLabels.set_size(testSetSize);

	for (auto i = 0; i < testLabels.n_elem; ++i)
		 testLabels[i] = 0;
}

//==============================================================================
template<typename T>
unsigned int AudioClassifier<T>::calcFeatureVecSize() const
{
    auto size = 0;

	if (usingRMS.load())
		++size;

	if (usingPeakEnergy.load())
		++size;

	if (usingZeroCrossingRate.load())
		++size;

    if (usingSpecCentroid.load())
        ++size; 
    
    if (usingSpecCrest.load())
        ++size;

    if (usingSpecFlatness.load())
        ++size;

    if (usingSpecRolloff.load())
        ++size;

    if (usingSpecKurtosis.load())
        ++size;

    if (usingMfcc.load())
    {
        //JWM - eventually change this to use numMfcc based on user selected AudioClassifyOptions. 
        size += 13;
    }

    return size;
}

//==============================================================================
template<typename T>
void AudioClassifier<T>::updateFeatures()
{
	auto featureSize = calcFeatureVecSize();

	if (numStftFrames > 0)
		numFeatures = featureSize * (numStftFrames * (numDelayedBuffers + 1));
	else
		numFeatures = featureSize * (numDelayedBuffers + 1);

	knn.setNumFeatures(numFeatures);
	nbc.setNumFeatures(numFeatures);
}

//==============================================================================
template<typename T>
float AudioClassifier<T>::test(std::vector<std::pair<unsigned int, unsigned int>>& outputResults)
{
	//Possibly add an error string input param if test set not ready
	if (!checkTestSetReady())
		return 0.0f;

	unsigned int numCorrect = 0;

	for (auto i = 0; i < testSetSize; ++i)
	{
		//JWM - Hopefully replace with AudioDataSet.data.col(i) where AudioDataSet testSet is the member variable
		arma::Col<T> testInstance = testData.col(i);
		auto actual = testLabels[i];
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
	auto result = static_cast<float>(numCorrect) / static_cast<float>(testSetSize) * 100.0f;
	return result;
}


//==============================================================================
template class AudioClassifier<float>;
template class AudioClassifier<double>;
