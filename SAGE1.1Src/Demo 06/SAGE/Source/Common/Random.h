/// \file Random.h
/// \brief Interface for random number generator

//Copyright Ian Parberry, 1999
//Last updated September 29, 1999

#ifndef __RANDOM__
#define __RANDOM__

/// \brief A random number generator.
/// \note This class assumes that integers and floats are both 32-bit.
class CRandom{
  private:
    int m_nCount; ///< Tracks the number of times the generator is used.
  public:
    CRandom(); ///< Constructor.
    int getInt(); ///< Returns a random 32-bit integer.
    int getInt(int minVal,int maxVal); ///< Returns a random number in [i,j).
    void seed(unsigned int seed = 99); ///< Seed the random number generator.
    float getFloat(); ///< Returns a random float in [0,1].
    float getFloat(float minVal, float maxVal); ///< Returns a random float in [i,j].
    bool getBool(); ///< Returns a random Boolean value.
};

#endif
