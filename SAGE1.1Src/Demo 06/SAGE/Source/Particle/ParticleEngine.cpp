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

/// \file ParticleEngine.cpp
/// \brief Code for the ParticleEngine class.

#include "ParticleEngine.h"
#include "ParticleSystem.h"
#include "ParticleDefines.h"
#include "Particle.h"
#include "tinyxml/tinyxml.h"
#include "directorymanager/directorymanager.h"
#include <d3dx9.h>
#include "common/Renderer.h"
#include "common/CommonStuff.h"
#include <list>

ParticleEngine gParticle;

/// \brief Handles comparing two particle systems to find which is closer
/// to the camera.
struct myCompare
{
  typedef ParticleSystem* P;

  /// \param x First particle system.
  /// \param y Second particle system.
  /// \return True is x is closer than y, false otherwise.
  /// \remark In case of a tie, the one with the smaller ID number is 'closer'.
  bool operator()(const P& x, const P& y) const
  {
    float xdist = Vector3::distanceSquared(gRenderer.getCameraPos(), x->getPosition());
    float ydist = Vector3::distanceSquared(gRenderer.getCameraPos(), y->getPosition());

    if(xdist < ydist)
      return true;
    else if(xdist == ydist)
      return (x->getUID() < y->getUID());
    else
      return false;
  }
};

typedef std::set<ParticleSystem*, myCompare> SystemSortedSet;
typedef SystemSortedSet::reverse_iterator SystemSortedSetIter;

ParticleEngine::ParticleEngine()
{
  m_xmlDoc = NULL;
  m_xmlDefs = NULL;

  srand(GetTickCount());
}

ParticleEngine::~ParticleEngine()
{
  shutdown();
}

/// \param defFile Name of the file containing the system definitions (xml)
void ParticleEngine::init(std::string defFile)
{
  // by resetting everything first, we allow for "hot swapping" the definition
  // file. not useful in a game, but could be useful in a particle system editor
  clear();

  if(m_xmlDoc != NULL)
  {
    delete m_xmlDoc;
    m_xmlDoc = NULL;
    m_xmlDefs = NULL;
  }

  // load effect definition file
  gDirectoryManager.setDirectory(eDirectoryXML);
  m_xmlDoc = new TiXmlDocument(defFile.c_str());
  m_xmlDoc->LoadFile();

  m_xmlDefs = m_xmlDoc->FirstChildElement("definitions");
  TiXmlElement* systemDef = m_xmlDefs->FirstChildElement("system");

  int numSystemTypes = 0;

  while(systemDef != NULL)
  {
    std::vector<ParticleSystem*> systems;

    m_Systems.push_back(systems);
    SystemCatalogIter iter = m_Systems.end();
    iter--;
    m_TypeMap.insert(NameTypePair(systemDef->Attribute("name"), numSystemTypes));

    int numCopies;
    int numParticlesThisSystem = 0;
    systemDef->Attribute("numcopies", &numCopies);

    TiXmlElement* effect = systemDef->FirstChildElement("effect");

    if(effect == NULL)
      ABORT("Invalid file format found while initializing ParticleEngine: filename %s", defFile);

    // count the number of particles for this system type
    while(effect != NULL)
    {
      int partCount;
      effect->Attribute("particleCount", &partCount);
      numParticlesThisSystem += partCount;
      effect = effect->NextSiblingElement("effect");
    }

    for(int i=0; i<numCopies; i++)
    {
      ParticleSystem *sys = new ParticleSystem();
      iter->push_back(sys);
      sys->init(systemDef);
    }

    systemDef = systemDef->NextSiblingElement("system");
    numSystemTypes++;
  }
}

void ParticleEngine::shutdown()
{
  clear(); // delete all systems

  if(m_xmlDoc != NULL)
  {
    delete m_xmlDoc;
    m_xmlDoc = NULL;
    m_xmlDefs = NULL;
  }

  assert(m_UIDMap.empty());
}


void ParticleEngine::updateSystems()
{
  UIDMapIter iter = m_UIDMap.begin();
  while(iter != m_UIDMap.end())
  {
    ParticleSystem *sys = iter->second;
    iter++;

    sys->update(gRenderer.getTimeStep());

    if(sys->isDead())
    {
      killSystem(sys->m_UID);
    }
  }
}


