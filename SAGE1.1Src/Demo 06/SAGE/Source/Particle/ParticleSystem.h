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

/// \file ParticleSystem.h
/// \brief Interface for the ParticleSystem class.



#ifndef __PARTICLESYSTEM_H_INCLUDED__
#define __PARTICLESYSTEM_H_INCLUDED__

#include <stdio.h>
#include <string>
#include "common/Vector3.h"

#include "ParticleDefines.h"

class ParticleEffect;
class TiXmlElement;

//-----------------------------------------------------------------------------
/// \brief Collection of effects that make up a single system.
///
/// The purpose of this class is to manage the effects
/// \remark All the members and methods of this class are private, with the
/// ParticleEngine being the only friend class declared. 
class ParticleSystem
{
  friend class ParticleEngine;

public:
  Vector3 getPosition() { return m_Position; }
  unsigned int getUID() { return m_UID; }

private:
  ParticleSystem();  ///< Basic constructor
  ~ParticleSystem(); ///< Basic destructor

  void init(TiXmlElement *sysDef); ///< Initialize the system
  void clear(); ///< Clears the system data
  void reset(); ///< Resets the system to initialized state
  void start(); ///< Sets the effects to alive

  void update(float elapsedTime); ///< Updates the particles
  void render(); ///< Renders all effects in this system

  bool isDead(); ///< Tests whether the system is dead (no live particles)

  /// \brief Set the system position
  /// \param pos Position of the system
  void setPosition(Vector3 pos) { m_Position = pos; }

  /// \brief Returns the name of the system
  /// \return The name of the system class
  std::string getName() { return m_Name; }

  int getParticleCount(); ///< Returns the number of particles in all effects

  ParticleEffect **m_Effect; ///< Pointer to the effects
  unsigned int m_UID; ///< Unique handle to this system
  int m_NumEffects; ///< Number of effects
  Vector3 m_Position; ///< Position of the system
  std::string m_Name; ///< Name of the system
  TiXmlElement *m_SystemDef; ///< XML tag containing the system definition
};
//-----------------------------------------------------------------------------

#endif