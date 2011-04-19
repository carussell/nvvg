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

/// \file ParticleEffect.h
/// \brief Interface for the ParticleEffect class.

#ifndef __PARTICLEEFFECT_H_INCLUDED__
#define __PARTICLEEFFECT_H_INCLUDED__

#include <stdio.h>
#include <string>
#include <vector>
#include <hash_map>
#include <d3dx9.h>
#include "tinyxml/tinyxml.h"
#include "common/Vector3.h"

#include "graphics/VertexBuffer.h"
#include "graphics/IndexBuffer.h"
#include "graphics/VertexTypes.h"

class Particle;
class ParticleEngine;

//-----------------------------------------------------------------------------
/// \brief Describes a particle effect
/// \remark All the members and methods of this class are private, with the
/// ParticleSystem and ParticlePropertyMapper being the only friend classes
/// declared. 
class ParticleEffect
{
private:

  friend class ParticleSystem;
  friend class ParticlePropertyMapper;

  typedef VertexBuffer<RenderVertexL> VertexLBuffer; ///< Shorthand for a lit vertex buffer
  typedef Vector3 (*DistributionFunc)(); ///< Shorthand for a function that returns a vector
  typedef void (ParticleEffect::*UpdateFunc)(); ///< Shorthand for a function that updates the particles
  typedef void (ParticleEffect::*InitFunc)(Particle*); ///< Shorthand for a function that initializes the particles
  typedef std::vector<UpdateFunc> UpdateFuncArray;
  typedef UpdateFuncArray::const_iterator UpdateFuncIter;
  typedef std::vector<InitFunc> InitFuncArray;
  typedef InitFuncArray::const_iterator InitFuncIter;
  typedef bool (ParticleEffect::*PropertyFunc)(TiXmlElement*); ///< Shorthand for a ParticleEffect property function
  typedef std::pair <std::string, PropertyFunc> PropPair; ///< Shorthand for the hash map elements
  typedef stdext::hash_map<std::string, PropertyFunc> PropertyMap; ///< Shorthand for the hash map used by the property map class

  ParticleEffect(TiXmlElement *subSysDef); ///< Basic constructor
  ~ParticleEffect();                       ///< Basic destructor
  void render(); ///< Renders the particles

  //------------------------------------------------------------
  /// \brief Accessors
  //{@
  /// \brief Sets the origin of the effect
  /// \param pos Position of the effect's origin
  void setPosition(Vector3 pos) { m_vecPosition = pos; }

  /// \brief Gets whether the effect has completed
  /// \return True if the effect has completed, false otherwise
  bool isDead() { return m_bIsDead; }

  /// \brief Gets the number of live particles in the effect currently
  /// \return Returns the number of live particles in the effect
  int getParticleCount() { return m_nLiveParticleCount; }
  //}@
  //------------------------------------------------------------
  
  //------------------------------------------------------------
  /// \brief Effect properties
  //{@
  Particle *m_Particles; ///< Pointer to particle array
  int *m_drawOrder; ///< Array of indices determining the order to draw in
  int m_nTotalParticleCount; ///< Max number of particles in the system
  int m_nLiveParticleCount; ///< Number of particles that are currently live
  int m_nEmitRate; ///< Max number of particles to create per second
  float m_fElapsedTime; ///< Time in seconds since last update called
  float m_fEmitPartial; ///< Partial particle, stores the value until greater than 1
  bool m_sort; ///< Whether the system should sort the particles back to front
  bool m_bCycleParticles; ///< True if particles are to be reused after they die
  Vector3 m_vecPosition; ///< System position
  Vector3 m_vecGravity; ///< System gravity
  VertexLBuffer *m_vertBuffer; ///< Vertex buffer used to render particles
  IndexBuffer *m_indexBuffer; ///< Index buffer used to render particles
  bool m_bIsDead; ///< True when all the particles are dead and we aren't cycling
  bool m_IsDying; ///< True when all particles have been created
  int m_textureHandle; ///< Handle to particle texture
  UpdateFuncArray m_UpdateFunc;
  InitFuncArray m_InitFunc;

  /// \todo Remove DirectX texture interface from ParticleEffect, use renderer
  LPDIRECT3DTEXTURE9 m_txtParticleTexture; ///< DirectX texture interface
  //}@
  //------------------------------------------------------------

  //------------------------------------------------------------
  /// \brief Particle initializing values
  //{@
  float m_fPILife; ///< How long in seconds the particle will live
  float m_fPISpeed; ///< The magnitude of the particle's initial velocity
  float m_fPISize; ///< The size of the particle
  unsigned int m_cPIColor; ///< The color of the particle
  float m_fPIDragValue; ///< The amount of drag on the particle
  float m_PIFadeIn; ///< How long until the particle is at maximum alpha
  float m_PIFadeOut; ///< How long until the particle begins to fade to 0 alpha
  float m_PIFadeMax; ///< Maximum alpha
  float m_PIRotationSpeed; ///< Speed at which the particle rotates (in radians/sec)
  float m_PIRotationStopTime; ///< Time until rotation stops
  DistributionFunc m_distFunc; ///< Function that determines the initial direction
  //}@
  //------------------------------------------------------------

  //------------------------------------------------------------
  /// \brief Maintenance functions
  //{@
  void initIndexBuffer(); ///< Initializes the index buffer
  void initProperties(TiXmlElement *sysDef); ///< Initializes the effect values
  void initParticles(); ///< Gives all particles an initial default value
  void start(); ///< Prepares the effect for starting

  void birthParticles(); ///< Creates all particles ready to be "born"
  bool initParticle(int index); ///< Initializes the particle at the given index
  void initParticleRotation(Particle *particle);

  void killParticles(); ///< Kills all particles that are too old
  bool killParticle(int index); ///< Kills the particle at a particular index

  void update(float elapsedTime); ///< Updates the particles' values
  void updateFade(); ///< Updates the particles alpha values based on lifeleft
  void updateRotation(); ///< Updates the particles rotation values

  void sort(); ///< Sorts the particles from back to front
  //}@
  //------------------------------------------------------------

  //------------------------------------------------------------
  /// \brief Member accessors
  //{@
  /// \brief Sets the effect emit rate
  /// \param emitRate Rate at which the effect emits particles (per second)
  void setEmit(int emitRate) { m_nEmitRate = emitRate; }

  /// \brief Sets the effect diffuse color
  /// \param color Diffuse color of the effect particles
  void setColor(unsigned int color) { m_cPIColor = color; }
  //}@
  //------------------------------------------------------------

  //------------------------------------------------------------
  /// \brief XML effect initializing functions
  //{@
  bool setEmit(TiXmlElement *prop); ///< Sets the emit rate
  bool setSort(TiXmlElement *prop); ///< Sets whether to sort
  bool setGravity(TiXmlElement *prop); ///< Sets the value of gravity
  bool setCycle(TiXmlElement *prop); ///< Sets whether the effect cycles

  bool setParticleLife(TiXmlElement *prop); ///< Sets the particle's life
  bool setParticleSpeed(TiXmlElement *prop);///< Sets the particle's speed
  bool setParticleColor(TiXmlElement *prop);///< Sets the particle's color
  bool setParticleSize(TiXmlElement *prop); ///< Sets the particle's size
  bool setParticleDrag(TiXmlElement *prop); ///< Sets the particle's drag value
  bool setParticleFade(TiXmlElement *prop); ///< Sets the particle's fade values
  bool setParticleRotation(TiXmlElement *prop); ///< Sets the particle's rotation values
  //}@
  //------------------------------------------------------------
};
//-----------------------------------------------------------------------------

#endif

