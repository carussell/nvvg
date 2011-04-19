#include "plane.h"
#include "RotationMatrix.h"
#include "Matrix4x3.h"



/// Changes a,b,c,d so that (a,b,c) is a unit vector.  d is also changed
/// based on the scaling needed to bring (a,b,c) to a unit vector.
void Plane::normalize()
{
    
  float mag = Vector3(a,b,c).magnitude();
  
  a /= mag;
  b /= mag;
  c /= mag;
  d /= mag;
  
  return;
}

/// \param point Specifies the point in question.
/// \return The point on the plane that is closest to the point passed in.
Vector3 Plane::closestPoint(const Vector3& point) const
{
  Vector3 out;
  Vector3 normal(a,b,c);

  out = point + (-d - Vector3::dotProduct(point, normal)) * normal;

  return out;
}

/// \param point Point to flip of the plane
/// \return The point passed in flipped over the plane
Vector3 Plane::reflectPoint(const Vector3& point) const
{
  Vector3 out;
  Vector3 closest;

  closest = closestPoint(point);
  
  out = 2.0f * closest - point;

  return out;  
}

/// \param vector The orientation (unit) vector that you want to reflect
/// \return A unit vector that is reflected about the slope of the plane.
Vector3 Plane::reflectOrientation(const Vector3& vector) const
{
  Vector3 out;
  Plane tempPlane(a,b,c,0.0f);

  out = tempPlane.reflectPoint(vector);

  return out;
}

/// \param angles The orientation (unit) vector that you want to reflect
/// \return Euler angles that are reflected about the slope of the plane.
EulerAngles Plane::reflectOrientation(const EulerAngles& angles) const
{
  EulerAngles out;
  Vector3 look, up, right;  
  RotationMatrix rotMat;
    
  // get look and up vector from euler angles
  rotMat.setup(angles); 
  look = rotMat.objectToInertial(Vector3(0.0f,0.0f,1.0f));
  up = rotMat.objectToInertial(Vector3(0.0f,1.0f,0.0f));

  // reflect the look and up vectors over the plane
  look = reflectOrientation(look);
  up = -reflectOrientation(up); 
  
  // calculate right vector
  right = Vector3::crossProduct( up, look);
  right.normalize();
    
  // create a rotation matrix from right, up, and look
  rotMat.m11 = right.x; rotMat.m12 = right.y; rotMat.m13 = right.z;
  rotMat.m21 = up.x; rotMat.m22 = up.y; rotMat.m23 = up.z;
  rotMat.m31 = look.x; rotMat.m32 = look.y; rotMat.m33 = look.z;
  
  // calculate new euler angles from the matrix
  out.fromRotationMatrix(rotMat);

  return out;
}