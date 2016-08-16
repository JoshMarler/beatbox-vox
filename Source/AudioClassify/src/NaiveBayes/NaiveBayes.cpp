
#include "NaiveBayes.h"

//=======================================================================================================

template<typename T>
NaiveBayes<T>::NaiveBayes(const size_t numClasses, const size_t numFeatures) 
	: sqrtTwoPi(std::sqrt(2 * arma::datum::pi))
{
	//Initialize matrices with size and fill with zeros
	priorProbs.zeros(numClasses);
	featureMeans.zeros(numFeatures, numClasses);
	featureVariances.zeros(numFeatures, numClasses);
	stdDev.zeros(numFeatures, numClasses);
	exponents.zeros(numFeatures);
	diffs.zeros(numFeatures);
	distribution.zeros(numFeatures);
	testProbs.zeros(numClasses);
}

template<typename T>
NaiveBayes<T>::~NaiveBayes()
{
}

//=======================================================================================================

template<typename T>
void NaiveBayes<T>::Train(const arma::Mat<T>& trainingData, const arma::Row<size_t>& classLabels)
{

	//Reset prob states
	priorProbs.zeros();
	featureMeans.zeros();
	featureVariances.zeros();

	//Mean sum - sum feature values for each class label instance
	for (size_t j = 0; j < trainingData.n_cols; ++j)
	{
		const size_t label = classLabels[j];

		//increment num occurences of current class in priorProbs set.
		++priorProbs[label];
		featureMeans.col(label) += trainingData.col(j);
	}

	//Normalise mean sums - divide class feature sums by number of class occurences.
	for (size_t i = 0; i < priorProbs.n_elem; ++i)
	{
		//Check if probability of each class != 0 due to no instance of class in training set.
		//Avoids divide by zero errors.
		if (priorProbs[i] != 0.0)
			featureMeans.col(i) /= priorProbs[i]; //divide by total num class occurences.
	}

	//Variances sum
	for (size_t j = 0; j < trainingData.n_cols; ++j)
	{
		const size_t label = classLabels[j];

		//JWM - NOTE: Should be real-time safe with arma::square not malloc / dynamic allocating
		//due to use of expression templates.
		featureVariances.col(label) += arma::square(trainingData.col(j) - featureMeans.col(label));
	}

	//Normalise variance sums
	for (size_t i = 0; i < priorProbs.n_elem; ++i)
	{
		if (priorProbs[i] > 1)
			featureVariances.col(i) /= (priorProbs[i] - 1);
	}


	//Normalise prior probabilities
	priorProbs /= trainingData.n_cols;
}

//=======================================================================================================

template<typename T>
size_t NaiveBayes<T>::Classify(const arma::Col<T> instance)
{
	size_t classVal = -1;

	//Calculate standard deviation
    stdDev = arma::sqrt(featureVariances);

	//Pre-Cook stdDev * sqrtTwoPi values for use in distribution calculation - Avoids uneccesary temporary.
	stdDev *= sqrtTwoPi;
    stdDev = arma::pow(stdDev, -1);
    

	for (size_t j = 0; j < featureMeans.n_cols; ++j)
	{
        /*
         * JWM
         * instance and featureMeans.col(j) vectors different size so probably need to use repmat() and possibly specify the training set size in the
         * class constructor for prototype. Then in later version potentially call train on an offline thread to allow for incremental training. 
         * Need to check repmat() doesn't create a temporary or malloc when assigment operator is applied to pre-allocated matrix/vector. 
         * */
		diffs = instance - featureMeans.col(j);

        //JWM - NOTE: Room exists to simplfy / improve this
        //Can work out a way to remove exp() function as the log operations make it redundant - check mlpack.
		exponents = arma::exp(-(arma::square(diffs))) / (2 * featureVariances.col(j));
        
		//Calculate Normal/Gaussian distribution. 
		distribution = exponents % stdDev.col(j);
		
        //Convert distribution to log values.
        distribution = arma::log(distribution);
		testProbs(j) = std::log(priorProbs(j)) + arma::accu(distribution);
	}


    //Class val is the label with the max prob value - classes 0 - numClasses hence index used. 
	classVal = testProbs.index_max();

	return classVal;
}

//=======================================================================================================

//Templating on float and double for different audio precision types. 
template class NaiveBayes<float>;
template class NaiveBayes<double>;

