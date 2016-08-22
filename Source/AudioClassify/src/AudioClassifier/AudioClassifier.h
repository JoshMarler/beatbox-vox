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
#include <armadillo>
#include "../../Gist/src/Gist.h"
#include "../AudioClassifyOptions/AudioClassifyOptions.h"
#include "../OnsetDetection/OnsetDetector.h"
#include "../NaiveBayes/NaiveBayes.h"

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


    //This function will return -1 for unclassified sounds 
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

    //This value indicates the current sound being trained declared atomic as may be set by a GUI thread / user control 
    std::atomic_int currentTrainingSound;

    //Inidicates if the model has been trained with full training set and classifier is ready, declared atomic as may be set by a GUI thread / user control
    std::atomic_bool classifierReady;

    std::atomic_bool training;

    //Map to indicate model status - sound -> ready bool status if training samples collected
    std::map<int, bool> soundsReady;

    //Vector to hold mag spectrum
    std::vector<T> magSpectrum;

    std::unique_ptr<Gist<T>> gistFeatures; 
    
    std::unique_ptr<OnsetDetector<T>> osDetector;
        
    arma::Mat<T> trainingData;

    arma::Row<size_t> trainingLabels;
    
    arma::Col<T> currentInstanceVector;
    
//    std::unique_ptr<NaiveBayes<T>> nbc;
//==============================================================================

    void setClassifierReady (bool ready);
    bool checkTrainingSetReady();
   
    void configTrainingSetMatrix();

    size_t calcFeatureVecSize();
};




#endif  // AUDIOCLASSIFIER_H_INCLUDED
