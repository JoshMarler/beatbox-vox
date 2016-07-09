/*
  ==============================================================================

    AudioClassifier.h
    Created: 9 Jul 2016 3:49:01pm
    Author:  joshua

  ==============================================================================
*/

#ifndef AUDIOCLASSIFIER_H_INCLUDED
#define AUDIOCLASSIFIER_H_INCLUDED

#include <map>
#include "mlpack/core.hpp"
#include "mlpack/methods/naive_bayes/naive_bayes_classifier.hpp"
#include "../Gist/src/Gist.h"
#include "../OnsetDetection/OnsetDetector.h"

using namespace arma;

template<class T>
class AudioClassifier
{

public:

    AudioClassifier();

    ~AudioClassifier();

    enum class AudioFeature: int 
    {
        spectralCentorid,
        spectralCrest
    };



private:

    std::unique_ptr<Gist<T>> gistOnset;
    std::unique_ptr<Gist<T>> gistFeatures; 
    
    std::unique_ptr<OnsetDetector> osDetector;
    
    std::map<AudioFeature, bool> audioFeatures = {{AudioFeature::spectralCentorid, true}, {AudioFeature::spectralCrest, true}};

    bool spectralCrestIsEnabled;
};




#endif  // AUDIOCLASSIFIER_H_INCLUDED
