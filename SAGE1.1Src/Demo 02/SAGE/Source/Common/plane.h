/// \file plane.h
#include "vector3.h"
#include "eulerAngles.h"
#ifndef __PLANE_H_INCLUDED__
#define __PLANE_H_INCLUDED__

//-----------------------------------------------------------------------------
/// \brief Mathematical plane
//
/// The plane is described with 4 variables a,b,c,d where aX + bY + cZ - d = 0.
class Plane
{
public:

  float a;  
  float b;  
  float c;  
  float d;
      
  /// \brief Default Constructor.
  Plane() {a = 0; b =0; c = 1.0; d = 0.0f;}

  /// \brief Constructor creates a plane given a,b,c and d
  Plane(float a, float b, float c, float d)
  { this->a = a; this->b = b; this->c = c; this->d = d; }

  /// \brief Casting
  //{@ Casting  
  operator float* () {return &a;} ///< Cast to an array of 4 floats
  operator const float* () {return &a;} ///< Cast to an array of 4 const floats
  //@}

  void normalize(); ///< Normalizes the plane.

  Vector3 closestPoint(const Vector3& point) const;  ///< Gives the point closest to the point passed in that is on the plane
  Vector3 reflectPoint(const Vector3& point) const;  ///< Reflects a point accross the plane.
  Vector3 reflectOrientation(const Vector3& vector) const;  ///< Reflects an unit vector over the plane.
  EulerAngles reflectOrientation(const EulerAngles& angles) const;  ///< Reflects euler angles across the plane.

};

#endif