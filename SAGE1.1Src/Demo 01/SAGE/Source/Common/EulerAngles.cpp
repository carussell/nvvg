/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// EulerAngles.cpp - Implementation of class EulerAngles
//
// Visit gamemath.com for the latest version of this file.
//
/////////////////////////////////////////////////////////////////////////////

#include <math.h>

#include "EulerAngles.h"
#include "Quaternion.h"
#include "MathUtil.h"
#include "Matrix4x3.h"
#include "RotationMatrix.h"

/////////////////////////////////////////////////////////////////////////////
//
// Notes:
//
// See Chapter 11 for more information on class design decisions.
//
// See section 10.3 for more information on the Euler angle conventions
// assumed.
//
/////////////////////////////////////////////////////////////////////////////

const EulerAngles EulerAngles::kEulerAnglesIdentity(0.0f, 0.0f, 0.0f);

/////////////////////////////////////////////////////////////////////////////
//
// class EulerAngles Implementation
//
/////////////////////////////////////////////////////////////////////////////

/// Determines the canonical Euler angle triple for this
/// set of angles and canonizes it.  Note that while this
/// doesn't affect the rotation represented by the triplet,
/// if your code uses the angles for some other purpose
///  (e.g., angular velocity), the results may be unexpected.
/// \note See section 10.3 for more information.
void	EulerAngles::canonize() {

	// First, wrap pitch in range -pi ... pi

	pitch = wrapPi(pitch);

	// Now, check for "the back side" of the matrix, pitch outside
	// the canonical range of -pi/2 ... pi/2

	if (pitch < -kPiOver2) {
		pitch = -kPi - pitch;
		heading += kPi;
		bank += kPi;
	} else if (pitch > kPiOver2) {
		pitch = kPi - pitch;
		heading += kPi;
		bank += kPi;
	}

	// OK, now check for the gimbal lock case (within a slight
	// tolerance)

	if (fabs(pitch) > kPiOver2 - 1e-4) {

		// We are in gimbal lock.  Assign all rotation
		// about the vertical axis to heading

		heading += bank;
		bank = 0.0f;

	} else {

		// Not in gimbal lock.  Wrap the bank angle in
		// canonical range 

		bank = wrapPi(bank);
	}

	// Wrap heading in canonical range

	heading = wrapPi(heading);
}

/// \param q Specifies the quaternion to be converted.
/// \note See 10.6.6.
void	EulerAngles::fromObjectToInertialQuaternion(const Quaternion &q) {

	// Extract sin(pitch)

	float sp = -2.0f * (q.y*q.z - q.w*q.x);

	// Check for gimbal lock, giving slight tolerance for numerical imprecision

	if (fabs(sp) > 0.9999f) {

		// Looking straight up or down

		pitch = kPiOver2 * sp;

		// Compute heading, slam bank to zero

		heading = atan2(-q.x*q.z + q.w*q.y, 0.5f - q.y*q.y - q.z*q.z);
		bank = 0.0f;

	} else {

		// Compute angles.  We don't have to use the "safe" asin
		// function because we already checked for range errors when
		// checking for gimbal lock

		pitch	= asin(sp);
		heading	= atan2(q.x*q.z + q.w*q.y, 0.5f - q.x*q.x - q.y*q.y);
		bank	= atan2(q.x*q.y + q.w*q.z, 0.5f - q.x*q.x - q.z*q.z);
	}
}

/// \param q Specifies the quaternion to be converted.
/// \note See 10.6.6.
void	EulerAngles::fromInertialToObjectQuaternion(const Quaternion &q) {

	// Extract sin(pitch)

	float sp = -2.0f * (q.y*q.z + q.w*q.x);

	// Check for gimbal lock, giving slight tolerance for numerical imprecision

	if (fabs(sp) > 0.9999f) {

		// Looking straight up or down

		pitch = kPiOver2 * sp;

		// Compute heading, slam bank to zero

		heading = atan2(-q.x*q.z - q.w*q.y, 0.5f - q.y*q.y - q.z*q.z);
		bank = 0.0f;

	} else {

		// Compute angles.  We don't have to use the "safe" asin
		// function because we already checked for range errors when
		// checking for gimbal lock

		pitch	= asin(sp);
		heading	= atan2(q.x*q.z - q.w*q.y, 0.5f - q.x*q.x - q.y*q.y);
		bank	= atan2(q.x*q.y - q.w*q.z, 0.5f - q.x*q.x - q.z*q.z);
	}
}

/// \param m Specifies the matrix to be converted.
/// \remarks The translation portion of the matrix is ignored, and the
///     matrix is assumed to be orthogonal.
void	EulerAngles::fromObjectToWorldMatrix(const Matrix4x3 &m) {

	// Extract sin(pitch) from m32.

	float	sp = -m.m32;

	// Check for gimbal lock
	
	if (fabs(sp) > 9.99999f) {

		// Looking straight up or down

		pitch = kPiOver2 * sp;

		// Compute heading, slam bank to zero

		heading = atan2(-m.m23, m.m11);
		bank = 0.0f;

	} else {

		// Compute angles.  We don't have to use the "safe" asin
		// function because we already checked for range errors when
		// checking for gimbal lock

		heading = atan2(m.m31, m.m33);
		pitch = asin(sp);
		bank = atan2(m.m12, m.m22);
	}
}

/// \param m Specifies the matrix to be converted.
/// \remarks The translation portion of the matrix is ignored, and the
///     matrix is assumed to be orthogonal.
void	EulerAngles::fromWorldToObjectMatrix(const Matrix4x3 &m) {

	// Extract sin(pitch) from m23.

	float	sp = -m.m23;

	// Check for gimbal lock
	
	if (fabs(sp) > 9.99999f) {

		// Looking straight up or down

		pitch = kPiOver2 * sp;

		// Compute heading, slam bank to zero

		heading = atan2(-m.m31, m.m11);
		bank = 0.0f;

	} else {

		// Compute angles.  We don't have to use the "safe" asin
		// function because we already checked for range errors when
		// checking for gimbal lock

		heading = atan2(m.m13, m.m33);
		pitch = asin(sp);
		bank = atan2(m.m21, m.m22);
	}
}

/// \param m Specifies the matrix to be converted.
/// \remarks The matrix is assumed to be orthogonal.
void	EulerAngles::fromRotationMatrix(const RotationMatrix &m) {

	// Extract sin(pitch) from m23.

	float	sp = -m.m32;

	// Check for gimbal lock
	
	if (fabs(sp) > 9.99999f) {

		// Looking straight up or down

		pitch = kPiOver2 * sp;

		// Compute heading, slam bank to zero

		heading = atan2(-m.m13, m.m11);
		bank = 0.0f;

	} else {

		// Compute angles.  We don't have to use the "safe" asin
		// function because we already checked for range errors when
		// checking for gimbal lock

		heading = atan2(m.m31, m.m33);
		pitch = asin(sp);
		bank = atan2(m.m12, m.m22);
	}
}



