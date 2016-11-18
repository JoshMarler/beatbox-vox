/*
  ==============================================================================

    AudioClassifier.h
    Created: 9 Jul 2016 3:49:01pm
    Author:  joshua

  ==============================================================================
*/

#ifndef AUDIOCLASSIFIER_H_INCLUDED
#define AUDIOCLASSIFIER_H_INCLUDED

//For windows compatibility with armadillo 64bit
#ifdef _WIN64
#define ARMA_64BIT_WORD
#endif

#include <string>
#include <memory>
#include <atomic>

#include <armadillo.h>

#include "../../Gist/src/Gist.h"
#include "../AudioClassifyOptions/AudioClassifyOptions.h"
#include "../OnsetDetection/OnsetDetector.h"
#include "../NaiveBayes/NaiveBayes.h"
#include "../NearestNeighbour/NearestNeighbour.h"

//==============================================================================
template<typename T>
class AudioClassifier
{

public:

	//==============================================================================
    AudioClassifier(int initBufferSize, T initSampleRate, int initNumSounds, int initNumInstances);
    ~AudioClassifier();

	//==============================================================================
    int getCurrentBufferSize() const;
    T getCurrentSampleRate() const;

    void setCurrentBufferSize (int newBufferSize);
    void setCurrentSampleRate (T newSampleRate);
    
	//==============================================================================
    int getCurrentSoundRecording() const;
    

	//==============================================================================
    //Onset detector functions
    void setOSDMeanCoeff(T newMeanCoeff);
    void setOSDMedianCoeff(T newMedianCoeff);
    void setOSDNoiseRatio(T newNoiseRatio);
    void setOSDMsBetweenOnsets(const int ms);
    void setOSDDetectorFunctionType(AudioClassifyOptions::ODFType newODFType);
	bool getOSDUsingAdaptiveWhitening();
	void setOSDUseAdaptiveWhitening(bool use);
	void setOSDWhitenerPeakDecayRate(unsigned int newDecayRate);
	bool getOSDUsingLocalMaximum();
	void setOSDUseLocalMaximum(bool use);

	//==============================================================================
	void setNumBuffersDelayed(unsigned int newNumDelayed);
	int getNumBuffersDelayed() const;

	/** This method sets the classifier type/learning algorithm to be used.
	 * @param classifierType the classifier type to be used i.e. AudioClassifyOptions::ClassifierType::knn
	 */
	void setClassifierType(AudioClassifyOptions::ClassifierType classifierType);

	AudioClassifyOptions::ClassifierType getClassifierType() const;

	/**
	 *
	 */
    void recordTrainingData(int trainingSound);
	
	void recordTestData(int testSound);

    void train();
	
	float test(std::vector<std::pair<unsigned int, unsigned int>>& outputResults);

	/** Saves the current training data set being used by the model/classifier. 
	 * This can be loaded again on the next application load so that training sets do not have
	 * to be collected again. 
     * Note: This method should NOT be called from the audio/callback thread as it involves file IO and will block.
	 * @param fileName the fully qualified file name/path to save the training set matrix to. i.e. C:\\Models\\model.csv 
     * @param errorString output parameter which will contain an error message if applicable or default "" blank string.
     * @return true for successful save otherwise false.
	 */
	bool saveTrainingSet(const std::string& fileName, std::string& errorString);

	/** Loads the specified training data set according to the file name / path 
	 * passed in and trains the internal classifier on with the loaded data. 
     * Note: This method should NOT be called from the audio/callback thread as it involves file IO and will block.
	 * @param fileName the fully qualified file name/path for the training data set to load. i.e. C:\\Models\\model.csv
     * @param errorString output parameter which will contain an error message if applicable or default "" blank string.
     * @return true for successful load otherwise false.
	 */
	bool loadTrainingSet(const std::string& fileName, std::string& errorString);

	bool saveTestSet(const std::string& fileName, std::string& errorString);
	bool loadTestSet(const std::string& fileName, std::string& errorString);

	/** @return the number of sounds currently being used in the model. */
	size_t getNumSounds() const;
    
	/** Sets the number of instances to be used per sound for the training set. 
	 * The model/classifier will need to be re-trained and a fresh training set collected.
	 * @param newNumInstances the number of instance to be recorded per sound for the training set.
	 */
    void setNumTrainingInstances(int newNumInstances);
	int getNumTrainingInstances() const;

	void setNumTestInstances(int newNumInstances);
	int getNumTestInstances() const;

