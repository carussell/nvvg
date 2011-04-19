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

/// \file TetherCamera.h
#ifndef __TETHERCAMERA_H_INCLUDED__
#define __TETHERCAMERA_H_INCLUDED__

#include "common/EulerAngles.h"
#include "common/camera.h"

class GameObjectManager;

/// The tether camera class is derived from the base camera class.
/// This camera follows behind a target object.  The target object
/// can be set using the setTargetObject method.  Process must be
/// called once per frame after the target object has moved.
class TetherCamera : public Camera 
{ 
public:
  ///Basic Constructor
  TetherCamera (GameObjectManager* objectManager);

  void reset(); ///< Resets camera to behind target object
  
  float minDist; ///< Minimum following distance
  float maxDist; ///< Maximum following distance

  /// \brief Specfies a target object for the tether camera to follow
  void setTargetObject(unsigned int objectID);
  
  /// \brief Processes orientation and position.
  void process(float elapsed);

  protected:

  unsigned int m_targetObjectID; ///< Object to follow
  GameObjectManager* m_objects; ///< Pointer to the object manager
  
};

#endif