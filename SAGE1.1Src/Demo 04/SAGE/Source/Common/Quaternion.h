/// \file Quaternion.h
/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// Quaternion.h - Declarations for class Quaternion
//
// Visit gamemath.com for the latest version of this file.
//
// For more details, see Quaternion.cpp
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __QUATERNION_H_INCLUDED__
#define __QUATERNION_H_INCLUDED__

class Vector3;
class EulerAngles;

/// \brief A quaternion-based representation of an orientation in 3-space.
class Quaternion {
public:

// Public data

	// The 4 values of the quaternion.  Normally, it will not
	// be necessary to manipulate these directly.  However,
	// we leave them public, since prohibiting direct access
	// makes some operations, such as file I/O, unnecessarily
	// complicated.

  float w;  ///< Specifies the w-coordinate.
  float x;  ///< Specifies the x-coordinate.
  float y;  ///< Specifies the y-coordinate.
  float z;  ///< Specifies the z-coordinate.

// Public operations

	// Set to identity

	void	identity() { w = 1.0f; x = y = z = 0.0f; }  ///< Sets this to the identity quaternion.

	// Setup the quaternion to a specific rotation

	void	setToRotateAboutX(float theta);  ///< Sets this to a rotation about the x-axis.
	void	setToRotateAboutY(float theta);  ///< Sets this to a rotation about the y-axis.
	void	setToRotateAboutZ(float theta);  ///< Sets this to a rotation about the z-axis.
	void	setToRotateAboutAxis(const Vector3 &axis, float theta);  ///< Sets this to a rotation about a given origin-centered axis.

	// Setup to perform object<->inertial rotations,
	// given orientation in Euler angle format

	void	setToRotateObjectToInertial(const EulerAngles &orientation);  ///< Sets this to perform an object-to-inertial rotation.
	void	setToRotateInertialToObject(const EulerAngles &orientation);  ///< Sets this to perform an inertial-to-object rotation.

	// Dot and cross products

  float dotProduct(const Quaternion &a) const;  ///< Computes the dot product of this quaternion and another.
  
  /// \brief Computes the dot product of two quaternions.
  /// \param a Specifies the first factor.
  /// \param b Specifies the second factor.
  /// \return The dot product of the two quaternions.
  static float dotProduct(const Quaternion &a, const Quaternion &b) {
    return a.dotProduct(b);
  }
  
  Quaternion crossProduct(const Quaternion &a) const;  ///< Computes the cross product of this quaternion and another.
  
  /// \brief Computes the cross product of two quaternions.
  /// \param a Specifies the first factor.
  /// \param b Specifies the second factor.
  /// \return The cross product of the two quaternions.
  static Quaternion crossProduct(const Quaternion &a, const Quaternion &b) {
    return a.crossProduct(b);
  }
  
	/// \brief Computes the cross product of this quaternion and another.
	/// \param a Specifies the second factor.
	/// \return The cross product of the two quaternions.
	Quaternion operator *(const Quaternion &a) const {
	  return crossProduct(a);
	}

	// Multiplication with assignment, as per C++ convention

	Quaternion &operator *=(const Quaternion &a);  ///< Computes and conserves the cross product of this quaternion and another.

	// Normalize the quaternion.

	void	normalize();

	// Extract and return the rotation angle and axis.

	float	getRotationAngle() const;  ///< Queries the quaternion for its angle of rotation.
	Vector3	getRotationAxis() const;	///< Queries the quaternion for its axis of rotation.
	
  static Quaternion slerp(const Quaternion &p, const Quaternion &q, float t);  ///< Performs spherical linear interpolation between two quaternions.
  
  Quaternion conjugate() const;  ///< Calculates the quaternion conjugate (i.e., the quaternion of opposite rotation).
  
  static Quaternion conjugate(const Quaternion &q) {
    return q.conjugate();
  }
  
  Quaternion pow(float exponent) const;  ///< Performs quatenion exponentiation.
  
  /// \brief Performs quaternion exponentiation.
  /// \param q Specifies the base quaternion to be raised.
  /// \param exponent Specifies the exponent.
  /// \return The quaternion \p q raised to the exponent \p exponent.
  static Quaternion pow(const Quaternion &q, float exponent) {
    return q.pow(exponent);
  }

  static const Quaternion kQuaternionIdentity;  ///< The identity quaternion.
};


/////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __QUATERNION_H_INCLUDED__
