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
	/** Normalises each row (attributes) between 0 and 1. 
	 * @param mat The input matrix to normalise.
	 */	
	template<typename T>
	void normalise(arma::Mat<T>& mat)
	{
		for (auto i = 0; i < mat.n_rows; ++i)
		{
			T min = mat.row(i).min();
			T max = mat.row(i).max();

			mat.row(i).transform([min, max](T val) { return (val - min) / (max - min); });
		}
	}

	template<typename T>
	void normalise(arma::Col<T>& col)
	{
		T min = col.min();
		T max = col.max();

		col.transform([min, max](T val) { return (val - min) / (max - min); });

	}

	template<typename T>
	void standardize()
	{
		
	}
}



#endif  // PREPROCESSING_H_INCLUDED
