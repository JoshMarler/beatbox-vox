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
#include <atomic>
#include "mlpack/core.hpp"
#include "mlpack/methods/naive_bayes/naive_bayes_classifier.hpp"
#include "../../Gist/src/Gist.h" 
#include "../AudioClassifyOptions/AudioClassifyOptions.h"
#include "../OnsetDetection/OnsetDetector.h"

//JWM - need to deal with namespace conflict against juce::Timer
//using namespace mlpack;
using namespace mlpack::naive_bayes;
using namespace arma;

template<typename T>
class AudioClassifier
{

public:

    AudioClassifier(int initBufferSize, T initSampleRate);

    ~AudioClassifier();

    int getCurrentBufferSize();
    T getCurrentSampleRate();

    void setCurrentBufferSize (int newBufferSize);
    void setCurrentSampleRate (T newSampleRate);
    
    //JWM - NOTE: may change this to getCurrentTrainingSoundVal rather than label - returns the numeric label value used internally by the classifier.
    int getCurrentTrainingSoundLabel();
    std::string getCurrentTrainingSoundName();

    void setTraining (int trainingSound);
    void setTraining (std::string trainingSound);

    void setTrainingSetSize(int newTrainingSetSize);

    bool getClassifierReady();

    void processAudioBuffer (T* buffer);

    void processCurrentInstance();

     //nbc(std::make_unique<NaiveBayesClassifier<>>()), 


private:

//==============================================================================

    int bufferSize;
    int trainingSetSize = 10;
    int trainingCount = 0;
    T sampleRate;

    bool training = true;
    
    std::atomic_bool usingSpecCentroid {true};
    std::atomic_bool usingSpecCrest {true};
    std::atomic_bool usingSpecFlatness {true};
    std::atomic_bool usingSpecRolloff {true};
    std::atomic_bool usingSpecKurtosis {true};
    std::atomic_bool usingMfcc {true};

    //JWM - This value indicates the current sound being trained declared atomic as may be set by a GUI thread / user control 
    std::atomic_int currentTrainingSound;

    //JWM - Inidicates if the model has been trained with full training set and classifier is ready, declared atomic as may be set by a GUI thread / user control
    std::atomic_bool classifierReady {false};

    //Vector to hold mag spectrum
    std::vector<T> magSpectrum;

    std::unique_ptr<Gist<T>> gistFeatures; 
    
    std::unique_ptr<OnsetDetector<T>> osDetector;
        
    std::map<int, std::string> soundLabels;
    
    mat trainingData;
    Row<size_t> trainingLabels;
    colvec currentInstanceVector;
    
    std::unique_ptr<NaiveBayesClassifier<>> nbc;
//==============================================================================

    void setClassifierReady (bool ready);
   
    void configTrainingSetMatrix();
    size_t calcFeatureVecSize();
    //JWM - creates an instance column/vector to be classifier or added to training set.
};




#endif  // AUDIOCLASSIFIER_H_INCLUDED
