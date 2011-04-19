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

#include <algorithm>
#include "Common/MathUtil.h"
#include "Common/RotationMatrix.h"
#include "Graphics/ModelManager.h"
#include "Objects/GameObject.h"
#include "Terrain/Terrain.h"
#include "Water/Water.h"
#include "Ned3DObjectManager.h"
#include "Objects.h"
#include "ObjectTypes.h"
#include "DirectoryManager/DirectoryManager.h"

PlaneObject* gPlane;

Ned3DObjectManager::Ned3DObjectManager() :
  m_models(NULL),
  m_planeModel(NULL),
  m_crowModel(NULL),
  m_siloModel(NULL),
  m_windmillModel(NULL),
  m_plane(NULL),
  m_terrain(NULL),
  m_water(NULL)
{
}

void Ned3DObjectManager::setModelManager(ModelManager &models)
{
  if(m_models == &models)
    return;
  m_models = &models;
  m_planeModel = NULL;
  m_crowModel = NULL;
  m_siloModel = NULL;
  m_windmillModel = NULL;
}

void Ned3DObjectManager::clear()
{
  m_planeModel = NULL;
  m_crowModel = NULL;
  m_siloModel = NULL;
  m_windmillModel = NULL;
  m_plane = NULL;
  m_terrain = NULL;
  m_water = NULL;
  m_crows.clear();
  m_furniture.clear();
  GameObjectManager::clear();
}

unsigned int Ned3DObjectManager::spawnPlane(const Vector3 &position, const EulerAngles &orientation)
{
  if(m_plane != NULL)
    return 0;  // Only one plane allowed
  if(m_planeModel == NULL)
    m_planeModel = m_models->getModelPointer("Plane"); // Cache plane model
  if(m_planeModel == NULL)
    return 0;  // Still NULL?  No such model
  m_plane = new PlaneObject(m_planeModel);
  m_plane->setPosition(position);
  m_plane->setOrientation(orientation);
  unsigned int id = addObject(m_plane, "Plane");
  return id;
}

unsigned int Ned3DObjectManager::spawnCrow(const Vector3 &position, const EulerAngles &orientation, float speed)
{
  if(m_crowModel == NULL)
    m_crowModel = m_models->getModelPointer("Crow"); // Cache crow model
  if(m_crowModel == NULL)
    return 0;  // Still NULL?  No such model
  CrowObject *crow = new CrowObject(m_crowModel);
  crow->setSpeed(speed);
  crow->setPosition(position);
  crow->setOrientation(orientation);
  crow->setMovementPattern(CrowObject::MP_STRAIGHT);
  unsigned int id = addObject(crow);
  m_crows.insert(crow);
  return id;
}

unsigned int Ned3DObjectManager::spawnCrow(const Vector3 &position, const Vector3 &circleCenter, float speed, bool flyLeft)
{
  if(m_crowModel == NULL)
    m_crowModel = m_models->getModelPointer("Crow"); // Cache crow model
  if(m_crowModel == NULL)
    return 0;  // Still NULL?  No such model
  CrowObject *crow = new CrowObject(m_crowModel);
  crow->setSpeed(speed);
  crow->setPosition(position);
  crow->setCirclingParameters(circleCenter, flyLeft);
  crow->setMovementPattern(CrowObject::MP_CIRCLING);
  unsigned int id = addObject(crow);
  m_crows.insert(crow);
  return id;
}

unsigned int Ned3DObjectManager::spawnTerrain(Terrain *terrain)
{
  m_terrain = new TerrainObject(terrain);
  return addObject(m_terrain, false, false, false, "Terrain");
}

unsigned int Ned3DObjectManager::spawnWater(Water *water)
{
  m_water = new WaterObject(water);
  return addObject(m_water, false, false, false, "Water");
}

unsigned int Ned3DObjectManager::spawnSilo(const Vector3 &position, const EulerAngles &orientation)
{
  static const std::string silos[] = {"Silo1","Silo2","Silo3","Silo4"};
  static int whichSilo = 0;
  m_siloModel = m_models->getModelPointer(silos[whichSilo]); // Cache silo model
  if(m_siloModel == NULL)
    return 0;  // Still NULL?  No such model
  SiloObject *silo = new SiloObject(m_siloModel);
  silo->setPosition(position);
  silo->setOrientation(orientation);
  unsigned int id = addObject(silo);
  m_furniture.insert(silo);
  whichSilo = ++whichSilo % 4;
  return id;
}

unsigned int Ned3DObjectManager::spawnWindmill(const Vector3 &position, const EulerAngles &orientation)
{
  if(m_windmillModel == NULL)
    m_windmillModel = m_models->getModelPointer("Windmill"); // Cache windmill model
  if(m_windmillModel == NULL)
    return 0;  // Still NULL?  No such model
  WindmillObject *windmill = new WindmillObject(m_windmillModel);
  windmill->setPosition(position);
  windmill->setPosition(Vector3(0,27.0f,-0.5f),1);
  windmill->setRotationSpeedBank(kPiOver2,1);
  windmill->setOrientation(orientation);
  unsigned int id = addObject(windmill);
  m_furniture.insert(windmill);
  return id;
}

// Returns a handle to the first crow in the list
unsigned int Ned3DObjectManager::getCrow()
{
  ObjectSetIter cit = m_crows.begin();
  if (cit == m_crows.end())
    return - 1;

  return (*cit)->getID();
}

void Ned3DObjectManager::deleteObject(GameObject *object)
{
  if(object == m_plane)
    m_plane = NULL;
  else if(object == m_terrain)
    m_terrain = NULL;
  else if(object == m_water)
    m_water = NULL;
  m_crows.erase(object);
  m_furniture.erase(object);
  GameObjectManager::deleteObject(object);
}