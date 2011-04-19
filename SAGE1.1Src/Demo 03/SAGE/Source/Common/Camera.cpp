/// \file camera.cpp
#include "common/Renderer.h"
#include "camera.h"
#include "mathutil.h"
#include "RotationMatrix.h"

// constructor
Camera::Camera()
{
  fov = 60.0f;
  cameraPos = Vector3(0.0f,0.0f,0.0f);
  cameraOrient = EulerAngles(0.0f,0.0f,0.0f);  
}

// Returns the view matrix of the camera
/// \param Pointer to a matrix to be filled in with the view matrix
void Camera::getViewMatrix(Matrix4x3 * matrix)
{
  if (matrix)
    matrix->setupParentToLocal(cameraPos, cameraOrient);
}

// sets camera as current camera
void Camera::setAsCamera()
{    
  gRenderer.setCamera(cameraPos, cameraOrient);
  gRenderer.setZoom(fovToZoom(degToRad(fov)));
}


// sets the orientation of the camera based on a look vector
/// \param look direction that the camera is looking.
/// \remark This assumes that there is no roll.
void Camera::setOrientation (Vector3 look)
{
  // Get normalized direction vector
  look.normalize();

  // Compute heading/pitch to look in given direction
  cameraOrient.heading = atan2(look.x, look.z);
  cameraOrient.pitch = -asin(look.y);
  cameraOrient.bank = 0.0f;
}

// resets the cmaera to its default position
void Camera::reset() 
{
  cameraOrient = EulerAngles::kEulerAnglesIdentity;    
  cameraPos = Vector3::kZeroVector;
}
  