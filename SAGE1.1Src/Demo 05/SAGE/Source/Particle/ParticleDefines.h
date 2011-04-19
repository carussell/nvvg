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

/// \file ParticleDefines.h
/// \author Jeremy Nunn
/// \date July 18, 2005

#ifndef __JPARTDEFINES_H_INCLUDED__
#define __JPARTDEFINES_H_INCLUDED__

#include "common/Vector3.h"
#include <string>

/// \brief Describes a function to be used to get inital particle velocities
typedef Vector3(*DistributionFunc)();

/// \brief Enumerated particle distribution shapes
enum EmitDistributionType
{
  edtShellSphere, ///< Uniform distribution random vector on a sphere with equal magnitudes
  edtSolidSphere, ///< Uniform distribution random vector within a sphere
  edtRing,        ///< Uniform distribution random vector on a ring with equal magnitudes
  edtDisc,        ///< Uniform distribution random vector within a ring (disc)
  edtSolidCube    ///< Uniform distribution random vector within a cube
};

//-----------------------------------------------------------------------------
/// \brief Group of useful functions for the particle engine
class ParticleUtil
{
public:

  //------------------------------------------------------------
  /// \brief Returns a random value between 0 and 1
  /// \return A random value between 0 and 1
  static float randf() { return ((float)rand() / (float)RAND_MAX); }

  /// \brief Particle distribution functions
  //@{
  /// \brief Returns the distribution function pointer for the given string
  static DistributionFunc getEDTFunc(const char* edt);

  /// \brief Returns a uniform distribution random vector on a sphere
  static Vector3 getRandVecShellSphere();

  /// \brief Returns a uniform distribution random vector within a sphere
  static Vector3 getRandVecSolidSphere();

  /// \brief Returns a uniform distribution random vector on a ring
  static Vector3 getRandVecRing();

  /// \brief Returns a uniform distribution random vector within a ring (disc)
  static Vector3 getRandVecDisc();

  /// \brief Returns a uniform distribution random vector within a cube
  static Vector3 getRandVecSolidCube();
  //@}
  //------------------------------------------------------------
};
//-----------------------------------------------------------------------------

#endif