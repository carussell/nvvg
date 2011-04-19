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

/// \file BulletObject.h
/// \brief Interface for the BulletObject class.

/// Edited by the SAGE team, 2005
/// Last updated June 13, 2006

#ifndef __BULLETOBJECT_H_INCLUDED__
#define __BULLETOBJECT_H_INCLUDED__

#include <set>
#include "Objects/GameObject.h"

extern const float gBulletRange; // The bullet range

/// \brief Represents a bullet fired from the plane.
class BulletObject : public GameObject
{
public:  
  friend class Ned3DObjectManager;
  
  BulletObject(float range = gBulletRange); ///< Constructs a bullet object.
  
  virtual void process(float dt); ///< Processes the bullet's game logic.
  virtual void render(); ///< Renders the bullet (in this case, does nothing.)
  
  /// \brief Checks whether the ray created by the bullet intersects the bounding box
  /// of the given object.
  /// \param victim Pointer to the GameObject to test against.
  /// \return True if the bullet ray intersects the objects bounding box.
  bool checkForBoundingBoxCollision(GameObject *victim);

  /// \brief Returns a pointer to the GameObject with which the bullet collided.
  /// \return The GameObject with which the bullet collided.
  GameObject *getVictim();
  
protected:
  void updateRay(); ///< Updates the bullet's path.

  float m_range; ///< Distance the bullet will travel.
  int m_framesLeft; ///< Number of frames the bullet has left to live.
  Vector3 m_bulletRay; ///< Direction of travel of the bullet.
  GameObject *m_victim; ///< Pointer to the GameObject with which the bullet collided
  float m_victimTime; ///< Parametric value indicating the point in time the bullet collided
};



#endif