/*
  ==============================================================================

    AudioClassifier.cpp
    Created: 9 Jul 2016 3:49:01pm
    Author:  joshua

  ==============================================================================
*/

#include "AudioClassifier.h"

template<class T>
AudioClassifier<T>::AudioClassifier() 
    : gistOnset(std::make_unique<Gist<T>> (512, 44100)),
      gistFeatures(std::make_unique<Gist<T>> (512, 44100))
{
    auto spectralCrest = audioFeatures.find(AudioFeature::spectralCrest);
    spectralCrestIsEnabled = spectralCrest->second;
    float debug = 0.0;
}

template<class T>
AudioClassifier<T>::~AudioClassifier()
{

}

//==============================================================================
template class AudioClassifier<float>;
template class AudioClassifier<double>;
