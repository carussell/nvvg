/*
----o0o=================================================================o0o----
* Copyright (c) 2006, Ian Parberry
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the University of North Texas nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
----o0o=================================================================o0o----
*/

/// \file tethercamera.cpp
/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// Camera.cpp - camera utilities
//
// Visit gamemath.com for the latest version of this file.
//
/////////////////////////////////////////////////////////////////////////////

#include "common/renderer.h"
#include "common/RotationMatrix.h"
#include "DerivedCameras/tethercamera.h"
#include "objects/gameobjectmanager.h"
#include "objects/gameobject.h"
#include "assert.h"

/// \param objectManager Pointer to the object manager.  This is needed
/// to look up the location of the target object.
TetherCamera::TetherCamera (GameObjectManager* objectManager):
  m_objects(objectManager)  
{  
}

// Resets camera to behind target object
void TetherCamera::reset() 
{
  Vector3 target;
  float targetHeading;

  assert(m_objects); // object manager doesn't exist
  GameObject* obj = m_objects->getObjectPointer(m_targetObjectID);    
  
  assert(obj); // object to follow doesn't exist  
  targetHeading = obj->getOrientation().heading;
  target = obj->getPosition();

  
  target.y += 3.0f;

  cameraOrient.set(targetHeading, 0.0f,0.0f);

  RotationMatrix cameraMatrix;
  cameraMatrix.setup(cameraOrient);

  Vector3 bOffset(0.0f,0.0f, -maxDist);
  
  Vector3 iOffset = cameraMatrix.objectToInertial(bOffset);
  cameraPos = target + iOffset;
}

// brief Specfies a target object
/// \param objectID Object ID that can be used to retreive the target object
/// from the object manager.
void TetherCamera::setTargetObject(unsigned int objectID)
{
 m_targetObjectID = objectID;
 return;
}

// processes movement of the camera
/// \param elapsed The elapsed time in seconds since the last call to this
/// function.
void TetherCamera::process(float elapsed) 
{ 
  float targetHeading;
  Vector3 target;

  assert(m_objects); // check object manager  
  GameObject* obj = m_objects->getObjectPointer(m_targetObjectID);
  assert(obj); // check object
  targetHeading = obj->getOrientation().heading;
  target = obj->getPosition();

  target.y += 3.0f;

  // Compute current vector to target
  Vector3 iDelta = target - cameraPos;

  // Compute current distance
  float dist = iDelta.magnitude();

  // Get normalized direction vector
  Vector3 iDir = iDelta / dist;

  // Clamp within desired range to get distance
  // for next frame
  if (dist < minDist)
    dist = minDist;
  
  if (dist > maxDist) 
    dist = maxDist;

  // Recompute difference vector
  iDelta = iDir * dist;

  // Compute camera's position
  cameraPos = target - iDelta;

  // Compute heading/pitch to look in given direction
  cameraOrient.heading = atan2(iDir.x, iDir.z);
  cameraOrient.pitch = -asin(iDir.y);
  cameraOrient.bank = 0.0f;
}