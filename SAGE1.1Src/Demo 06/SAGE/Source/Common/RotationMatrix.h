/// \file RotationMatrix.h
/// \brief Interface for the RotationMatrix class.

/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// RotationMatrix.h - Declarations for class RotationMatrix
//
// Visit gamemath.com for the latest version of this file.
//
// For more details, see RotationMatrix.cpp
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __ROTATIONMATRIX_H_INCLUDED__
#define __ROTATIONMATRIX_H_INCLUDED__

class Vector3;
class EulerAngles;
class Quaternion;

//---------------------------------------------------------------------------
// class RotationMatrix
//
// Implement a simple 3x3 matrix that is used for ROTATION ONLY.  The
// matrix is assumed to be orthogonal.  The direction of transformation
// is specified at the time of transformation.

/// \brief Implements a 3x3 rotation matrix in 3-space.
///
/// Implements a 3x3 rotation matrix in 3-space.  The matrix is assumed
/// to be orthogonal.
class RotationMatrix {
public:

// Public data

	// The 9 values of the matrix.  See RotationMatrix.cpp file for
	// the details of the layout

	float	m11;  ///< Specifies row one, column one
	float	m12;  ///< Specifies row one, column two
	float	m13;  ///< Specifies row one, column three
	float	m21;  ///< Specifies row two, column one
	float	m22;  ///< Specifies row two, column two
	float	m23;  ///< Specifies row two, column three
	float	m31;  ///< Specifies row three, column one
	float	m32;  ///< Specifies row three, column two
	float	m33;  ///< Specifies row three, column three

// Public operations

	// Set to identity

	void	identity();  ///< Sets this to the identity matrix.


	// Setup the matrix with a specified orientation

	void	setup(const EulerAngles &orientation);  ///< Sets this to the equivalent of the given Euler angle triple.
	void	fromInertialToObjectQuaternion(const Quaternion &q);  ///< Sets this to the equivalent of the given inertial-to-object quaternion.
	void	fromObjectToInertialQuaternion(const Quaternion &q);  ///< Sets this to the equivalent of the given inertial-to-object quaternion.

	// Perform rotations

	Vector3	inertialToObject(const Vector3 &v) const;  ///< Transforms a vector from inertial to object space.
	Vector3	objectToInertial(const Vector3 &v) const;  ///< Transforms a vector from object to inertial space.
};

/////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __ROTATIONMATRIX_H_INCLUDED__
