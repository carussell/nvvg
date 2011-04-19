/// \file Vector3.h
/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// Vector3.h - Declarations for 3D vector class
//
// Visit gamemath.com for the latest version of this file.
//
// For additional comments, see Chapter 6.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __VECTOR3_H_INCLUDED__
#define __VECTOR3_H_INCLUDED__

#include <math.h>

/////////////////////////////////////////////////////////////////////////////
//
// class Vector3 - a simple 3D vector class
//
/////////////////////////////////////////////////////////////////////////////

/// \brief A representation of a three-dimensional vector.
class Vector3 {
public:

// Public representation:  Not many options here.

	float x; ///< Specifies the x-coordinate.
	float y; ///< Specifies the y-coordinate.
	float z; ///< Specifies the z-coordinate.

// Constructors

	// Default constructor leaves vector in
	// an indeterminate state

	Vector3() {}  ///< Constructs a vector of indeterminate value.

	// Copy constructor

  /// \brief Constructs a deep copy of a vector.
  /// \param a Specifies the vector to be copied.
	Vector3(const Vector3 &a) : x(a.x), y(a.y), z(a.z) {}

	// Construct given three values

  /// \brief Constructs a vector with the specified coordinates.
  /// \param nx Specifies the x-coordinate.
  /// \param ny Specifies the y-coordinate.
  /// \param nz Specifies the z-coordinate.
	Vector3(float nx, float ny, float nz) : x(nx), y(ny), z(nz) {}

// Standard object maintenance

	// Assignment.  We adhere to C convention and
	// return reference to the lvalue

	/// \brief Assigns a deep copy of a vector to this.
	/// \param a Specifies the vector to be copied.
	/// \return A reference to this vector.
	Vector3 &operator =(const Vector3 &a) {
		x = a.x; y = a.y; z = a.z;
		return *this;
	}

	// Check for equality

  /// \brief Checks this vector and another for equality.
  /// \param a Specifies the vector to be compared.
  /// \return true iff the two vectors are equivalent.
	bool operator ==(const Vector3 &a) const {
		return x==a.x && y==a.y && z==a.z;
	}

  /// \brief Checks this vector and another for strict inequality.
  /// \param a Specifies the vector to be compared.
  /// \return true iff the two vectors are not equivalent.
	bool operator !=(const Vector3 &a) const {
		return x!=a.x || y!=a.y || z!=a.z;
	}


// Vector operations

	// Set the vector to zero

  /// \brief Sets this vector to the zero vector.
	void zero() { x = y = z = 0.0f; }

	// Unary minus returns the negative of the vector

  /// \brief Negates the vector
  /// \return The additive inverse (negation) of this vector.
	Vector3 operator -() const { return Vector3(-x,-y,-z); }

	// Binary + and - add and subtract vectors

  /// \brief Adds a vector to this vector.
  /// \param a Specifies the vector to be added to this.
  /// \return The sum of the two vectors.
	Vector3 operator +(const Vector3 &a) const {
		return Vector3(x + a.x, y + a.y, z + a.z);
	}

  /// \brief Subtracts a vector from this vector.
  /// \param a Specifies the vector to be subtracted from this.
  /// \return The difference of the two vectors.
	Vector3 operator -(const Vector3 &a) const {
		return Vector3(x - a.x, y - a.y, z - a.z);
	}

	// Multiplication and division by scalar

	/// \brief Multiplies this vector by a scalar.
	/// \param a Specifies the scalar factor.
	/// \return The product of the scalar and vector.
	Vector3 operator *(float a) const {
		return Vector3(x*a, y*a, z*a);
	}

	/// \brief Multiplies a vector by a scalar.
	/// \param k Specifies the scalar factor.
	/// \param v Specifies the vector to be scaled.
	/// \return The product of the scalar and vector.
	friend Vector3 operator *(float k,const Vector3 &v) {
	  return Vector3(k*v.x, k*v.y, k*v.z);
	}

  /// \brief Divides this vector by a scalar.
  /// \param a Specifies the scalar divisor, which must not be zero.
  /// \return The quotient of the vector and scalar.
  /// \warning An attempt to pass zero into this operator will
  ///     result in a divide-by-zero error.
  ///
  /// Divides this vector by a scalar.  Equivalent to multiplying
  /// the vector by 1/a.
	Vector3 operator /(float a) const {
		float	oneOverA = 1.0f / a; // NOTE: no check for divide by zero here
		return Vector3(x*oneOverA, y*oneOverA, z*oneOverA);
	}

	// Combined assignment operators to conform to
	// C notation convention

  /// \brief Adds a vector to this and conserves the result.
  /// \param a Specifies the vector to be added.
  /// \return A reference to this now-modified vector.
	Vector3 &operator +=(const Vector3 &a) {
		x += a.x; y += a.y; z += a.z;
		return *this;
	}

  /// \brief Subtracts a vector from this and conserves the result.
  /// \param a Specifies the vector to be subtracted.
  /// \return A reference to this now-modified vector.
	Vector3 &operator -=(const Vector3 &a) {
		x -= a.x; y -= a.y; z -= a.z;
		return *this;
	}

  /// \brief Multiplies this by a scalar and conserves the result.
  /// \param a Specifies the scalar factor.
  /// \return A reference to this now-modified vector.
	Vector3 &operator *=(float a) {
		x *= a; y *= a; z *= a;
		return *this;
	}

