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

//==============================================================================
template<typename T>
AudioClassifier<T>::AudioClassifier(int initBufferSize, T initSampleRate, int initNumSounds, int initNumInstances)
	: gistFeatures((initBufferSize * (numDelayedBuffers + 1)), static_cast<int>(initSampleRate)),
	  gistFeaturesOSD(initBufferSize, initSampleRate),
	  osDetector((initBufferSize / 2), initSampleRate),
	  nbc(initNumSounds, 21),
	  knn(21, initNumSounds, initNumInstances)
{
	numInstances = initNumInstances;
    numSounds = initNumSounds;

	trainingSetSize = numSounds * numInstances;

	numFeatures = calcFeatureVecSize();

	currentClassfierType.store(AudioClassifyOptions::ClassifierType::nearestNeighbour);

    setCurrentSampleRate(initSampleRate);
    setCurrentBufferSize(initBufferSize);

	/* JWM - Potentially alter later to have an AudioClassifier::Config object/struct
		     passed to the constructor which specifies numSounds, trainingSetSize and 
	         the features to be used via AudioClassify::AudioClassifyOptions.
			 Could also possibly pass a file name to load for model state/training set...
	*/

    auto numCoefficients = gistFeatures.getMFCCNumCoefficients();
    mfccs.reset(new T[numCoefficients]);

    //Set initial sound ready states to false in training set.  
    soundsReady.resize(numSounds, false);

	configTrainingSetMatrix();
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
	
	gistFeaturesOSD.setAudioFrameSize(bufferSize);

	magSpectrumOSD.reset(new T[bufferSize / 2]);    
	std::fill(magSpectrumOSD.get(), (magSpectrumOSD.get() + (bufferSize / 2)), static_cast<T>(0.0));


	const auto delayedBufferSize = bufferSize * (numDelayedBuffers + 1);

	audioBuffer.reset(new T[delayedBufferSize]);
	std::fill(audioBuffer.get(), (audioBuffer.get() + delayedBufferSize), static_cast<T>(0.0));

	gistFeatures.setAudioFrameSize(delayedBufferSize);


	//If changing num delayed buffers current training set no longer valid so reset and require re-train.
	resetClassifierState();
}

//==============================================================================
template<typename T>
void AudioClassifier<T>::setCurrentSampleRate (T newSampleRate)
{
    sampleRate = newSampleRate;
    gistFeatures.setSamplingFrequency(static_cast<int>(sampleRate));
	gistFeaturesOSD.setSamplingFrequency(static_cast<int>(sampleRate));
	osDetector.setSampleRate(sampleRate);
}

//==============================================================================
template<typename T>
bool AudioClassifier<T>::saveTrainingSet(const std::string & fileName, std::string & errorString)
{
	auto success = false;
	std::ofstream outFileStream;
	arma::Mat<T> savedData;

	if (!checkTrainingSetReady())
	{
		errorString = "The training set is not complete. Complete recording of training set/sounds before save";
		return false;
	}

	//Training set ready check passed. Assign to saveable data.
	savedData = trainingData;

	//Insert additional row for training data/instance labels
	savedData.insert_rows(trainingData.n_rows, 1);
	
	for (auto i = 0; i < savedData.n_cols; ++i)
	{
		savedData.row(savedData.n_rows - 1)[i] = trainingLabels[i];
	}

	outFileStream.open(fileName);
	success = savedData.save(outFileStream, arma::file_type::csv_ascii);
	outFileStream.close();

	if (!success)
		errorString = "There was an error saving the training set. Check the filename/path.";

	return success;
}

