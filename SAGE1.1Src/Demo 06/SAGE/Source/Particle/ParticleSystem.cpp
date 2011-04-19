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

/// \file ParticleSystem.cpp
/// \brief Code for the ParticleSystem class.

#include "ParticleSystem.h"
#include "ParticleEngine.h"
#include "ParticleEffect.h"
#include "ParticleDefines.h"
#include "tinyxml/tinyxml.h"
#include "common/Renderer.h"

ParticleSystem::ParticleSystem()
{
  // initialize members
  m_Effect = NULL;
  m_NumEffects = 0;
  m_Position = Vector3::kZeroVector;
  m_Name = "";
}

ParticleSystem::~ParticleSystem()
{
  clear();
}

/// \param sysDef XML tag containing the system definition
void ParticleSystem::init(TiXmlElement *sysDef)
{
  clear();
  m_SystemDef = sysDef;

  m_Name = sysDef->Attribute("name");

  // determine the number of effects by counting the xml tags
  TiXmlElement *effect = sysDef->FirstChildElement("effect");

  while(effect != 0)
  {
    ++m_NumEffects;
    effect = effect->NextSiblingElement("effect");
  }

  // create array of effect pointers
  m_Effect = new ParticleEffect*[m_NumEffects];

  // create effects
  effect = sysDef->FirstChildElement("effect");
  int effectParticleOffset = 0;
  for(int i=0; i<m_NumEffects; i++)
  {
    int partCount;
    effect->Attribute("particlecount", &partCount);

    m_Effect[i] = new ParticleEffect(effect);
    effectParticleOffset += partCount;
    m_Effect[i]->m_bIsDead = true;

    effect = effect->NextSiblingElement("effect");
  }
}

void ParticleSystem::clear()
{
  // delete the effects
  for(int i=0; i<m_NumEffects; i++)
  {
    if(m_Effect[i] != 0)
    {
      delete m_Effect[i];
      m_Effect[i] = 0;
    }
  }

  // delete the array of effect pointers
  delete[] m_Effect;

  m_Effect = NULL;
  m_NumEffects = 0;
  m_Position = Vector3::kZeroVector;
  m_Name = "";
}

void ParticleSystem::reset()
{
  for(int i=0; i<m_NumEffects; i++)
    m_Effect[i]->m_bIsDead = true;
}

void ParticleSystem::start()
{
  for(int i=0; i<m_NumEffects; i++)
  {
    m_Effect[i]->start();
  }
}

/// \param elapsedTime Time in seconds since the last update call was made
void ParticleSystem::update(float elapsedTime)
{
  for(int i=0; i<m_NumEffects; i++)
  {
    m_Effect[i]->setPosition(m_Position);
    m_Effect[i]->update(elapsedTime);
  }
}

void ParticleSystem::render()
{
  for(int i=0; i<m_NumEffects; i++)
  {
    m_Effect[i]->render();
  }
}

/// \return True if all effects are dead, false otherwise
bool ParticleSystem::isDead()
{
  for(int i=0; i<m_NumEffects; i++)
  {
    if(!m_Effect[i]->isDead())
    {
      return false;
    }
  }

  return true;
}

/// \return The number of particles in the system
int ParticleSystem::getParticleCount()
{
  int retval = 0;

  for(int i=0; i<m_NumEffects; i++)
  {
    retval += m_Effect[i]->getParticleCount();
  }

  return retval;
}

