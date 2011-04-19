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


#include <assert.h>
#include "Common/MathUtil.h"
#include "Common/RotationMatrix.h"
#include "CrowObject.h"
#include "ObjectTypes.h"

CrowObject::CrowObject(Model *m):
  GameObject(m,1,11),
  m_behavior(BS_CRUISING),
  m_movement(MP_HOVER),
  m_circleCenter(Vector3::kZeroVector),
  m_circleLeft(true)
{
  assert(m);
  assert(m->getPartCount() >= 1);
  setModelOrientation(EulerAngles(kPi, 0.0f, 0.0f));
  m_fSpeed = 0.0f;
  m_v3Velocity = Vector3::kZeroVector;
  m_className = "Crow";
  m_type = ObjectTypes::CROW;
}


CrowObject::~CrowObject()
{
}

void CrowObject::process(float dt)
{
  // Put any non-movement logic here

}

void CrowObject::move(float dt)
{

  switch (m_behavior)
  {
    case BS_CRUISING:
    {
      switch(m_movement)
      {
        case MP_CIRCLING:
        {
          // Correct orientation
          
          Vector3 right(m_v3Position[0].x - m_circleCenter.x, m_circleCenter.y, m_v3Position[0].z - m_circleCenter.z);
          const Vector3 &up = Vector3::kUpVector;
          Vector3 forward = up.crossProduct(right);
          if(!m_circleLeft)
          {
            forward *= -1.0f;
            right *= -1.0f;
          }
          
          RotationMatrix m;
          m.m11 = right.x;
          m.m12 = up.x;
          m.m13 = forward.x;
          m.m21 = right.y;
          m.m22 = up.y;
          m.m23 = forward.y;
          m.m31 = right.z;
          m.m32 = up.z;
          m.m33 = forward.z;
          m_eaOrient[0].fromRotationMatrix(m);

          // Update flapping speed (just happens to look good this way)
          m_animFreq = m_fSpeed * 1.6f;
          
          // Move it          
          GameObject::move(dt);

          forward.normalize();
          // calculate velocity from euler angles
          m_v3Velocity = forward * m_fSpeed * 20.0f; // TODO: 20
          

          // Clamp position to circle so we don't drift

          Vector3 radial(m_v3Position[0].x - m_circleCenter.x, 0.0f, m_v3Position[0].z - m_circleCenter.z);
          radial.normalize();
          radial *= m_circleRadius;
          m_v3Position[0].x = m_circleCenter.x + radial.x;
          m_v3Position[0].z = m_circleCenter.z + radial.z;
          
        } break;
        case MP_STRAIGHT:
        {
          GameObject::move(dt);
        } break;
        case MP_HOVER:
        {
          float tempSpeed = m_fSpeed;
          EulerAngles tempAngVel = m_eaAngularVelocity[0];
          m_fSpeed = 0.0f;
          m_eaAngularVelocity[0].identity();
          GameObject::move(dt);
          m_fSpeed = 0.0f;
          m_eaAngularVelocity[0] = tempAngVel;
        } break;
      }; // end cruising switch
    } break; // end case cruising

    case BS_DYING:
    {
      m_eaAngularVelocity[0].bank = 12.0f;
      float mag = m_v3Velocity.magnitude();
      m_eaOrient[0].pitch = -(float)asin((double)(m_v3Velocity.y/ mag));
      float gravity =  -10.0f;
      m_fSpeed = 0.0f;
      m_v3Velocity.y += gravity * dt;      
      m_v3Position[0] += m_v3Velocity * dt;           
      GameObject::move(dt);
    }break;




  } // end behavior switch
}

void CrowObject::setMovementPattern(MovementPattern pattern)
{
  m_movement = pattern;

  switch(pattern)
  {
    case MP_CIRCLING:
    {
    } break;
  }
}

void CrowObject::setCirclingParameters(const Vector3 &circleCenter, bool flyLeft)
{
  m_circleCenter = circleCenter;
  m_circleLeft = flyLeft;
  m_circleRadius = m_v3Position[0].distance(circleCenter);
}

void CrowObject::setCirclingParameters(const Vector3 &circleCenter, bool flyLeft, float circleRadius)
{
  m_circleCenter = circleCenter;
  m_circleLeft = flyLeft;
  m_circleRadius = circleRadius;
}

void CrowObject::setDying()
{
  m_behavior = BS_DYING;
  m_eaOrient[0].pitch = degToRad(30);
}

bool CrowObject::isDying()
{
  return (m_behavior == BS_DYING);
}

void CrowObject::killObject()
{
  GameObject::killObject();
}