//==============================================================================
template<typename T>
bool AudioClassifier<T>::loadTrainingSet(const std::string & fileName, std::string & errorString)
{
	auto success = false;
	arma::Mat<T> loadedData;
	std::ifstream inFileStream;
	
	inFileStream.open(fileName);
	success = loadedData.load(inFileStream, arma::file_type::csv_ascii);
	inFileStream.close();
	
	if (success)
	{
		//NOTE: May change the below so that loading an arbitrary training set alters the members like numSounds etc.
		//Confirm the loaded data matches the AudioClassifier object's parameters
		if (loadedData.n_cols != (numSounds * numInstances) && loadedData.n_rows != numFeatures + 1)
		{
			errorString = "The loaded training set did not match the AudioClassifier object's state."
			              "Check the training set loaded matches the following members of the AudioClassifier:"
						  "numSounds (classes), trainingSetSize (instances) and numFeatures (attributes)";
			return false;
		}

		for (auto i = 0; i < trainingData.n_rows; ++i)
		{
			trainingData.row(i) = loadedData.row(i);
		}

		//The last row of the loaded data set will be the training instances class values
		auto labels = loadedData.row(loadedData.n_rows - 1);

		for (auto i = 0; i < trainingLabels.n_cols; ++i)
		{
			trainingLabels[i] = static_cast<arma::u64>(labels[i]);
		}

		/** Note: Eventually will probably add check that there are equal number of training instances
		 *  for each of the classes but this is not necessary for now and can be done when AudioClassify reaches
		 *  library / JUCE Module stage.
		 */

		//Set all sounds as ready so model can be trained.
		for (auto v : soundsReady)
		{
			v = true;
		}

	}

	return success;
}

//==============================================================================
template<typename T>
size_t AudioClassifier<T>::getNumSounds() const
{
    return numSounds;
}

//==============================================================================
template<typename T>
int AudioClassifier<T>::getCurrentTrainingSound() const
{
    return currentTrainingSound.load();
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

	//Need to resize internal buffers according to new numDelayedBuffers
	setCurrentBufferSize(bufferSize);
}

template<typename T>
int AudioClassifier<T>::getNumBuffersDelayed() const
{
	return numDelayedBuffers;
}

//==============================================================================
template<typename T>
void AudioClassifier<T>::setClassifierType(AudioClassifyOptions::ClassifierType classifierType)
{
	currentClassfierType.store(classifierType);

	//NOTE: Eventually it may be worth creating an abstract base class for classifiers and then
	//holding a std::unique_ptr to a bass classifier which gets set to the current derived classifier type.
	//Could help with limiting resource usage etc.
	//Probably also need to check if the classifier is ready and if not check if the training set is ready and call Train()
}

//==============================================================================
//JWM - NOTE: revist later - will need assertion if user uses sound value out of range 0 - numSounds
template<typename T>
void AudioClassifier<T>::recordTrainingSample(int sound)
{
    currentTrainingSound.store(sound);

    trainingCount = (sound * numInstances);

    training.store(true);
}

//==============================================================================
template<typename T>
void AudioClassifier<T>::trainModel()
{
    //If all sound samples collected for training set train model.
    if (checkTrainingSetReady())
    {
		//JWM - may change this later and not train all models at once.
        nbc.Train(trainingData, trainingLabels); 
		knn.train(trainingData, trainingLabels);

        classifierReady.store(true);    
    }

    //JWM - NOTE: Potentially return boolean and return false if checkTrainingSetReady() returns false.
}

//==============================================================================
template<typename T>
void AudioClassifier<T>::setNumInstances(int newNumInstances)
{
    numInstances = newNumInstances;
	trainingSetSize = (numInstances * numSounds);

    //Resize/configure trainingSet matrix
	configTrainingSetMatrix();
}

template<typename T>
int AudioClassifier<T>::getNumInstances() const
{
	 return numInstances;
}

//==============================================================================
template<typename T>
bool AudioClassifier<T>::getClassifierReady() const
{
    return classifierReady.load();
}

//==============================================================================
template<typename T>
bool AudioClassifier<T>::isTraining() const
{
    return training.load();
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
		gistFeaturesOSD.processAudioFrame(buffer, bufferSize);
		gistFeaturesOSD.getMagnitudeSpectrum(magSpectrumOSD.get());
		
		hasOnset = osDetector.checkForOnset(magSpectrumOSD.get(), bufferSize / 2);
	}

	if (hasOnset)
	{
		/* Using std::abs is a bit of a dirty trick here to handle when numDelayedBuffers == 0
		 * Could potentially rethink this at some point.
		 */
		if (numDelayedBuffers != 0 && delayedProcessedCount < numDelayedBuffers)
		{
			auto writeIndex = delayedProcessedCount * bufferSize;
			std::copy(buffer, buffer + bufferSize, audioBuffer.get() + writeIndex);
			
			++delayedProcessedCount;
		}
		else
		{
			if (numDelayedBuffers != 0 && delayedProcessedCount != numDelayedBuffers)
				++delayedProcessedCount;

			auto writeIndex = delayedProcessedCount * bufferSize;
			std::copy(buffer, buffer + bufferSize, audioBuffer.get() + writeIndex);
			
			gistFeatures.processAudioFrame(audioBuffer.get(), bufferSize * (numDelayedBuffers + 1));
			processCurrentInstance();

			//Reset state until next detected onset
			delayedProcessedCount = 0;
		}
				
	}

}

