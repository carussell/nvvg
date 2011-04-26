/*
 * ExplodingSiloObject.  Adapted from SiloObject.  Original license below.
 *
 *--o0o=================================================================o0o----
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
 *--o0o=================================================================o0o--*/

#include "ExplodingSiloObject.h"
#include "Particle/ParticleEngine.h"

ExplodingSiloObject::ExplodingSiloObject(Model *m)
  : SiloObject(m),
  m_enginePosition(0,10.0f, 10.0f),//should be renamed to top and position changed
  m_topPosition(0, 10.0f, 9.0f)
{
  this->m_className = "ExplodingSilo";
  this->m_type = ObjectTypes::EXPLODINGSILO;
  this->m_isExplodingSiloDead = false;
  this->m_smokeID = -1;
  this->m_allParticles.resize(2);
  this->m_allParticles[0] = "planeexplosion";   // when hp = 0
  this->m_allParticles[1] = "";             // hp > 2
  // load all textures that will be used on the plane.
  // Multiple textures are used throughout game play so the silos appear to
  // "take damage"
  this->m_allTextures.resize(2);
  this->m_allTextures[0] = "deadecylo.tga"; // when dead
  //this->m_allTextures[0] = "ecylo4.tga"; // when dead
  this->m_allTextures[1] = "ecylo4.tga"; // when alive
  // cache all these
  for (int a =0; a < (int)m_allTextures.size(); a++)
    gRenderer.cacheTextureDX(m_allTextures[a].c_str());


  //JULIA added for Smokin' Silos
  //m_allParticles.resize(2);
 // m_allParticles[0] = "smokeveryheavy";   // when hp = 0
  //m_allParticles[1] = "smokeheavy";   // when hp = 0
  
}

/*
//old
void ExplodingSiloObject::kill(void)
{
}*/
void ExplodingSiloObject::kill(void)
{
	this->m_isExplodingSiloDead = true;
	smoke();
}
 void ExplodingSiloObject::smoke(void)
 {
	 this->m_smokeID = 0;
	setTextureAndSmoke();
 }
ExplodingSiloObject::~ExplodingSiloObject()
{
  // kill particle engine if one is attached
  if (m_smokeID != -1)
	  gParticle.killSystem(m_smokeID);
  m_smokeID = -1;

}
//unlike plane's smoke, this will be called on death and 
// will make smoke appear
void ExplodingSiloObject::setTextureAndSmoke()
{
   
  int smokeIndex = 0; // index into m_allParticles array
  int textureIndex = 0;// index into m_allTextures array
      // set texture 
  int numParts = m_pModel->getPartCount();
  for (int a = 0; a < numParts; a++)
    m_pModel->setPartTextureName(a,m_allTextures[textureIndex].c_str());
   m_pModel->cache();

   // remove previous smoke system
   if (m_smokeID != -1)
     gParticle.killSystem(m_smokeID);
   if (m_allParticles[smokeIndex] != "")
   {
     m_smokeID = gParticle.createSystem(m_allParticles[smokeIndex]);
	 //need to change m_enginePosition to silo
     //smokeEngine.setSystemPos(m_smokeID, transformObjectToInertial(m_enginePosition));
	 gParticle.setSystemPos(m_smokeID, transformObjectToInertial(m_topPosition));
   }
}

