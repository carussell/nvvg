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

/// \file BulletObject.cpp
/// \brief Code for the BulletObject class.

/// Edited by the SAGE team, 2005
/// Last updated June 13, 2006


#include <assert.h>
#include "Common/RotationMatrix.h"
#include "ObjectTypes.h"
#include "BulletObject.h"

const float gBulletRange = 2000.0f;

BulletObject::BulletObject(float range) :
  GameObject(NULL,1),
  m_range(range),
  m_framesLeft(1),
  m_victim(NULL),
  m_victimTime(1.0f)
{
  m_className = "Bullet";
  m_type = ObjectTypes::BULLET;
  updateRay();
}

void BulletObject::process(float dt)
{
  // Lasts only one frame
  if(m_framesLeft <= 0)
  {
    m_lifeState = LS_DEAD;
    return;
  }
  if(dt != 0.0f)
  {
    --m_framesLeft;
    // Anything to do?
    updateRay();
  }
}

void BulletObject::render()
{
  // Invisibullet
}

bool BulletObject::checkForBoundingBoxCollision(GameObject *victim)
{
  if(victim == NULL) return false;
  float t = victim->getBoundingBox().rayIntersect(m_v3Position[0],m_bulletRay);
  if(t < m_victimTime)
  {
    m_victimTime = t;
    m_victim = victim;
    return true;
  }
  return false;
}

GameObject *BulletObject::getVictim()
{
  return m_victim;
}

void BulletObject::updateRay()
{
  EulerAngles lookEuler = getOrientation();
  lookEuler.bank = 0.0f;
  RotationMatrix look;
  look.setup(lookEuler);
  m_bulletRay = look.objectToInertial(Vector3(0.0f,0.0f,m_range));
}