//==============================================================================
template<typename T>
void AudioClassifier<T>::processCurrentInstance()
{
    auto pos = 0;

	if (usingRMS.load())
		currentInstanceVector[pos++] = gistFeatures.rootMeanSquare();

	if (usingPeakEnergy.load())
		currentInstanceVector[pos++] = gistFeatures.peakEnergy();

	if (usingZeroCrossingRate.load())
		currentInstanceVector[pos++] = gistFeatures.zeroCrossingRate();

    if (usingSpecCentroid.load())
        currentInstanceVector[pos++] = gistFeatures.spectralCentroid(); 

    if (usingSpecCrest.load())
        currentInstanceVector[pos++] = gistFeatures.spectralCrest();

    if (usingSpecFlatness.load())
        currentInstanceVector[pos++] = gistFeatures.spectralFlatness(); 

    if (usingSpecRolloff.load())
        currentInstanceVector[pos++] = gistFeatures.spectralRolloff();
    
    if (usingSpecKurtosis.load())
        currentInstanceVector[pos++] = gistFeatures.spectralKurtosis();

    if (usingMfcc.load())
    {
         gistFeatures.melFrequencyCepstralCoefficients(mfccs.get()); 
            
         auto numCoefficients = gistFeatures.getMFCCNumCoefficients();
         for (auto i = 0; i < numCoefficients; i++) 
         { 
           currentInstanceVector[pos++] = mfccs[i];  
         } 
    }
	
    //If currently training update the training set with new instance 
	if (currentTrainingSound.load() != -1 && training.load())
		addToTrainingSet(currentInstanceVector);

}

//==============================================================================
template<typename T>
void AudioClassifier<T>::addToTrainingSet(const arma::Col<T>& newInstance)
{
	classifierReady.store(false);

	auto sound = currentTrainingSound.load();

	if (trainingCount < (numInstances * (sound + 1)))
	{
		trainingData.col(trainingCount) = newInstance;
		trainingLabels[trainingCount] = static_cast<std::size_t>(sound);

		++trainingCount;
	}
	else
	{
		//Set sound ready state to true for current training sound.
		soundsReady[sound] = true;
		training.store(false);
		currentTrainingSound.store(-1);
	}
}

//==============================================================================
template<typename T>
void AudioClassifier<T>::resetClassifierState()
{
	currentTrainingSound.store(-1);
	training.store(false);
	trainingCount = 0;
    classifierReady.store(false);

	for (auto v : soundsReady)
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
			default: break; // sound returned -1 (invalid label - valid labels = 0 to numSounds)
	    }
    }

    return sound;
}

//==============================================================================
template<typename T>
bool AudioClassifier<T>::checkTrainingSetReady() const
{
    auto readyCount = 0;

    for (auto v : soundsReady)
    {
        if (v == true)
            readyCount++;
    }

    if (readyCount == numSounds)
        return true;
    else
        return false;
}

//==============================================================================
template <typename T>
bool AudioClassifier<T>::checkTrainingSoundReady (const unsigned sound) const
{
	auto ready = soundsReady[sound];
	return ready;
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

	for (auto i = 0; i < trainingLabels.n_elem; ++i)
	{
		//Consider making trainingLabel <int> rather than unsigned to init with -1 label vals
		trainingLabels[i] = 0;
	}

	currentInstanceVector.set_size(numFeatures);
	currentInstanceVector.zeros();

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
template class AudioClassifier<float>;
template class AudioClassifier<double>;
