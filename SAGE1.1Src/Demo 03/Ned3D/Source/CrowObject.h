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


#ifndef __CROWOBJECT_H_INCLUDED__
#define __CROWOBJECT_H_INCLUDED__

#include "Objects/GameObject.h"

class CrowObject : public GameObject
{
public:
  friend class Ned3DObjectManager;

  enum MovementPattern
  {
    MP_HOVER,      ///< Crow hovers in place
    MP_STRAIGHT,   ///< Crow flies straight forward
    MP_CIRCLING    ///< Crow circles a point    
  };

  CrowObject(Model *m);  ///< Constructs a crow using the given model.
  ~CrowObject();         ///< Destroys the crow.
  
  // Standard frame functions
  
  virtual void process(float dt);
  virtual void move(float dt);
  
  
  // Crow-specific functions
  
  void setMovementPattern(MovementPattern pattern);
  void setCirclingParameters(const Vector3 &circleCenter, bool flyLeft);
  void setCirclingParameters(const Vector3 &circleCenter, bool flyLeft, float circleRadius);

protected:
  
  // Protected members
  
  MovementPattern m_movement; ///< Movement pattern.

  Vector3 m_v3Velocity; ///< Holds the velocity of the object
  
  // Movement parameters
  Vector3 m_circleCenter;
  bool m_circleLeft;
  float m_circleRadius;
  
  // Input states
};

#endif