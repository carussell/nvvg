/// \file EulerAngles.h
/// \brief Interface for the EulerAngles class.

/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// EulerAngles.h - Declarations for class EulerAngles
//
// Visit gamemath.com for the latest version of this file.
//
// For more details, see EulerAngles.cpp
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __EULERANGLES_H_INCLUDED__
#define __EULERANGLES_H_INCLUDED__

// Forward declarations

class Quaternion;
class Matrix4x3;
class RotationMatrix;

//---------------------------------------------------------------------------
// class EulerAngles
//
// This class represents a heading-pitch-bank Euler angle triple.

/// \brief An Euler-angle-based representation of orientation.
class EulerAngles {
public:

// Public data

	// Straightforward representation.  Store the three angles, in
	// radians

	float	heading; ///< Specifies the heading (yaw) angle.
	float	pitch;   ///< Specifies the pitch angle.
	float	bank;    ///< Specifies the bank (roll) angle.

// Public operations

	// Default constructor does nothing

  /// \brief Constructs an indeterminate Euler angle triple.
	EulerAngles() {}

	// Construct from three values

  /// \brief Constructs an Euler angle triple with the given angles.
  /// \param h Specifies the heading angle.
  /// \param p Specifies the pitch angle.
  /// \param b Specifies the bank angle.
	EulerAngles(float h, float p, float b) :
		heading(h), pitch(p), bank(b) {}

	// Set to identity triple (all zeros)

  /// \brief Sets this to the identity orientation (all zeros).
	void	identity() { pitch = bank = heading = 0.0f; }

  /// \brief Sets the angles to given values
  /// \param h Specifies the heading angle
  /// \param p Specifies the pitch angle
  /// \param b Specifies the bank angle
  /// \return A reference to the angles.
  EulerAngles &set(float h, float p, float b) { heading = h; pitch = p; bank = b; return *this; }
  
  /// \brief Copies another set of Euler angles.
  /// \param ea Specifies the angles to be copied.
  /// \return A reference to the angles.
  EulerAngles &set(const EulerAngles &ea) { return (*this = ea); }
  
	void	canonize();   ///< Sets this to the equivalent canonical Euler angles.
	void	fromObjectToInertialQuaternion(const Quaternion &q);   ///< Sets this to the Euler equivalent of an object-to-inertial quaternion.
	void	fromInertialToObjectQuaternion(const Quaternion &q);   ///< Sets this to the Euler equivalent of an inertial-to-object quaternion.
	void	fromObjectToWorldMatrix(const Matrix4x3 &m);   ///< Sets this to the Euler equivalent of an object-to-world matrix.
	void	fromWorldToObjectMatrix(const Matrix4x3 &m);  ///< Sets this to the Euler equivalent of a world-to-object matrix.
	void	fromRotationMatrix(const RotationMatrix &m);  ///< Sets this to the Euler equivalent of a rotation matrix.

	static const EulerAngles kEulerAnglesIdentity;   ///< The identity Euler angle triplet	(all zeros).

  /// \brief Divides a Euler angle by a scalar.
	/// \param a Specifies the scalar factor.
	/// \return The Euler angle divided by the scaler.
  EulerAngles	operator /(const float a)
  {  
    EulerAngles angle;
    angle.pitch = pitch / a;
    angle.bank = bank / a;
    angle.heading = heading / a;
    return angle;
  }

  /// \brief Divides this by a Euler angle and conserves the result.
  /// \param a Specifies the scalar divisor.
  /// \return A reference to this now-modified Euler angle.
  /// \warning An attempt to pass zero into this operator will
  ///     result in a divide-by-zero error.
  ///
  /// Divides this Euler angle by a scalar and conserves the result.
  /// Equivalent to multiplyingthe Euler angle by 1/a.
  EulerAngles	&operator /=(const float a)
  {  
    pitch /= a;
    bank /= a;
    heading /= a;
    return *this;
  }

  /// \brief Multiplies a Euler angle by a scalar.
	/// \param a Specifies the scalar factor.
	/// \return The Euler angle scaled by the scaler.
  EulerAngles	operator *(const float a)
  {  
    EulerAngles angle;
    angle.pitch = pitch * a;
    angle.bank = bank * a;
    angle.heading = heading * a;
    return angle;
  }

  /// \brief Multiplies this Euler Angle by a scalar and conserves the result.
  /// \param a Specifies the scalar factor.
  /// \return A reference to this now-modified Euler Angle.
	EulerAngles &operator *=(float a) {
		pitch *= a; bank *= a; heading *= a;
		return *this;
	}

  /// \brief Multiplies a Euler angle by a scalar.
	/// \param a Specifies the scalar factor.
	/// \param b Specifies the Euler angle to be scaled.
	/// \return The Euler angle b scaled by scaler a.
  friend EulerAngles	operator *(const float a, const EulerAngles &b)
  {  
    EulerAngles angle;
    angle.pitch = b.pitch * a;
    angle.bank = b.bank * a;
    angle.heading = b.heading * a;
    return angle;
  }

  /// \brief Adds two Euler angles together.
  /// \param a Euler angle to add.
  /// \return Sum of the two Euler angles.
	EulerAngles operator +(const EulerAngles &a) 
  {
    EulerAngles angle;
    angle.pitch = pitch + a.pitch;
    angle.heading = heading + a.heading;
    angle.bank = bank + a.bank;		
		return angle;
	}

  /// \brief Adds a Euler angle to this and conserves the result.
  /// \param a Specifies the Euler angle to be added.
  /// \return A reference to this now-modified Euler angle.
	EulerAngles &operator +=(const EulerAngles &a) {
		pitch += a.pitch; bank += a.bank; heading += a.heading;
		return *this;
	}
};



/////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __EULERANGLES_H_INCLUDED__