  /// \brief Divides this by a scalar and conserves the result.
  /// \param a Specifies the scalar divisor.
  /// \return A reference to this now-modified vector.
  /// \warning An attempt to pass zero into this operator will
  ///     result in a divide-by-zero error.
  ///
  /// Divides this vector by a scalar and conserves the result.
  /// Equivalent to multiplyingthe vector by 1/a.
	Vector3 &operator /=(float a) {
		float	oneOverA = 1.0f / a;
		x *= oneOverA; y *= oneOverA; z *= oneOverA;
		return *this;
	}

  /// \brief Sets the vector's components.
  /// \param v Specifies the vector to be copied.
  /// \return A reference to the vector.
  Vector3 &set(const Vector3 &v) {
    return (*this = v);
  }

  /// \brief Sets the vector's components.
  /// \param nx Specifies the x-coordinate.
  /// \param ny Specifies the y-coordinate.
  /// \param nz Specifies the z-coordinate.
  /// \return A reference to the vector.
  Vector3 &set(float nx, float ny, float nz) {
    x = nx; y = ny; z = nz; return *this;
  }

	// Normalize the vector

  /// \brief Normalizes the vector to unit length.
  ///
  /// Normalizes the vector to unit length.  If this vector is
  /// the zero vector, does nothing.
	void	normalize() {
		float magSq = x*x + y*y + z*z;
		if (magSq > 0.0f) { // check for divide-by-zero
			float oneOverMag = 1.0f / sqrt(magSq);
			x *= oneOverMag;
			y *= oneOverMag;
			z *= oneOverMag;
		}
	}

	// Vector dot product.  We overload the standard
	// multiplication symbol to do this

  /// \brief Computes the dot product of this vector and another.
  /// \param a Specifies the second factor.
  /// \return The dot product of the two vectors.
	float operator *(const Vector3 &a) const {
	  return dotProduct(a);
	}
	
	/// \brief Queries the vector for its magnitude.
	/// \return The magnitude of the vector.
	/// \remark Since a square root is required, use this function
	///     only when exact magnitudes are needed.  Otherwise, use
	///     magnitude squared.
	float magnitude() const {
	  return sqrt(x*x + y*y + z*z);
	}

	/// \brief Queries the vector for its squared magnitude.
	/// \return The squared magnitude of the vector.
	/// \remark Since no square root is required, use this function
	///     when only relative magnitudes are needed (such as when
	///     sorting vectors by magnitude).
	float magnitudeSquared() const {
	  return x*x + y*y + z*z;
	}
	
  /// \brief Computes the cross product of this vector and another.
  /// \param a Specifies the second factor.
  /// \return The cross product of the two vectors.
  Vector3 crossProduct(const Vector3 &a) const {
	  return Vector3(
		  y*a.z - z*a.y,
		  z*a.x - x*a.z,
		  x*a.y - y*a.x
	  );
  }

  /// \brief Computes the crossproduct of two vectors.
  /// \param a Specifies the first factor.
  /// \param b Specifies the second factor.
  /// \return The cross product of the two vectors.
  static Vector3 crossProduct(const Vector3 &a, const Vector3 &b) {
    return a.crossProduct(b);
  }
  
  /// \brief Computes the dot product of this vector and another.
  /// \param a Specifies the second factor.
  /// \return The dot product of the two vectors.
  float dotProduct(const Vector3 &a) const {
		return x*a.x + y*a.y + z*a.z;
  }

  /// \brief Computes the dot product of two vectors.
  /// \param a Specifies the first factor.
  /// \param b Specifies the second factor.
  /// \return The dot product of the two vectors.
  static float dotProduct(const Vector3 &a, const Vector3 &b) {
    return a.dotProduct(b);
  }
  
  /// \brief Computes the distance between this vector and another.
  /// \param a Specifies the other vector.
  /// \return The distance between the two vectors.
  float distance(const Vector3 &a) const {
    float dx = x - a.x;
    float dy = y - a.y;
    float dz = z - a.z;
    return sqrt(dx*dx + dy*dy + dz*dz);
  }
  
  /// \brief Computes the distance between two vectors.
  /// \param a Specifies the first vector.
  /// \param b Specifies the second vector.
  /// \return The distance between the two vectors.
	static float distance(const Vector3 &a, const Vector3 &b) {
	  return a.distance(b);
	}

  /// \brief Computes the squared distance between this vector and another.
  /// \param a Specifies the other vector.
  /// \return The squared distance between the two vectors.
  float distanceSquared(const Vector3 &a) const {
	  float dx = x - a.x;
	  float dy = y - a.y;
	  float dz = z - a.z;
	  return dx*dx + dy*dy + dz*dz;
  }
  
  /// \brief Computes the squared distance between two vectors.
  /// \param a Specifies the first vector.
  /// \param b Specifies the second vector.
  /// \return The squared distance between the two vectors.
  static float distanceSquared(const Vector3 &a, const Vector3 &b) {
    return a.distanceSquared(b);
  }

  /// \brief The zero vector.
  /// \note Defined in MathUtil.cpp
  static const Vector3 kZeroVector;
  
  /// \brief The x unit vector.
  /// \note Defined in MathUtil.cpp
  static const Vector3 kRightVector;
  
  /// \brief The y unit vector.
  /// \note Defined in MathUtil.cpp
  static const Vector3 kUpVector;
  
  /// \brief The z unit vector.
  /// \note Defined in MathUtil.cpp
  static const Vector3 kForwardVector;
};

#endif // #ifndef __VECTOR3_H_INCLUDED__
