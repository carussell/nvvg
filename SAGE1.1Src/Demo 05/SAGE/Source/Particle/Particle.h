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

/// \file Particle.h
/// \author Jeremy Nunn
/// \date July 18, 2005

#ifndef __PARTICLE_H_INCLUDED__
#define __PARTICLE_H_INCLUDED__

#include "common/Vector3.h"

//-----------------------------------------------------------------------------
/// \brief Particle information used by ParticleEffect
///
/// Represents a single particle
class Particle
{
  friend class ParticleEffect;

public:
  Vector3 position; ///< Position of the particle
  Vector3 velocity; ///< Velocity of the particle
  float drag; ///< Rate at which velocity slows
  float size; ///< Size of the particle
  unsigned int color; ///< Color value of the particle
  float lifeleft; ///< Time in seconds until the particle dies
  bool birthed; ///< Whether this particle has been created (used)

  float rotation; ///< Current rotation of the particle
  float rotationSpeed; ///< Speed at which the particle rotates (in radians/sec)
  float rotationStopTime; ///< Time until rotation stops

  float uLeft; ///< Texture coords of the particle (top left)
  float uRight; ///< Texture coords of the particle (top right)
  float vTop; ///< Texture coords of the particle (bottom left)
  float vBottom; ///< Texture coords of the particle (bottom right)

  float distance; ///< Used by the effect class for sorting
};
//-----------------------------------------------------------------------------

#endif