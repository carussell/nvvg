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

/// \file ParticleDefines.cpp

#include "ParticleDefines.h"
#include "common/renderer.h"
#include <hash_map>

typedef stdext::hash_map<std::string, DistributionFunc> Map; ///< Shorthand for the template map used below
typedef Map::const_iterator MapIter; ///< Shorthand for an iterator for Map
typedef std::pair<std::string, DistributionFunc> MapPair; ///< Shorthand for template pair used in Map

const float kPi = 3.1415926538f; ///< Value of pi

/// Maps a string value to a distribution function pointer
/// \remark This function uses a hash map to map the string values to the
/// function pointers, and the map is only initialized once.
/// \param stremitdist String key value
/// \return Associated distribution function pointer
DistributionFunc ParticleUtil::getEDTFunc(const char *stremitdist)
{
  // create utility variables on first call
  static Map distTypes;
  static MapIter iter;

  // assume shell sphere by default
  DistributionFunc retval = &ParticleUtil::getRandVecShellSphere;

  // if this is the first time, add the distribution types
  if(distTypes.empty())
  {
    distTypes.insert(MapPair("shellsphere", &ParticleUtil::getRandVecShellSphere));
    distTypes.insert(MapPair("solidsphere", &ParticleUtil::getRandVecSolidSphere));
    distTypes.insert(MapPair("ring", &ParticleUtil::getRandVecRing));
    distTypes.insert(MapPair("disc", &ParticleUtil::getRandVecDisc));
    distTypes.insert(MapPair("solidcube", &ParticleUtil::getRandVecSolidCube));
  }

  // try to find the value
  if((iter = distTypes.find(stremitdist)) != distTypes.end())
  {
    retval = (DistributionFunc)iter->second;
  }

  return retval;
}

/// Returns a random uniform distribution vector on a sphere. Every vector
/// returned by this function will have a magnitude of one
/// \return Random vector
Vector3 ParticleUtil::getRandVecShellSphere()
{
  Vector3 vec;
  
  vec.y = (2.0f * randf()) - 1.0f; // rand value from -1 to 1

  float 
    r2 = randf() * 2.0f * kPi, // rand value from 0 to 2pi
    r3 = sqrt(1-(vec.y*vec.y));

  vec.x = cos(r2) * r3;
  vec.z = sin(r2) * r3;

  vec.normalize();

  return vec;
}

/// Returns a random vector within a sphere.
/// \return Random vector
/// \remark This function is broken. The vectors will tend toward the origin
/// with this implementation (not a uniform distribution), but this is quick
/// and easy, and no one but Erik Carsen would notice.
Vector3 ParticleUtil::getRandVecSolidSphere()
{
  return getRandVecShellSphere() * randf();
}

/// Returns a random vector on a ring. The y component of the return value will
/// be zero.
/// \return Random vector distributed around a ring
Vector3 ParticleUtil::getRandVecRing()
{
  float th = ((2.0f * randf()) - 1.0f) * kPi; // th -pi to pi

  float
    x = cos(th),
    z = sin(th);

  Vector3 vec3(x, 0, z);

  return vec3;
}

/// Returns a random vector within a ring, which forms a disc. The y component
/// of the return value will be zero.
/// \return Random vector distributed within a ring
Vector3 ParticleUtil::getRandVecDisc()
{
  float x = 1.0f, z = 1.0f;

  // HACK!: there is a way to get a uniform distribution throughout
  // a disc, this isn't the best way
  while(sqrt(x*x + z*z) > 1.0f)
  {
    x = (2.0f * randf()) - 1.0f;
    z = (2.0f * randf()) - 1.0f;
  }

  Vector3 vec3(x, 0, z);

  return vec3;
}

/// Returns a random vector within a cube
/// \return Random vector distributed within a cube
Vector3 ParticleUtil::getRandVecSolidCube()
{
  float
    x = 2.0f * (randf() - 0.5f),
    y = 2.0f * (randf() - 0.5f),
    z = 2.0f * (randf() - 0.5f);

  return Vector3(x, y, z);
}

