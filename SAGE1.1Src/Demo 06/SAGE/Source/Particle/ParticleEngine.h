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

/// \file ParticleEngine.h
/// \brief Interface for the ParticleEngine class.

#ifndef __PARTICLEENGINE_H_INCLUDED__
#define __PARTICLEENGINE_H_INCLUDED__

#include <string>
#include <hash_map>
#include <vector>
#include <set>
#include "Particle.h"
#include "common/Vector3.h"
#include "generators/IDGenerator.h"

class ParticleSystem;
class TiXmlDocument;
class TiXmlElement;
struct IDirect3DDevice9;

//-----------------------------------------------------------------------------
/// \brief Creates, manages, and renders particle systems
///
/// The ParticleEngine uses xml files to create particle systems. The xml files
/// contain the system definitions, describing the attributes of the system.
class ParticleEngine
{
  typedef stdext::hash_map<unsigned int, ParticleSystem*> UIDMap;
  typedef UIDMap::const_iterator UIDMapIter;
  typedef std::pair<unsigned int, ParticleSystem*> UIDIndexPair;

  typedef std::vector<ParticleSystem*> SystemArray;
  typedef SystemArray::iterator SystemIter;
  typedef std::vector<SystemArray> SystemCatalog;
  typedef SystemCatalog::iterator SystemCatalogIter;

  typedef stdext::hash_map<std::string, int> SystemTypeMap;
  typedef std::pair<std::string, int> NameTypePair;

public:
  ParticleEngine(); ///< Basic constructor
  ~ParticleEngine(); ///< Basic destructor

  void init(std::string defFile); ///< Initializes the engine's data members
  void shutdown(); ///< Shutdowns the engine

  void clear(); ///< Kills all systems currently running
  void killSystem(unsigned int sysID); ///< Kills a specific system
  void killAll(); ///< Deletes all particle systems

  void render(bool doUpdate=true); ///< Renders all systems
  unsigned int createSystem(std::string effectName); ///< Create a new system

  void setSystemPos(unsigned int sysID, Vector3 pos); ///< Set a system's position

  std::string getSystemName(unsigned int sysID); ///< Get the definition name of a system

  /// \brief Gets the engine performance data
  int getPerformanceData(int *numSystems, int *numParticles);

private:
  SystemCatalog m_Systems; ///< Catalog of all systems possible
  SystemTypeMap m_TypeMap;

  IDGenerator m_IDGenerator; ///< ID generator for the systems
  UIDMap m_UIDMap; ///< Map of UID's to particle systems
  char* m_pEffectFile; ///< Name of the particle effect definition file
  TiXmlDocument* m_xmlDoc; ///< Xml document containing the effect definitions
  TiXmlElement* m_xmlDefs; ///< Definition node in the particle xml file
  unsigned int m_nLastTimeUpdated; ///< Time of last engine update

  void updateSystems(); ///< Updates all particle systems
  ParticleSystem* getSystemFromUID(unsigned int uid); ///< Finds the index mapped to the uid
};
//-----------------------------------------------------------------------------

extern ParticleEngine gParticle; ///< Global particle engine instance

#endif