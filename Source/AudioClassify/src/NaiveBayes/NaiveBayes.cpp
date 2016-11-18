
#include "NaiveBayes.h"

#include <limits>

//=======================================================================================================
template<typename T>
NaiveBayes<T>::NaiveBayes(const size_t initNumClasses, const size_t initNumFeatures)
	: sqrtTwoPi(std::sqrt(2 * arma::datum::pi)),
	  numFeatures(initNumFeatures),
	  numClasses(initNumClasses)	
{
	//Initialize matrices with size and fill with zeros
	initialise();
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

	//Mean sum. Sum the feature values for each class label instance
	for (size_t j = 0; j < trainingData.n_cols; ++j)
	{
		const auto label = classLabels[j];

		//Increment num occurences of current class in priorProbs set.
		++priorProbs[label];
		featureMeans.col(label) += trainingData.col(j);
	}

	//Normalise mean sums. Divide class feature sums by number of class occurences.
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
		const auto label = classLabels[j];

		/**
		 * Should be real-time safe with arma::square not malloc / dynamic allocating
		 *  due to use of expression templates.
		 */
		featureVariances.col(label) += arma::square(trainingData.col(j) - featureMeans.col(label));
	}

	//Normalise variance sums
	for (size_t i = 0; i < priorProbs.n_elem; ++i)
	{
		if (priorProbs[i] > 1)
			featureVariances.col(i) /= (priorProbs[i] - 1);
	}

	/** Ensure that the featureVariances can be inverted. 
	 *  The distribution calculation takes the standard deviation which 
	 *  is equal to sqrt(featureVariances). The standard deviation is then inverted
	 *  i.e. 1/stdDev or equivalently arma::pow(stdDev, -1). So this will cause a 
	 *  divide by zero type error if any attribute variances are equal to 0.0
	 *  The stdDev will then contain -inf or NaN values which causes calculation errors.
	 *  Replace 0.0 variances with minimal floating point value.
	 */
	
	for (std::size_t i = 0; i < featureVariances.n_elem; ++i)
	{
		if (featureVariances[i] == 0.0)
			featureVariances[i] = std::numeric_limits<T>::min();
	}


	//Normalise prior probabilities
	priorProbs /= static_cast<T>(trainingData.n_cols);
}

//=======================================================================================================

template<typename T>
size_t NaiveBayes<T>::Classify(const arma::Col<T>& instance)
{
	auto classVal = -1;

	//Calculate standard deviation
    stdDev = arma::sqrt(featureVariances);

	//Pre-Cook stdDev * sqrtTwoPi values for distribution calculation - Avoids uneccesary temporary - no malloc.
	stdDev *= sqrtTwoPi;
    stdDev = arma::pow(stdDev, -1);
    

	for (size_t j = 0; j < featureMeans.n_cols; ++j)
	{
		diffs = instance - featureMeans.col(j);
		
		/** Using log probabilities to eliminate/reduce floating point
		 *  erros when multiplication of small numbers/attributes exceeds representable min. 
		 *  
		 *  log(exp(value)) == value so the original calculation below has been
		 *  altered when calculating normal/Gaussian distribution exponents. 
		 *  
		 *  Non log based Calculation:
		 *		
		 *		exponents = arma::exp(-(arma::square(diffs))) / (2 * featureVariances.col(j));
		 *		
		 *	We can use 2 * featureVariances in the calculation below rather than squaring the standard 
		 *	deviation/stdDev values as per notation of distribution in the literatue - square(sqrt(value)) == value.
		 */ 
		exponents = arma::log(1 / arma::square(diffs)) - arma::log(2 * featureVariances.col(j));

		//Calculate Normal/Gaussian distribution. 
		distribution = exponents + arma::log(stdDev.col(j));

		//Use sum of log values for test instance probablities rather than raw multiply (less risk of float errors).
		testProbs(j) = std::log(priorProbs(j)) + arma::accu(distribution);
	}


    //Class val is the label with the max prob value (Classes range 0 - numClasses hence index_max() used);
	classVal = testProbs.index_max();

	return classVal;
}

//=======================================================================================================
template<typename T>
void NaiveBayes<T>::setNumFeatures(unsigned int newNumFeatures)
{
	numFeatures = newNumFeatures;
	initialise();
}

//=======================================================================================================
template<typename T>
void NaiveBayes<T>::initialise()
{
	priorProbs.zeros(numClasses);
	featureMeans.zeros(numFeatures, numClasses);
	featureVariances.zeros(numFeatures, numClasses);
	stdDev.zeros(numFeatures, numClasses);
	exponents.zeros(numFeatures);
	diffs.zeros(numFeatures);
	distribution.zeros(numFeatures);
	testProbs.zeros(numClasses);
}

//=======================================================================================================

//Templating on float and double for different audio precision types. 
template class NaiveBayes<float>;
template class NaiveBayes<double>;

