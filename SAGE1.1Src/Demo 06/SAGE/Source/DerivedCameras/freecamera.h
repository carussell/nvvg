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

/// \file freecamera.h
/// \brief Interface for the FreeCamera class.

#ifndef __FREECAMERA_H_INCLUDED__
#define __FREECAMERA_H_INCLUDED__
#include "common/camera.h"

/// \brief Camera that is not tied to a particular object.
class FreeCamera : public Camera // Tether camera
{ 
public:
  /// \brief Basic constructor
  FreeCamera() {reset();}

  void reset(); ///< Resets speed, location, and orientation  
  void process(float elapsed); ///< Process input and movement.
  
  /// \brief Sets the speed of the free camera.
  /// \param unitsPerSecond The new speed of the camera in Units per second.
  void setSpeed(float unitsPerSecond) {m_speed = unitsPerSecond;}
  
private:
    
  EulerAngles m_cameraOrientMoving; ///< Used to smooth camera movement
  
  float m_speed; ///< Units per second the camera travels

};

#endif