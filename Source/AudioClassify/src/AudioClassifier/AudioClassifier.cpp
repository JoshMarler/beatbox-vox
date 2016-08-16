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
    : gistFeatures(std::make_unique<Gist<T>>(initBufferSize, initSampleRate)),
      osDetector(std::make_unique<OnsetDetector<T>>(initBufferSize)),
      currentTrainingSound(0),
      trainingData(18, (trainingSetSize * initNumSounds), arma::fill::zeros),
      trainingLabels(trainingSetSize),
      currentInstanceVector(18, arma::fill::zeros),
      nbc(std::make_unique<NaiveBayes<T>>(initNumSounds, 18))
{
    setCurrentSampleRate(initSampleRate);
    setCurrentBufferSize(initBufferSize);

    classifierReady.store(false);

    numSounds = initNumSounds;
}

template<typename T>
AudioClassifier<T>::~AudioClassifier()
{

}

//==============================================================================
template<typename T>
int AudioClassifier<T>::getCurrentBufferSize()
{
    return bufferSize;
}

template<typename T>
T AudioClassifier<T>::getCurrentSampleRate()
{
    return sampleRate;
}

//==============================================================================
template<typename T>
void AudioClassifier<T>::setCurrentBufferSize (int newBufferSize)
{
    bufferSize = newBufferSize;
    gistFeatures->setAudioFrameSize(newBufferSize);

    magSpectrum.resize(newBufferSize / 2);
    std::fill(magSpectrum.begin(), magSpectrum.end(), 0.0f);
}

template<typename T>
void AudioClassifier<T>::setCurrentSampleRate (T newSampleRate)
{
    sampleRate = newSampleRate;
    gistFeatures->setSamplingFrequency(static_cast<int>(newSampleRate));
}

//==============================================================================
template<typename T>
int AudioClassifier<T>::getCurrentTrainingSound()
{
    return currentTrainingSound.load();
}

//==============================================================================

//JWM - NOTE: revist later - will need assertion if user uses sound value out of range 1 - numSOunds
template<typename T>
void AudioClassifier<T>::setTrainingSound (int trainingSound)
{
    currentTrainingSound.store(trainingSound);
    training.store(true);
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
bool AudioClassifier<T>::getClassifierReady()
{
    return classifierReady.load();
}

//==============================================================================
template<typename T> 
void AudioClassifier<T>::processAudioBuffer (T* buffer)
{
    const int bufferSize = getCurrentBufferSize();

    //Reset hasOnset for next process buffer.
    hasOnset = false;

    gistFeatures->processAudioFrame(buffer, bufferSize);
    magSpectrum = gistFeatures->getMagnitudeSpectrum();
    
    hasOnset = osDetector->checkForOnset(magSpectrum);

    if (hasOnset)
    {
        processCurrentInstance();

        if (currentTrainingSound.load() != -1)
        {
            //JWM - may change this logic later re handling classifier is ready etc.
            classifierReady.store(false);

            if (trainingCount < trainingSetSize)
            {
                trainingData.col(trainingCount) = currentInstanceVector;
                trainingLabels[trainingCount] = currentTrainingSound.load();
                trainingCount++;
            }
            else
            {
                soundsReady.find(currentTrainingSound.load());

                //Reset training status for next sound
                trainingCount = 0;
                currentTrainingSound.store(-1);
                

                nbc->Train(trainingData, trainingLabels); 
            }
        }

    }

}

//==============================================================================
template<typename T>
void AudioClassifier<T>::processCurrentInstance()
{
    size_t pos = 0;

    if (usingSpecCentroid.load())
    {
        currentInstanceVector[pos] = gistFeatures->spectralCentroid(); 
        pos++;
    }

    if (usingSpecCrest.load())
    {
        currentInstanceVector[pos] = gistFeatures->spectralCrest();
        pos++;
    }

    if (usingSpecFlatness.load())
    {
        currentInstanceVector[pos] = gistFeatures->spectralFlatness(); 
        pos++;
    }

    if (usingSpecRolloff.load())
    {
        currentInstanceVector[pos] = gistFeatures->spectralRolloff();
        pos++;
    }
    
    if (usingSpecKurtosis.load())
    {
        currentInstanceVector[pos] = gistFeatures->spectralKurtosis();
        pos++;
    }

    if (usingMfcc.load())
    {
        auto mfccVec = gistFeatures->melFrequencyCepstralCoefficients();

        for (auto val : mfccVec)
        {
          currentInstanceVector[pos] = val; 
          pos++;
        }
    }
}

//==============================================================================
template<typename T>
unsigned AudioClassifier<T>::classify()
{
    unsigned sound = -1;

    auto ready = classifierReady.load();
      
    if (!ready)
        return -1;
   
    if (hasOnset)
    {
        sound = nbc->Classify(currentInstanceVector);
    }

    return sound;
}

//==============================================================================

template<typename T>
bool AudioClassifier<T>::checkTrainingSetReady()
{
    size_t readyCount = 0;
    for(size_t i = 0; i < numSounds; i++)
    {
       auto it = soundsReady.find(i); 
       if (it->second == true)
           readyCount++;
    }


    if(readyCount == numSounds)
        return true;
    else
        return false;
    
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
size_t AudioClassifier<T>::calcFeatureVecSize()
{
    size_t size = 0;

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
        //JWM - eventually change this to use numMfcc's or something
        size += 13;
    }

    return size;
}

//==============================================================================
template class AudioClassifier<float>;
template class AudioClassifier<double>;
