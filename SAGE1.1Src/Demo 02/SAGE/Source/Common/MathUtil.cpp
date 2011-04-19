/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// MathUtil.cpp - Miscellaneous math utilities
//
// Visit gamemath.com for the latest version of this file.
//
/////////////////////////////////////////////////////////////////////////////

#include <math.h>

#include "MathUtil.h"
#include "Vector3.h"

const Vector3 Vector3::kZeroVector(0.0f, 0.0f, 0.0f);
const Vector3 Vector3::kRightVector(1.0f, 0.0f, 0.0f);
const Vector3 Vector3::kUpVector(0.0f, 1.0f, 0.0f);
const Vector3 Vector3::kForwardVector(0.0f, 0.0f, 1.0f);

//---------------------------------------------------------------------------
// "Wrap" an angle in range -pi...pi by adding the correct multiple
// of 2 pi

/// \param theta Specifies the angle to be wrapped.
/// \return The angle wrapped into [-pi, pi].
float wrapPi(float theta) {
	theta += kPi;
	theta -= floor(theta * k1Over2Pi) * k2Pi;
	theta -= kPi;
	return theta;
}

//---------------------------------------------------------------------------
// safeAcos
//
// Same as acos(x), but if x is out of range, it is "clamped" to the nearest
// valid value.  The value returned is in range 0...pi, the same as the
// standard C acos() function

/// \param x Specifies the value whose arccosine is to be computed.
/// \return The arccosine of \p x (where \p x is clamped into [-1,1]).
float safeAcos(float x) {

	// Check limit conditions

	if (x <= -1.0f) {
		return kPi;
	}
	if (x >= 1.0f) {
		return 0.0f;
	}

	// Value is in the domain - use standard C function

	return acos(x);
}
