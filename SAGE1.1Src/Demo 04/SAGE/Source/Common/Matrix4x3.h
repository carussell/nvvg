/// \file Matrix4x3.h
/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// Matrix4x3.h - Declarations for class Matrix4x3
//
// Visit gamemath.com for the latest version of this file.
//
// For more details, see Matrix4x3.cpp
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __MATRIX4X3_H_INCLUDED__
#define __MATRIX4X3_H_INCLUDED__

class Vector3;
class EulerAngles;
class Quaternion;
class RotationMatrix;
class Plane;

//---------------------------------------------------------------------------
// class Matrix4x3
//
// Implement a 4x3 transformation matrix.  This class can represent
// any 3D affine transformation.

/// \brief Implements a 4x3 transformation matrix in 3-space.
///
/// Implements a 4x3 transformation matrix--more accurately, a 4x4
/// transformation matrix with an implied right column of [0,0,0,1]^T.
/// Can represent any 3D affine transformation.
class Matrix4x3 {
public:

// Public data

	// The values of the matrix.  Basically the upper 3x3 portion
	// contains a linear transformation, and the last row is the
	// translation portion.  See the Matrix4x3.cpp for more
	// details.

  float m11;    ///< Specifies row one, column one.
  float m12;    ///< Specifies row one, column two.
  float m13;    ///< Specifies row one, column three.
  float m21;    ///< Specifies row two, column one.
  float m22;    ///< Specifies row two, column two.
  float m23;    ///< Specifies row two, column three.
  float m31;    ///< Specifies row three, column one.
  float m32;    ///< Specifies row three, column two.
  float m33;    ///< Specifies row three, column three.
  float tx;     ///< Specifies row four, column one.
  float ty;     ///< Specifies row four, column two.
  float tz;     ///< Specifies row four, column three.

// Public operations

	// Set to identity

	void	identity();  ///< Sets this to the identity matrix.

	// Access the translation portion of the matrix directly

	void	zeroTranslation();  ///< Sets the translation portion of this to the zero vector.
	
	void	setTranslation(const Vector3 &d);	///< Sets the translation portion of this to the given vector.

	void	setupTranslation(const Vector3 &d);	///< Sets this to a translation-only matrix using the given vector.

	// Setup the matrix to perform a specific transforms from parent <->
	// local space, assuming the local space is in the specified position
	// and orientation within the parent space.  The orientation may be
	// specified using either Euler angles, or a rotation matrix

	void	setupLocalToParent(const Vector3 &pos, const EulerAngles &orient);  ///< Sets this to a local-to-parent transformation given an orientation.

	void	setupLocalToParent(const Vector3 &pos, const RotationMatrix &orient);  ///< Sets this to a local-to-parent transformation given an orientation.

	void	setupParentToLocal(const Vector3 &pos, const EulerAngles &orient);  ///< Sets this to a parent-to-local transformation given an orientation.

	void	setupParentToLocal(const Vector3 &pos, const RotationMatrix &orient);  ///< Sets this to a parent-to-local transformation given an orientation.

	// Setup the matrix to perform a rotation about a cardinal axis

	void	setupRotateX(float theta);  ///< Sets this to a rotation matrix about the x-axis.

	void	setupRotateY(float theta);  ///< Sets this to a rotation matrix about the y-axis.

	void	setupRotateZ(float theta);  ///< Sets this to a rotation matrix about the z-axis.

	void	setupRotate(const Vector3 &axis, float theta);  ///< Sets this to a rotation matrix around a given axis.

	// Setup the matrix to perform a rotation, given
	// the angular displacement in quaternion form

	void	fromQuaternion(const Quaternion &q);  ///< Sets this to the rotational equivalent of a quaternion.

	// Setup the matrix to perform scale on each axis

	void	setupScale(const Vector3 &s);  ///< Sets this to a matrix that scales along each axis.

	// Setup the matrix to perform scale along an arbitrary axis

	void	setupScaleAlongAxis(const Vector3 &axis, float k);  ///< Sets this to a matrix that scales along an arbitrary axis.

	// Setup the matrix to perform a shear

	void	setupShear(int axis, float s, float t);  ///< Sets this to a shearing matrix along an axis.

	// Setup the matrix to perform a projection onto a plane passing
	// through the origin

	void	setupProject(const Vector3 &n);  ///< Sets this matrix to project onto a plane through the origin.

	// Setup the matrix to perform a reflection about a plane parallel
	// to a cardinal plane

	void	setupReflect(int axis, float k = 0.0f);  ///< Sets this matrix to reflect about a plane parallel to a cardinal plane.

	// Setup the matrix to perform a reflection about an arbitrary plane
	// through the origin

	void	setupReflect(const Vector3 &n);  ///< Sets the matrix to reflect about an arbitrary plane through the origin.

	void	setupReflect(const Plane &plane);  ///< Sets this to a reflection matrix over a given plane.

  float	determinant() const;  ///< Computes the determinant of the 3x3 portion of the matrix.

  // Compute the inverse of a matrix

  Matrix4x3 inverse() const;  ///< Computes the inverse of the matrix.

  // Extract the translation portion of the matrix

  Vector3	getTranslation() const;  ///< Queries the matrix for its translation vector.

  // Extract the position/orientation from a local->parent matrix,
  // or a parent->local matrix

  Vector3	getPositionFromParentToLocalMatrix() const;  ///< Queries a parent-to-local matrix for its position vector.
  
  Vector3	getPositionFromLocalToParentMatrix() const;  ///< Queries a local-to-parent matrix for its position vector.
};

// Operator* is used to transforms a point, and also concatenate matrices.
// The order of multiplications from left to right is the same as
// the order of transformations

Vector3		operator*(const Vector3 &p, const Matrix4x3 &m);  ///< Multiplies a vector by a matrix.

Matrix4x3	operator*(const Matrix4x3 &a, const Matrix4x3 &b);  ///< Concatenates two matrices.

// Operator *= for conformance to C++ standards

Vector3		&operator*=(Vector3 &p, const Matrix4x3 &m);  ///< Multiplies a vector by a matrix and conserves the result.

Matrix4x3	&operator*=(const Matrix4x3 &a, const Matrix4x3 &b);  ///< Concatenates two matrices and conserves the result.

/////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __ROTATIONMATRIX_H_INCLUDED__
