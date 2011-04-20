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
#include "Particle/ParticleEngine.h"
#include "Terrain/Terrain.h"
#include "Water/Water.h"
#include "Ned3DObjectManager.h"
#include "Objects.h"
#include "ObjectTypes.h"
#include "DirectoryManager/DirectoryManager.h"
#include "ExplodingSiloObject.h"
#include "GhostSiloObject.h"
#include "BuzzedSiloObject.h"

PlaneObject* gPlane;

Ned3DObjectManager::Ned3DObjectManager() :
  m_models(NULL),
  m_planeModel(NULL),
  m_crowModel(NULL),
  m_bulletModel(NULL),
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
  m_bulletModel = NULL;
  m_siloModel = NULL;
  m_windmillModel = NULL;
}

void Ned3DObjectManager::clear()
{
  m_planeModel = NULL;
  m_crowModel = NULL;
  m_bulletModel = NULL;
  m_siloModel = NULL;
  m_windmillModel = NULL;
  m_plane = NULL;
  m_terrain = NULL;
  m_water = NULL;
  m_crows.clear();
  m_bullets.clear();
  m_furniture.clear();
  m_explodingSilos.clear();
  m_buzzedSilos.clear();
  m_ghostSilos.clear();
  GameObjectManager::clear();
}

void Ned3DObjectManager::handleInteractions()
{
  for(ObjectSetIter fit = m_furniture.begin(); fit != m_furniture.end(); ++fit)
    interactPlaneFurniture(*m_plane, **fit);
  for(ObjectSetIter bit = m_bullets.begin(); bit != m_bullets.end(); ++bit)
  {
    BulletObject &bullet = (BulletObject &)**bit;
    if(!bullet.isAlive()) continue;
      
    // Check for bullets hitting stuff
    for(ObjectSetIter cit = m_crows.begin(); cit != m_crows.end(); ++cit)
    {
      CrowObject &crow = (CrowObject &)**cit;
      if(!crow.isAlive()) continue;
      interactCrowBullet(crow, bullet);
    }
	bool noSilo = true;
	for(ObjectSetIter esit = m_explodingSilos.begin(); esit != m_explodingSilos.end(); ++esit){
		ExplodingSiloObject &silo = (ExplodingSiloObject &)**esit;
		interactBulletExplodingSilo(silo, bullet);
		noSilo = false;
	}
    GameObject *victim = bullet.getVictim();
    if(noSilo && victim != NULL)
    {
      // Bullet hit something
      switch(victim->getType())
      {
        case ObjectTypes::CROW :
        {
          shootCrow((CrowObject &)*victim);
        } break;
      }
    }
  }
  
  for(ObjectSetIter esit = m_explodingSilos.begin(); esit != m_explodingSilos.end(); ++esit){
	  ExplodingSiloObject &silo = (ExplodingSiloObject &)**esit;
	  interactPlaneExplodingSilo(*m_plane, silo);
  }
  for(ObjectSetIter bsit = m_buzzedSilos.begin(); bsit != m_buzzedSilos.end(); ++bsit){
	  BuzzedSiloObject &silo = (BuzzedSiloObject &)**bsit;
      interactPlaneBuzzedSilo(*m_plane, silo);
  }  
  for(ObjectSetIter gsit = m_ghostSilos.begin(); gsit != m_ghostSilos.end(); ++gsit){
	  GhostSiloObject &silo = (GhostSiloObject &)**gsit;
      interactPlaneGhostSilo(*m_plane, silo);
  }
  // Handle crow-crow interactions (slow....) and crow-plane interactions
  
  for(ObjectSetIter cit1 = m_crows.begin(); cit1 != m_crows.end(); ++cit1)
  {
    CrowObject &crow1 = (CrowObject &)**cit1;
    if(!crow1.isAlive()) continue;
    
    interactPlaneCrow(*m_plane,crow1);
    interactCrowTerrain(crow1,*m_terrain);
    
    ObjectSetIter cit2 = cit1;
    for(++cit2; cit2 != m_crows.end(); ++cit2)
    {
      CrowObject &crow2 = (CrowObject &)**cit2;
      if(!crow2.isAlive()) continue;
      interactCrowCrow(crow1, crow2);
    }
  }

  // Handle plane crashes
  
  interactPlaneTerrain(*m_plane, *m_terrain);
  interactPlaneWater(*m_plane, *m_water);
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

unsigned int Ned3DObjectManager::spawnBullet(const Vector3 &position, const EulerAngles &orientation)
{
  BulletObject *bullet = new BulletObject();
  bullet->setPosition(position);
  bullet->setOrientation(orientation);
  unsigned int id = addObject(bullet);
  m_bullets.insert(bullet);
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
  static const std::string silos[] = {"eSilo1","eSilo2","eSilo3","eSilo4", "gSilo1", "gSilo2", 
										"gSilo3", "gSilo4", "bSilo1", "bSilo2"};
  unsigned int id = -1;
  static int whichSilo = 0;
  m_siloModel = m_models->getModelPointer(silos[whichSilo]); // Cache silo model
  if(m_siloModel == NULL)
    return 0;  // Still NULL?  No such model

  if (whichSilo < 4){
	ExplodingSiloObject *silo = new ExplodingSiloObject(m_siloModel);
	silo->setPosition(position);
	silo->setOrientation(orientation);
	id = addObject(silo);
	m_explodingSilos.insert(silo);
  }
  else if (whichSilo < 8){
	GhostSiloObject *silo = new GhostSiloObject(m_siloModel);
	silo->setPosition(position);
	silo->setOrientation(orientation);
	id = addObject(silo);
	m_ghostSilos.insert(silo);
  }
  else{
	BuzzedSiloObject *silo = new BuzzedSiloObject(m_siloModel);
	silo->setPosition(position);
	silo->setOrientation(orientation);
	id = addObject(silo);
	m_buzzedSilos.insert(silo);
  }
  whichSilo = ++whichSilo % 10;
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

// Returns true if a crow intersects the ray
bool Ned3DObjectManager::rayIntersectCrow(const Vector3 &position, const Vector3 direction)
{
  
  // Check for bullets hitting crows
  for(ObjectSetIter cit = m_crows.begin(); cit != m_crows.end(); ++cit)
  {
    CrowObject &crow = (CrowObject &)**cit;
    if(!crow.isAlive()) continue;
    
    float t = crow.getBoundingBox().rayIntersect(position,direction);
    if(t <= 1.0f) return true;        
  }

  return false;
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
  m_bullets.erase(object);
  m_furniture.erase(object);
  GameObjectManager::deleteObject(object);
}
/** NEW STUFF **/
bool Ned3DObjectManager::interactPlaneBuzzedSilo(PlaneObject &plane, BuzzedSiloObject &buzzedSilo)
{
	bool collided = enforcePositions(plane, buzzedSilo);
	bool buzzed = false;
	if (collided) {
		plane.killPlane();
		//mark silo
	}
	else {
		buzzed = buzzedObject(plane, buzzedSilo);
		if (buzzed){
			//mark silo
		}
	}
	return (collided || buzzed);
}

/** NEW STUFF **/
bool Ned3DObjectManager::interactPlaneGhostSilo(PlaneObject &plane, GhostSiloObject &ghostSilo)
{
	bool collided = enforcePositions(plane, ghostSilo);
	if (collided) {
		plane.killPlane();
	}
	return collided;
}

/** NEW STUFF **/
bool Ned3DObjectManager::interactPlaneExplodingSilo(PlaneObject &plane, ExplodingSiloObject &explodingSilo)
{
	bool collided = enforcePositions(plane, explodingSilo);
	if (collided) {
		plane.killPlane();
		//kill silo
	}
	return collided;
}

/** NEW STUFF **/
bool Ned3DObjectManager::interactBulletExplodingSilo(ExplodingSiloObject &explodingSilo, BulletObject &bullet)
{
	bool collided = bullet.checkForBoundingBoxCollision(&explodingSilo);
	if (collided){
		//;
	}
	return collided;
}		

bool Ned3DObjectManager::interactPlaneCrow(PlaneObject &plane, CrowObject &crow)
{
  bool collided = enforcePositions(plane, crow);
  if(collided && !crow.isDying())
  {
    shootCrow(crow);
    plane.damage(1);
  }
  return collided;
}

bool Ned3DObjectManager::interactPlaneTerrain(PlaneObject &plane, TerrainObject &terrain)
{
  Terrain *terr = terrain.getTerrain();
  if(terr == NULL) return false;

  //test for plane collision with terrain
  Vector3 planePos = plane.getPosition();
  EulerAngles planeOrient = plane.getOrientation();
  Vector3 disp = planePos - disp;
  RotationMatrix planeMatrix;
  planeMatrix.setup(plane.getOrientation()); // get plane's orientation

  float planeBottom = plane.getBoundingBox().min.y;
  float terrainHeight = terr->getHeight(planePos.x,planePos.z);
  if(plane.isPlaneAlive() && planeBottom < terrainHeight)
  { //collision
    Vector3 viewVector = planeMatrix.objectToInertial(Vector3(0,0,1));
    if(viewVector * terr->getNormal(planePos.x,planePos.z) < -0.5f // dot product
      || plane.isCrashing())
    { 
      plane.killPlane();
      int partHndl = gParticle.createSystem("planeexplosion");
      gParticle.setSystemPos(partHndl, plane.getPosition());
      plane.setSpeed(0.0f);
      planePos += 2.0f * viewVector;
      planeOrient.pitch = kPi / 4.0f;
      planeOrient.bank = kPi / 4.0f;
      plane.setOrientation(planeOrient);
    }
    else planePos.y = terrainHeight + planePos.y - planeBottom;
    plane.setPosition(planePos);
    return true;
  }
  return false;
}

bool Ned3DObjectManager::interactPlaneWater(PlaneObject &plane, WaterObject &water)
{
  Water *pWater = water.getWater();
  if(pWater == NULL) return false;
  
  // Test for plane collision with water
  
  Vector3 planePos = plane.getPosition();
  EulerAngles planeOrient = plane.getOrientation();
  Vector3 disp = planePos - disp;
  RotationMatrix planeMatrix;
  planeMatrix.setup(plane.getOrientation()); // get plane's orientation
  float planeBottom = plane.getBoundingBox().min.y;
  float waterHeight = pWater->getWaterHeight();
  
  if(plane.isPlaneAlive() && planeBottom < waterHeight)
  { //collision
    Vector3 viewVector = planeMatrix.objectToInertial(Vector3(0,0,1));
    plane.killPlane();
    plane.setSpeed(0.0f);
    planePos += 2.0f * viewVector;
    planeOrient.pitch = kPi / 4.0f;
    planeOrient.bank = kPi / 4.0f;
    plane.setOrientation(planeOrient);
    plane.setPosition(planePos);
    
    int partHndl = gParticle.createSystem("planeexplosion");
    gParticle.setSystemPos(partHndl, plane.getPosition());
    return true;
  }
  return false;
}

bool Ned3DObjectManager::interactPlaneFurniture(PlaneObject &plane, GameObject &silo)
{
  return enforcePosition(plane, silo);
}

bool Ned3DObjectManager::interactCrowBullet(CrowObject &crow, BulletObject &bullet)
{
  return bullet.checkForBoundingBoxCollision(&crow);
}

bool Ned3DObjectManager::interactCrowCrow(CrowObject &crow1, CrowObject &crow2)
{
  return enforcePositions(crow1, crow2);
}


bool Ned3DObjectManager::interactCrowTerrain(CrowObject &crow, TerrainObject &terrain)
{
  Terrain *terr = terrain.getTerrain();
  if(terr == NULL) return false;

  //test for crow collision with terrain
  Vector3 crowPos = crow.getPosition();
    
  float terrainHeight = terr->getHeight(crowPos.x,crowPos.z);
  if (crowPos.y < terrainHeight)
  {
    crowPos.y = terrainHeight;
    crow.setPosition(crowPos);       
    int tmpHndl = gParticle.createSystem("crowfeatherssplat");
    gParticle.setSystemPos(tmpHndl, crowPos);
    crow.killObject();

    return true;
  }
  return false;
}

void Ned3DObjectManager::shootCrow(CrowObject &crow)
{
  int tmpHndl = gParticle.createSystem("crowfeathers");
  Vector3 crowPos = crow.getPosition();
  gParticle.setSystemPos(tmpHndl, crowPos);
  crow.setDying();
}

bool Ned3DObjectManager::buzzedObject(GameObject &moving, GameObject &stationary)
{
  const AABB3 &box1 = moving.getBoundingBox(), &box2 = stationary.getBoundingBox();
  AABB3 intersectBox;
  AABB3 otherBox;
  float minZ = box2.max.z;
  float height = minZ - box1.min.z;
  const Vector3 obj1Pos = Vector3(box2.min.x, box2.min.y, minZ);
  const Vector3 obj2Pos = Vector3(box2.max.x, box2.max.y, minZ+height);
  intersectBox.min.set(obj1Pos);
  intersectBox.min.set(obj2Pos);
  if(AABB3::intersect(box1, intersectBox, &otherBox))
	  return true;
  return false;
}

bool Ned3DObjectManager::enforcePosition(GameObject &moving, GameObject &stationary)
{
  const AABB3 &box1 = moving.getBoundingBox(), &box2 = stationary.getBoundingBox();
  AABB3 intersectBox;
  if(AABB3::intersect(box1, box2, &intersectBox))
  {
    // Collision:  Knock back obj1
    //   - Kludge method:  Push back on smallest dimension
    Vector3 delta = intersectBox.size();
    Vector3 obj1Pos = moving.getPosition(), obj2Pos = stationary.getPosition();
    if(delta.x <= delta.y)
      if(delta.x <= delta.z)
      {
        // Push back on x
        obj1Pos.x += (box1.min.x < box2.min.x) ? -delta.x : delta.x;
      }
      else
      {
        // Push back on z
        obj1Pos.z += (box1.min.z < box2.min.z) ? -delta.z : delta.z;
      }
    else if(delta.y <= delta.z)
    {
      // Push back on y
      obj1Pos.y += (box1.min.y < box2.min.y) ? -delta.y : delta.y;
    }
    else
    {
      // Push back on z
      obj1Pos.z += (box1.min.z < box2.min.z) ? -delta.z : delta.z;
    }
    moving.setPosition(obj1Pos);
    return true;
  }
  return false;
}

bool Ned3DObjectManager::enforcePositions(GameObject &obj1, GameObject &obj2)
{
  const AABB3 &box1 = obj1.getBoundingBox(), &box2 = obj2.getBoundingBox();
  AABB3 intersectBox;
  if(AABB3::intersect(box1, box2, &intersectBox))
  {
    // Collision:  Knock back both objects
    //   - Kludge method:  Push back on smallest dimension
    Vector3 delta = intersectBox.size();
    Vector3 obj1Pos = obj1.getPosition(), obj2Pos = obj2.getPosition();
    if(delta.x <= delta.y)
      if(delta.x <= delta.z)
      {
        // Push back on x
        float dx = (box1.min.x < box2.min.x) ? -delta.x : delta.x;
        obj1Pos.x += dx;
        obj2Pos.x -= dx;
      }
      else
      {
        // Push back on z
        float dz = (box1.min.z < box2.min.z) ? -delta.z : delta.z;
        obj1Pos.z += dz;
        obj2Pos.z -= dz;
      }
    else if(delta.y <= delta.z)
    {
        // Push back on y
        float dy = (box1.min.y < box2.min.y) ? -delta.y : delta.y;
        obj1Pos.y += dy;
        obj2Pos.y -= dy;
    }
    else
    {
      // Push back on z
      float dz = (box1.min.z < box2.min.z) ? -delta.z : delta.z;
      obj1Pos.z += dz;
      obj2Pos.z -= dz;
    }
    obj1.setPosition(obj1Pos);
    obj2.setPosition(obj2Pos);
    return true;
  }
  return false;
}