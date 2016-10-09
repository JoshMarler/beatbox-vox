/*
   ==============================================================================

    AudioClassifier.cpp
    Created: 9 Jul 2016 3:49:01pm
    Author:  joshua

    ==============================================================================
*/

#include "AudioClassifier.h"

//==============================================================================
template<typename T>
AudioClassifier<T>::AudioClassifier(int initBufferSize, T initSampleRate, int initNumSounds) 
    : trainingData(21, (trainingSetSize * initNumSounds), arma::fill::zeros),
      trainingLabels((trainingSetSize * initNumSounds)),
      currentInstanceVector(21, arma::fill::zeros),
      gistFeatures(initBufferSize, initSampleRate),
      osDetector(initBufferSize),
      nbc(initNumSounds, 21)
{
    setCurrentSampleRate(initSampleRate);
    setCurrentBufferSize(initBufferSize);

    training.store(false);
    classifierReady.store(false);
    currentTrainingSound.store(-1);

    numSounds = initNumSounds;

    auto numCoefficients = gistFeatures.getMFCCNumCoefficients();
    mfccs.reset(new T[numCoefficients]);

    //Set initial sound ready states to false in training set.  
    soundsReady.resize(numSounds, false);
}

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
    gistFeatures.setAudioFrameSize(newBufferSize);

    magSpectrum.reset(new T[newBufferSize / 2]);    

    std::fill(magSpectrum.get(), (magSpectrum.get() + (newBufferSize / 2)), 0.0f);

    osDetector.setCurrentBufferSize(newBufferSize);
    /**
     * Note: After prototype stage this function should probably handle clearing the model
     * and setting classifier ready to false as well as emptying the trainingDataSet and trainingLables matrices.
     * The NaiveBayes class may require a clear method which clears out the various probability and feature mean vectors etc.
     */
}

template<typename T>
void AudioClassifier<T>::setCurrentSampleRate (T newSampleRate)
{
    sampleRate = newSampleRate;
    gistFeatures.setSamplingFrequency(static_cast<int>(newSampleRate));
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
void AudioClassifier<T>::setOnsetDetectorMeanCoeff(T newMeanCoeff)
{
    osDetector.setMeanCoefficient(newMeanCoeff);
}

//==============================================================================
template<typename T>
void AudioClassifier<T>::setOnsetDetectorMedianCoeff(T newMedianCoeff)
{
    osDetector.setMedianCoefficient(newMedianCoeff);
}
//==============================================================================
template<typename T>
void AudioClassifier<T>::setOnsetDetectorNoiseRatio(T newNoiseRatio)
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
void AudioClassifier<T>::setOnsetDetectorODFType(AudioClassifyOptions::ODFType newODFType)
{
    osDetector.setCurrentODFType(newODFType);
}

//==============================================================================
//JWM - NOTE: revist later - will need assertion if user uses sound value out of range 0 - numSOunds
template<typename T>
void AudioClassifier<T>::recordTrainingSample(int sound)
{
    currentTrainingSound.store(sound);

    trainingCount = (sound * trainingSetSize);

    training.store(true);
}

//==============================================================================
template<typename T>
void AudioClassifier<T>::trainModel()
{
    //If all sound samples collected for training set train model.
    if (checkTrainingSetReady())
    {
        nbc.Train(trainingData, trainingLabels); 
        classifierReady.store(true);    
    }

    //JWM - NOTE: Potentially return boolean and return false if checkTrainingSetReady() returns false.
}

//==============================================================================
template<typename T>
void AudioClassifier<T>::setTrainingSetSize(int newTrainingSetSize)
{
    trainingSetSize = newTrainingSetSize;

    //JWM - resize the training set matrix and handle retraiing/keep data.
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
    //Reset hasOnset for next process buffer.
    hasOnset = false;

    const auto bufferSize = getCurrentBufferSize();

    /** if (bufferSize != numSamples) */
    /** { */
    /**     //setCurrentBufferSize() needs to be called before continuing processing - training set/model will be invalid strictly speaking. */
    /**     return; */
    /** } */

    gistFeatures.processAudioFrame(buffer, bufferSize);
    gistFeatures.getMagnitudeSpectrum(magSpectrum.get());
    
    hasOnset = osDetector.checkForOnset(magSpectrum.get(), bufferSize / 2);

    if (hasOnset)
    {
        processCurrentInstance();

        if (currentTrainingSound.load() != -1 && training.load())
        {
            classifierReady.store(false);

	        auto sound = currentTrainingSound.load();

            if (trainingCount < (trainingSetSize * (sound + 1)))
            {
                trainingData.col(trainingCount) = currentInstanceVector;
                trainingLabels[trainingCount] = static_cast<size_t>(sound);

                trainingCount++;
            }
            else
            {
                //Set sound ready state to true for current training sound.
                soundsReady[sound] = true;
                training.store(false);
                currentTrainingSound.store(-1);
            }
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
}
//==============================================================================
template<typename T>
bool AudioClassifier<T>::noteOnsetDetected() const
{
    return hasOnset;
}

//==============================================================================
template<typename T>
int AudioClassifier<T>::classify()
{
	auto sound = -1;

    auto ready = classifierReady.load();
      
    if (!ready)
        return -1;
   
    if (hasOnset)
        sound = nbc.Classify(currentInstanceVector);

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
    //JWM - iterate through feature map to calculate size of matrix needed + trainingSetSize .
}


//==============================================================================

//==============================================================================
template<typename T>
unsigned int AudioClassifier<T>::calcFeatureVecSize() const
{
    auto size = 0;

	if (usingRMS.load())
		size++;

	if (usingPeakEnergy.load())
		size++;

	if (usingZeroCrossingRate.load())
		size++;

    if (usingSpecCentroid.load())
        size++; 
    
    if (usingSpecCrest.load())
        size++;

    if (usingSpecFlatness.load())
        size++;

    if (usingSpecRolloff.load())
        size++;

    if (usingSpecKurtosis.load())
        size++;

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
