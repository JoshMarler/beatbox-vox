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
    : gistFeatures(std::make_unique<Gist<T>> (initBufferSize, initSampleRate)),
      currentTrainingSound(-1)
{
    auto spectralCrest = audioFeatures.find(AudioClassifyOptions::AudioFeature::spectralCrest);
    spectralCrestIsEnabled = spectralCrest->second;
    float debug = 0.0;

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
void AudioClassifier<T>::setCurrentTrainingSound (int newTrainingSound)
{
    currentTrainingSound.store(newTrainingSound);
}

//JWM - NOTE: look into this further as may not be best way to handle labels speed wise.
template<typename T>
void AudioClassifier<T>::setCurrentTrainingSound (std::string newTrainingSound)
{
    for (auto it : soundLabels)
    {
        if (newTrainingSound.compare(it.second) == 0)
        {
            currentTrainingSound.store(it.first); 
        }
    }
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
        //classify
    }
}

//==============================================================================

template<typename T>
void AudioClassifier<T>::configTrainingSetMatrix()
{
    //JWM - iterate through feature map to calculate size of matrix needed + trainingSetSize .
}


//==============================================================================
template class AudioClassifier<float>;
template class AudioClassifier<double>;
