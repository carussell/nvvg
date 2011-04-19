/// \file vector2.h
#ifndef __VECTOR2_H_INCLUDED__
#define __VECTOR2_H_INCLUDED__

struct Vector2
{
  Vector2() {y = x = 0.0f;}
  Vector2(float x, float y) {this->x = x; this->y = y;}  

  float x;
  float y;


  /// \brief Multiplies a vector by a scalar.
	/// \param k Specifies the scalar factor.
	/// \param v Specifies the vector to be scaled.
	/// \return The product of the scalar and vector.
	friend Vector2 operator *(float k,const Vector2 &v) {
	  return Vector2(k*v.x, k*v.y);
	}


  /// \brief Adds a vector to this and conserves the result.
  /// \param a Specifies the vector to be added.
  /// \return A reference to this now-modified vector.
	Vector2 &operator +=(const Vector2 &a) {
		x += a.x; y += a.y;
    return *this;
  }

};

#endif