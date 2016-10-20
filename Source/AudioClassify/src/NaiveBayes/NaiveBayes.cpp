
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
		const auto label = classLabels[j];

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
		const auto label = classLabels[j];

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
		 *  erros when multiplication of small number exceeds representable min. 
		 *  
		 *  log(exp(value)) is equal to value so the original calculation below has been
		 *  altered when calculating normal/Gaussian distribution exponents. 
		 *  
		 *  Previous Calculation:
		 *		
		 *		exponents = arma::exp(-(arma::square(diffs))) / (2 * featureVariances.col(j));
		 *		
		 *	We can use 2 * featureVariances in the calculation below rather than squaring the standard 
		 *	deviation/stdDev values as square(sqrt(value)) == value.
		 */ 
		exponents = arma::log(1 / arma::square(diffs)) - arma::log(2 * featureVariances.col(j));

		//Calculate Normal/Gaussian distribution. 
		distribution = exponents + arma::log(stdDev.col(j));

		//Use sum of log values for test instance probablities rather than raw multiply (less risk of float errors).
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

