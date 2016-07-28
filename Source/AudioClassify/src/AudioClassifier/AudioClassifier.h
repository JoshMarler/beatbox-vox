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

    AudioClassifier(int initBufferSize, T initSampleRate, int initNumSounds);

    ~AudioClassifier();

    int getCurrentBufferSize();
    T getCurrentSampleRate();

    void setCurrentBufferSize (int newBufferSize);
    void setCurrentSampleRate (T newSampleRate);
    
    void setTrainingSound(int trainingSound);
    int getCurrentTrainingSound();

    void setTrainingSetSize(int newTrainingSetSize);


    bool getClassifierReady();

    void processAudioBuffer (T* buffer);

    void processCurrentInstance();


    //JWM - This funciton will return 0 for unclassified sounds 
    //NOTE - Need to decide how to handle response of this function when classifier not
    //ready at compile time i.e. assertion, exception or return -1
    unsigned classify();

private:

//==============================================================================

    int bufferSize;
    int trainingSetSize = 10;
    int trainingCount = 0;
    int numSounds; 
    bool hasOnset = false;

    T sampleRate;

    std::atomic_bool usingSpecCentroid {true};
    std::atomic_bool usingSpecCrest {true};
    std::atomic_bool usingSpecFlatness {true};
    std::atomic_bool usingSpecRolloff {true};
    std::atomic_bool usingSpecKurtosis {true};
    std::atomic_bool usingMfcc {true};

    //JWM - This value indicates the current sound being trained declared atomic as may be set by a GUI thread / user control 
    std::atomic_int currentTrainingSound;

    //JWM - Inidicates if the model has been trained with full training set and classifier is ready, declared atomic as may be set by a GUI thread / user control
    std::atomic_bool classifierReady;

    std::atomic_bool training;

    //Vector to hold mag spectrum
    std::vector<T> magSpectrum;

    std::unique_ptr<Gist<T>> gistFeatures; 
    
    std::unique_ptr<OnsetDetector<T>> osDetector;
        
    mat trainingData;

    //JWM - not ideal but mlpack does not provide method for classifying single instance column
    //May have to reconsider use of mlpack and attempt to roll out own classifier algorithms.
    //Alternativley submit pull request to mlpack with a classify(arma::Col data) function.
    mat classifyData;
    Row<size_t> resultsData;

    Row<size_t> trainingLabels;
    colvec currentInstanceVector;
    
    std::unique_ptr<NaiveBayesClassifier<>> nbc;
//==============================================================================

    void setClassifierReady (bool ready);
   
    void configTrainingSetMatrix();

    size_t calcFeatureVecSize();
};




#endif  // AUDIOCLASSIFIER_H_INCLUDED