    bool checkTrainingSetReady() const;
	bool checkTrainingSoundReady(const unsigned sound) const;

    bool checkTestSetReady() const;
	bool checkTestSoundReady(const unsigned sound) const;

    bool getClassifierReady() const;
    
    bool isRecording() const;

    void processAudioBuffer (const T* buffer, const int numSamples);


    /** Checks whether the current frame has a detected note onset. This can be called to help 
     * with configuring the AudioClassifier oject's OnsetDetector. This function should be called
     * right after a call to processAudioBuffer() in the same frame i.e. before the next processAudioBuffer() 
     * call.
     * @returns true if a note onset has been detected for the frame/previous frame depending on whether
     * the onset detector is using local maximums.
     */
    bool noteOnsetDetected() const;

    //This function will return -1 for unclassified sounds 
    int classify();

private:

	//==============================================================================
    int bufferSize = 0;
	int delayedBufferSize = 0;

	//==============================================================================
	int numDelayedBuffers = 0;
	unsigned int delayedProcessedCount = 0;

	//==============================================================================
    int trainingSetSize = 0;
	int testSetSize = 0;
	int numTrainingInstances = 0;
	int numTestInstances = 0;
    int trainingCount = 0;
	int testCount = 0;

	//==============================================================================
    int numSounds = 0; 
	unsigned int numFeatures = 0;
	
	//==============================================================================
	bool hasOnset = false;

	//==============================================================================
	T sampleRate = static_cast<T>(0.0);

	//==============================================================================
    //Values to indicate which features currently being used by model.
	std::atomic_bool usingRMS {true};
	std::atomic_bool usingPeakEnergy {true};
	std::atomic_bool usingZeroCrossingRate {true};
    std::atomic_bool usingSpecCentroid {true};
    std::atomic_bool usingSpecCrest {true};
    std::atomic_bool usingSpecFlatness {true};
    std::atomic_bool usingSpecRolloff {true};
    std::atomic_bool usingSpecKurtosis {true};
    std::atomic_bool usingMfcc {true};

	//==============================================================================
	std::atomic_bool useDelayedEval {false};

	//==============================================================================
	std::atomic<AudioClassifyOptions::ClassifierType> currentClassfierType;

	//==============================================================================
	/* Classifier current state variables */

    //Indicates the current sound being recorded/trained.
    std::atomic_int currentTrainingSoundRecording;
	std::atomic_int currentTestSoundRecording;

	//Classifier is ready to use true/false.
    std::atomic_bool classifierReady;

	//Indicates whether currently recording test/training data.
    std::atomic_bool recordingTrainingData;
	std::atomic_bool recordingTestData;

    //Holds states for each sound in model to confirm whether sound's training set has been recorded.
    std::vector<bool> trainingSoundsReady;
	std::vector<bool> testSoundsReady;
    
	//==============================================================================
    //Array/Buffer to hold mag spectrum.
    std::unique_ptr<T[]> magSpectrumOSD;
	
	//Array/Buffer to hold the current audio buffer - used for delayed evaluation so size will be (bufferSize * (numDelayed + 1))
	std::unique_ptr<T[]> audioBuffer;

    //Array/Buffer to hold the mel frequency cepstral coefficients.
    std::unique_ptr<T[]> mfccs;

	//==============================================================================
    //Holds the training data set.
    arma::Mat<T> trainingData;

    //Holds the trainingData matrix's corresponding label values for labelled training data.
    arma::Row<size_t> trainingLabels;

	arma::Mat<T> testData;
	arma::Row<size_t> testLabels;
    
    //Holds the the feature values/vector for the current instance/block.
    arma::Col<T> currentInstanceVector;
   
	//==============================================================================
    Gist<T> gistFeatures;
	Gist<T> gistFeaturesOSD;
    OnsetDetector<T> osDetector;
    NaiveBayes<T> nbc;
	NearestNeighbour<T> knn;

	//==============================================================================
    void processCurrentInstance();

	void addToTrainingSet(const arma::Col<T>& newInstance);
	void addToTestSet(const arma::Col<T>& newInstance);

	void resetClassifierState();
	void resetTestState();
	void configTrainingSetMatrix();
	void configTestSetMatrix();

	//Calculates the size required for the feature rows based on features being used
    unsigned int calcFeatureVecSize() const;

	//==============================================================================
};




#endif  // AUDIOCLASSIFIER_H_INCLUDED
