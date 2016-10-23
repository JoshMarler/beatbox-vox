/*
  ==============================================================================

    PreProcessing.h
    Created: 23 Oct 2016 4:08:19pm
    Author:  Joshua Marler

  ==============================================================================
*/

#ifndef PREPROCESSING_H_INCLUDED
#define PREPROCESSING_H_INCLUDED

#ifdef _WIN64
#define ARMA_64BIT_WORD
#endif


#include <armadillo.h>

namespace PreProcessing
{
	/** Normalises each element to 0 - 1 range. 
	 * @param mat The input matrix to normalise.
	 */	
	template<typename T>
	void normalise(arma::Mat<T>& mat)
	{
		mat.transform([](T val) { val = std::abs(val); return val / (1 + val); });
	}

	template<typename T>
	void standardize()
	{
		
	}
}



#endif  // PREPROCESSING_H_INCLUDED
