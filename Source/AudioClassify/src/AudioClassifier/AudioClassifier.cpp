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
AudioClassifier<T>::AudioClassifier(int initBufferSize, T initSampleRate) 
    : gistFeatures(std::make_unique<Gist<T>>(initBufferSize, initSampleRate)),
      osDetector(std::make_unique<OnsetDetector<T>>(initBufferSize)),
      currentTrainingSound(1),
      trainingData(18, trainingSetSize, fill::zeros),
      trainingLabels(trainingSetSize),
      currentInstanceVector(18, fill::zeros),
      nbc(std::make_unique<NaiveBayesClassifier<>>(18, 3))
{
    //auto spectralCrest = audioFeatures.find(AudioClassifyOptions::AudioFeature::spectralCrest);
    //spectralCrestIsEnabled = spectralCrest->second;

    setCurrentSampleRate(initSampleRate);
    setCurrentBufferSize(initBufferSize);
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
int AudioClassifier<T>::getCurrentTrainingSoundLabel()
{
    return currentTrainingSound.load();
}

template<typename T>
std::string AudioClassifier<T>::getCurrentTrainingSoundName()
{
    std::string name = soundLabels.find(currentTrainingSound.load())->second;
    
    return name;
}

//==============================================================================
template<typename T>
void AudioClassifier<T>::setTraining (int newTrainingSound)
{
    currentTrainingSound.store(newTrainingSound);
    training = true;
}

//JWM - NOTE: look into this further as may not be best way to handle labels speed wise.
template<typename T>
void AudioClassifier<T>::setTraining (std::string newTrainingSound)
{
    for (auto it : soundLabels)
    {
        if (newTrainingSound.compare(it.second) == 0)
        {
            currentTrainingSound.store(it.first); 
        }
    }

    training = true;
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

    gistFeatures->processAudioFrame(buffer, bufferSize);
    magSpectrum = gistFeatures->getMagnitudeSpectrum();
    
    bool hasOnset = osDetector->checkForOnset(magSpectrum);

    if (hasOnset)
    {
        processCurrentInstance();

        if (training)
        {
            if (trainingCount < trainingSetSize)
            {
                trainingData.col(trainingCount) = currentInstanceVector;
                trainingLabels[trainingCount] = currentTrainingSound.load();
                trainingCount++;
            }
            else
            {
                mlpack::Timer::Start("timer");
                nbc->Train(trainingData, trainingLabels, false); 
                mlpack::Timer::Stop("timer");
                auto timeVal = mlpack::Timer::Get("timer");
                training = false;
                trainingCount = 0;
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
