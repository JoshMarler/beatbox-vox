/*
  ==============================================================================

    MathHelpers.h
    Created: 28 Jun 2016 7:30:18pm
    Author:  joshua

  ==============================================================================
*/

#ifndef MATHHELPERS_H_INCLUDED
#define MATHHELPERS_H_INCLUDED

#include "JuceHeader.h"
#include <vector>
namespace MathHelpers
{
    template<typename FloatType>
    FloatType getMean(std::vector<FloatType> vec)
    {
        auto sum = std::accumulate(vec.cbegin(), vec.cend(), 0.0);
        return sum / vec.size();       
    }
}


#endif  // MATHHELPERS_H_INCLUDED
