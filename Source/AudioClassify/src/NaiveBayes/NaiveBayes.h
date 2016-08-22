/*
  ==============================================================================

    AudioClassifyOptions.h
    Created: 15 Jul 2016 10:31:26pm
    Author:  joshua

  ==============================================================================
*/

#ifndef NAIVEBAYES_H_INCLUDED
#define NAIVEBAYES_H_INCLUDED

#include <armadillo>

template<typename T>
class NaiveBayes
{
public:
	NaiveBayes(const size_t numClasses, const size_t numFeatures);
	~NaiveBayes();

	void Train(const arma::Mat<T>& newTrainingData, const arma::Row<size_t>& labels);


	/**
	 * Classifies a single instance and returns the label with the highest probability value.	
	 *
	 * @param instance The instance to be classified (passed as single column/vector)
	*/
	size_t Classify(const arma::Col<T>& instance);

private:

	//Pre calculated constant initialized at construction for Guassian distribution calc.
	const T sqrtTwoPi;
	
	arma::Mat<T> trainingSet;

	//Feature means and variances for Gaussian distribution.
	arma::Mat<T> featureMeans;
	arma::Mat<T> featureVariances;

	//Matrix for standardDeviation values per feature / per class.
	arma::Mat<T> stdDev;
	
	//Vector for exponents per class used in Gaussian distribution calculation.
	arma::Col<T> exponents;

	//Vector for instance - feature means differences per class.
	arma::Col<T> diffs;

	//Vector for Gaussian distribution of features per class.
	arma::Col<T> distribution;

	//Class prior probabilities.	
	arma::Col<T> priorProbs;

	//Vector placeholder for final prob values for each class. 
	arma::Col<T> testProbs;
};

#endif  // NAIVEBAYES_H_INCLUDED
