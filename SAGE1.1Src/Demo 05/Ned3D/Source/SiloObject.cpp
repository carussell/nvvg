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
#include "ObjectTypes.h"
#include "SiloObject.h"

SiloObject::SiloObject(Model *m)
  : GameObject(m,1)
{
  assert(m);
  assert(m->getPartCount() >= 1);
  m_className = "Silo";
  m_type = ObjectTypes::SILO;
  //m_smokeID = -1;

  //m_allParticles.resize(2);
  //m_allParticles[0] = "smokeveryheavy";   // when hp = 0
  //m_allParticles[1] = "";             // hp > 2

  //setTextureAndSmoke();
}

void SiloObject::process(float dt)
{
}

void SiloObject::move(float dt)
{
  GameObject::move(dt);
}
/*
SiloObject::~SiloObject()
{
  // kill particle engine if one is attached
  if (m_smokeID != -1)
    gParticle.killSystem(m_smokeID);
  m_smokeID = -1;

}
//unlike plane's smoke, this will be called on death and 
// will make smoke appear
void SiloObject::setTextureAndSmoke()
{
   
  int smokeIndex = m_hp; // index into m_allParticles array
  // make sure the indicies are in range
  if (smokeIndex >= (int)m_allParticles.size())
    smokeIndex = (int)m_allParticles.size() - 1;
  if (smokeIndex < 0) smokeIndex = 0;

   // remove previous smoke system
   if (m_smokeID != -1)
     gParticle.killSystem(m_smokeID);
   if (m_allParticles[smokeIndex] != "")
   {
     m_smokeID = gParticle.createSystem(m_allParticles[smokeIndex]);
	 //need to change m_enginePosition to silo
     gParticle.setSystemPos(m_smokeID, transformObjectToInertial(m_enginePosition));
   }
}
*/