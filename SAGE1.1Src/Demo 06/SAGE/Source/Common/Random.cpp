/// \file Random.cpp
/// \brief Code for the random number generator

//Copyright Ian Parberry, 1998
//Last updated July 3, 1998

#include <windows.h>

#include "common/random.h"

CRandom Random; //random number generator

CRandom::CRandom(){ //constructor
  seed(); //seed random number generator
}

/// \param seed Specifies the seed value
void CRandom::seed(unsigned int seed){ //seed random number generator
  srand(seed); m_nCount=0;
}

/// \return A random 32-bit signed integer.
int CRandom::getInt(){
  unsigned int sample = (rand() << 17) | (rand() << 2) | (rand() & 3); // random 32-bit unsigned value
  return (int &)sample; // Reinterpret it as a signed integer
}

/// \param minVal Specifies the minimum value.
/// \param maxVal Specifies one more than the maximum value.
/// \return A random number in the interval [<tt>minVal</tt>, <tt>maxVal - 1</tt>].
int CRandom::getInt(int minVal, int maxVal){  
  //return random number in  i..j
  if(minVal >= maxVal)
    return minVal;
  int sample = getInt() & 0x7FFFFFFF;
  return sample%(maxVal - minVal + 1) + minVal;
}

/// \return A random float in the interval [0.0,1.0]
float CRandom::getFloat(){  
  // ASSUME:  float and int are same size (32-bit)
  static const double factor = 1.0 / 2147483647.0; // Maps from [0,2^31-1] to [0,1]
  int sample = getInt() & 0x7FFFFFFF; // Strip sign bit; random nonnegative 31-bit value
  return (float)((double)sample * factor);
}

/// \param minVal Specifies the minimum value.
/// \param maxVal Specifies one more than the maximum value.
/// \return A random number in the interval [<tt>minVal</tt>, <tt>maxVal</tt>).
/// \note The interval is open on the right side--that is, this function will not
///     return \p maxVal.
float CRandom::getFloat(float minVal, float maxVal)
{
  if(minVal >= maxVal)
    return minVal;
  return minVal + getFloat() * (maxVal - minVal);
}

/// \return true or false, with roughly equal probability.
bool CRandom::getBool()
{
  return ((rand() & 1) == 0);
}