void ParticleEngine::clear()
{
  for(int i=0; i<(int)m_Systems.size(); i++)
  {
    for(int j=0; j<(int)m_Systems[i].size(); j++)
    {
      ParticleSystem *sys = m_Systems[i][j];
      m_UIDMap.erase(sys->m_UID);
      m_IDGenerator.releaseID(sys->m_UID);

      delete sys;
      m_Systems[i][j] = NULL;
    }
    m_Systems[i].clear();
  }
  m_Systems.clear();
}

/// Killing a particle system will stop the system from being rendered
/// \param uid ID of the system to kill
void ParticleEngine::killSystem(unsigned int uid)
{
  ParticleSystem *sys = getSystemFromUID(uid);

  if(sys != NULL)
    sys->reset();

  m_UIDMap.erase(uid);
  m_IDGenerator.releaseID(uid);
}

/// Kills all particle systems
void ParticleEngine::killAll()
{

  UIDMapIter iter = m_UIDMap.begin();
  while(iter != m_UIDMap.end())
  {
    ParticleSystem *sys = iter->second;
    sys->reset();     
    iter++;    
  }

  m_IDGenerator.clear();

}

/// Renders all the particle systems. Passing in false for doUpdate allows
/// you to render the systems multiple times per frame without updating. This
/// is useful when a shader requires multiple passes (such as water reflection)
/// \param doUpdate Whether the systems should be updated before rendering
void ParticleEngine::render(bool doUpdate)
{
  static SystemSortedSet sortedSystems;
  sortedSystems.clear();

  if(m_UIDMap.size() == 0)
    return;

  if(doUpdate)
    updateSystems();

  // render all systems
  for(UIDMapIter iter = m_UIDMap.begin(); iter != m_UIDMap.end(); iter++)
  {
    sortedSystems.insert(iter->second);
  }

  for(SystemSortedSetIter iter = sortedSystems.rbegin(); iter != sortedSystems.rend(); iter++)
  {
    (*iter)->render();
  }
}

/// \remark Reasons for getting an invalid handle include passing in a bad
/// effect name and trying to create more than the max number of systems.
/// \param effectName Name of the particle effect to create
/// \return Handle to the system being created, -1 if invalid
unsigned int ParticleEngine::createSystem(std::string effectName)
{
  SystemTypeMap::const_iterator iter = m_TypeMap.find(effectName);

  int catalogIndex = -1;

  if(iter == m_TypeMap.end())
    return -1;
  
  catalogIndex = iter->second;
  
  ParticleSystem *system = NULL;
  for(int i=0; i< (int)m_Systems[catalogIndex].size(); i++)
  {
    if(m_Systems[catalogIndex][i]->isDead())
    {
      system = m_Systems[catalogIndex][i];
      break;
    }
  }

  if(system == NULL)
    return -1;

  system->start();
  unsigned int uid = m_IDGenerator.generateID();
  system->m_UID = uid;
  m_UIDMap.insert(UIDIndexPair(uid, system));

  return uid; // this value will be used as the handle
}

/// \param uid ID of the system to move
/// \param pos Position of the system
void ParticleEngine::setSystemPos(unsigned int uid, Vector3 pos)
{
  ParticleSystem *system = getSystemFromUID(uid);

  // constant time lookup since id is the index of the system requested
  if(system != NULL)
  {
    system->setPosition(pos);
  }
}

/// \param uid ID of the system
/// \return The name of the system class
std::string ParticleEngine::getSystemName(unsigned int uid)
{
  ParticleSystem *system = getSystemFromUID(uid);

  if(system != NULL)
  {
    return system->getName();
  }

  // id was not valid, return a null string
  std::string retval = "";
  return retval;
}

/// \param numSys Address of an int to store the current number of systems
/// \param numPart Address of an int to store the current number of particles
/// in all systems
/// \return The number of particles in all systems
int ParticleEngine::getPerformanceData(int *numSys, int *numPart)
{
  if(numSys != NULL)
    *numSys = (int)m_UIDMap.size();

  // loop through all the systems and add up the total number of particles
  int parts = 0;
  
  for(UIDMapIter iter = m_UIDMap.begin(); iter != m_UIDMap.end(); iter++)
  {
    ParticleSystem *system = iter->second;
    parts += system->getParticleCount();
  }

  if(numPart != NULL)
    *numPart = parts;

  return parts;
}

ParticleSystem* ParticleEngine::getSystemFromUID(unsigned int uid)
{
  UIDMapIter iter;

  iter = m_UIDMap.find(uid);

  if(iter == m_UIDMap.end()) // we don't have that key in our map
    return NULL;
  else
    return iter->second;
}