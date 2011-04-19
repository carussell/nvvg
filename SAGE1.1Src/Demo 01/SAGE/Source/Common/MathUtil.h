/// \file MathUtil.h
/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// MathUtil.h - Declarations for miscellaneous math utilities
//
// Visit gamemath.com for the latest version of this file.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __MATHUTIL_H_INCLUDED__
#define __MATHUTIL_H_INCLUDED__

#include <math.h>

// Declare a global constant for pi and a few multiples.

const float kPi = 3.14159265f;   ///< Represents pi.
const float k2Pi = kPi * 2.0f;   ///< Represents 2 * pi.
const float kPiOver2 = kPi / 2.0f;   ///< Represents pi / 2.
const float k1OverPi = 1.0f / kPi;   ///< Represents 1 / pi.
const float k1Over2Pi = 1.0f / k2Pi;   ///< Represents 1 / (2 * pi).
const float kPiOver180 = kPi / 180.0f;   ///< Represents pi / 180.
const float k180OverPi = 180.0f / kPi;   ///< Represents 180 / pi.

extern float wrapPi(float theta);   ///< Wraps an angle into the interval [-pi,pi].

// "Safe" inverse trig functions
extern float safeAcos(float x); ///< Computes an inverse cosine, clamping invalid values to the interval [-1,1].

// Convert between degrees and radians

/// \brief Converts an angle from degrees to radians.
/// \param deg Specifies the angle in degrees to be converted.
/// \return The angle in radians.
inline float	degToRad(float deg) { return deg * kPiOver180; }

/// \brief Converts an angle from radians to degrees.
/// \param rad Specifies the angle in radians to be converted.
/// \return The angle in degrees.
inline float	radToDeg(float rad) { return rad * k180OverPi; }

// Compute the sin and cosine of an angle.  On some platforms, if we know
// that we need both values, it can be computed faster than computing
// the two values seperately.

/// \brief Computs both the sine and cosine of an angle.
/// \param returnSin Points to a float to be filled with the sine of theta.
/// \param returnCos Points to a float to be filled with the cosine of theta.
/// \param theta Specifies the angle whose sine and cosine are to be computed.
/// \remark On some platforms, this can be faster than computing the two
///     values separately.
inline void sinCos(float *returnSin, float *returnCos, float theta) {

	// For simplicity, we'll just use the normal trig functions.
	// Note that on some platforms we may be able to do better

	*returnSin = sin(theta);
	*returnCos = cos(theta);
}

// Convert between "field of view" and "zoom"  See section 15.2.4.
// The FOV angle is specified in radians.

/// \brief Converts a field-of-view angle to a zoom value.
/// \param fov Specifies the field-of-view angle.
/// \return The corresponding zoom value.
inline float	fovToZoom(float fov) { return 1.0f / tan(fov * .5f); }

/// \brief Converts a zoom value to a field-of-view angle.
/// \param zoom Specifies the zoom value.
/// \return The corresponding field-of-view angle.
inline float	zoomToFov(float zoom) { return 2.0f * atan(1.0f / zoom); }

/// \brief Computes the true value of a mod b (with no mirroring for negative numbers).
/// \param x Specifies the dividend.
/// \param y Specifies the divisor.
/// \return The remainder of a divided by b
inline float modulo(float x, float y)
{
  float rv = fmod(x,y);
  return (rv < 0.0f) ? (y + rv) : rv;  
}

/// \brief Clamps a value to a given lower bound.
/// \param f Specifies the value to be clamped.
/// \param lowerBound Specifies the minimum clamped value.
inline void clampUp(float &f, float lowerBound)
{
  if(f < lowerBound)
    f = lowerBound;
}

/// \brief Clamps a value to a given upper bound.
/// \param f Specifies the value to be clamped.
/// \param upperBound Specifies the maximum clamped value.
inline void clampDown(float &f, float upperBound)
{
  if(f > upperBound)
    f = upperBound;
}

/// \brief Clamps a value to a given range.
/// \param f Specifies the value to be clamped.
/// \param lowerBound Specifies the minimum clamped value.
/// \param upperBound Specifies the maximum clamped value.
/// \note This function assumes that <tt>lowerBound < upperBound</tt>.
///       If not, behavior is undefined.
inline void clamp(float &f, float lowerBound, float upperBound)
{
  if(f > upperBound)
    f = upperBound;
  else if(f < lowerBound)
    f = lowerBound;
}

/////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __MATHUTIL_H_INCLUDED__
