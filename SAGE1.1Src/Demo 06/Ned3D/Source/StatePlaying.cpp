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

/// \file StatePlaying.cpp
/// \brief Code for the StatePlaying class.

/// Edited by the SAGE team, 2005
/// Last updated June 13, 2006

#include "StatePlaying.h"
#include "game.h"
#include "input/input.h"
#include <algorithm>
#include "DirectoryManager/DirectoryManager.h"
#include "Common/MathUtil.h"
#include "Common/Renderer.h"
#include "Common/Random.h"
#include "Common/RotationMatrix.h"
#include "Console/Console.h"
#include "Graphics/ModelManager.h"
#include "Input/Input.h"
#include "Particle/ParticleEngine.h"
#include "Sound/SoundManager.h"
#include "Terrain/Terrain.h"
#include "WindowsWrapper/WindowsWrapper.h"
#include "Ned3DObjectManager.h"
#include "Game.h"

extern CRandom Random;

/// Global function that can be called from the console
bool StatePlaying::consoleSetFollowCamera(ParameterList* params,std::string* errorMessage)
{
  gGame.m_currentCam = gGame.m_statePlaying.m_tetherCamera;
	return true;
}

bool StatePlaying::consoleSetCameraTarget(ParameterList* params,std::string* errorMessage)
{
  unsigned int obj = gGame.m_statePlaying.m_objects->getObjectID(params->Strings[0]);
  if(obj == 0)
  {
    *errorMessage = "Object not found.";
    return false;
  }
  if(gGame.m_statePlaying.m_tetherCamera == NULL)
  {
    *errorMessage = "Tether camera not initialized.";
    return false;
  }
  gGame.m_statePlaying.m_tetherCamera->setTargetObject(obj);
  gGame.m_statePlaying.m_tetherCamera->reset();
  return true;
}

bool StatePlaying::consoleGodMode(ParameterList* params,std::string* errorMessage)
{
  PlaneObject::takeDamage = !params->Bools[0];
  return true;
}

StatePlaying::StatePlaying():
terrain(NULL),
water(NULL),
m_objects(NULL),
m_tetherCamera(NULL),
m_failedInstance(SoundManager::NOINSTANCE)
{

}

void StatePlaying::initiate()
{
  float farClippingPlane = 2000.0f;

  // Set far clipping plane
  gRenderer.setNearFarClippingPlanes(1.0f,farClippingPlane);    

  m_objects = new Ned3DObjectManager();	
	m_objects->setNumberOfDeadFrames(2);
	m_tetherCamera = new TetherCamera(m_objects);
	
  // Create terrain
  terrain = new Terrain(8,"terrain.xml"); //powers of two for terrain size
  m_objects->spawnTerrain(terrain);
  
  // Load models
  m_objects->setModelManager(gModelManager);
  gModelManager.importXml("models.xml");

  // Loads game objects like the crows, plane, and silo
	resetGame();
  
  // set fog
  gRenderer.setFogEnable(true);
  gRenderer.setFogDistance(farClippingPlane - 1000.0f,farClippingPlane);
  gRenderer.setFogColor(MAKE_ARGB(0,60,180,254));

  // set lights
  gRenderer.setAmbientLightColor(MAKE_ARGB(255,100,100,100));
  gRenderer.setDirectionalLightColor(0XFFFFFFFF);
  Vector3 dir = Vector3(5.0f,-5.0f, 6.0f);
  dir.normalize();
  gRenderer.setDirectionalLightVector(dir);
  	
  // Create water now that we know what camera to use
  float fov = degToRad(gGame.m_currentCam->fov);
  water = new Water(fov, farClippingPlane, "water.xml");
  
  m_objects->spawnWater(water);

	// aquire sounds  
  gSoundManager.setDopplerUnit(1.0f/3.0f); // sound factors
  // get windmill sound
  m_windmillSound = gSoundManager.requestSoundHandle("windmill2.wav");
  m_windmillSoundInstance = gSoundManager.requestInstance(m_windmillSound);  

  // add console commands  
  gConsole.addFunction("camerafollow","",consoleSetFollowCamera);
  gConsole.addFunction("cameratarget","s",consoleSetCameraTarget);
  gConsole.addFunction("godmode","b",consoleGodMode);

}

void StatePlaying::shutdown()
{
  delete m_tetherCamera; m_tetherCamera = NULL;
  delete water; water = NULL;
	delete terrain; terrain = NULL;
  delete m_objects; m_objects = NULL;
}

void StatePlaying::exitState()
{
  // stop windmill sound
  gSoundManager.stop(m_windmillSound, m_windmillSoundInstance);
}

void StatePlaying::enterState()
{
  // start up the windmill sound
  gSoundManager.play(m_windmillSound, m_windmillSoundInstance, true);
}

void StatePlaying::process(float dt)
{ 
  PlaneObject *planeObject = m_objects->getPlaneObject();

  // this should never happen but if it does leave
  if (planeObject == NULL) 
    return; 
    
  gConsole.process();
  
  // call process and move on all objects in the object manager
  m_objects->update(dt); 
    
  // process escape key and space bar
  processInput();

  // update location of camera
  processCamera(dt);
    
  // allow water to process per frame movements
  water->process(dt);
 
  // as soon as the plane crashes, start the timer
  if (planeObject->isPlaneAlive() == false && m_planeCrashed == false)
  {
    m_planeCrashed = true; // set that the plane has crashed
    m_timeSinceCrashed = 0.0f;
    m_crowID = -1;   
  }

  // once the timer hits 3 seconds, make the camera follow a crow
  if (m_planeCrashed)
  {
    m_timeSinceCrashed += dt;
    if (m_timeSinceCrashed >= 3.0f && m_crowID == -1)
    {
      // make the camera follow a crow around
      m_crowID = m_objects->getCrow();
      if (m_crowID != -1)
        m_tetherCamera->setTargetObject(m_crowID);

      // play the failed sound
      m_failedSound = gSoundManager.requestSoundHandle("Failed.wav");
      m_failedInstance = gSoundManager.requestInstance(m_failedSound);
      if(m_failedInstance != SoundManager::NOINSTANCE)
      {
        gSoundManager.setToListener(m_failedSound,m_failedInstance);
        gSoundManager.play(m_failedSound,m_failedInstance);
      }
    }
    else if(m_failedInstance != SoundManager::NOINSTANCE)
      gSoundManager.setToListener(m_failedSound,m_failedInstance);
  }

  // render reflection
  if (Water::m_bReflection)
  {
    // render water reflection    
    Plane plane( 0, 1, 0, -water->getWaterHeight());
    //get water texture ready  
    water->m_reflection.beginReflectedScene(plane);
    renderScene(true);
    water->m_reflection.endReflectedScene();    
  }

}

void StatePlaying::renderScreen()
{
  // render the entire scene
  renderScene();
  
  PlaneObject* plane = m_objects->getPlaneObject();
  if (plane != NULL)
  {
    //render plane reticles    
    plane->renderReticle(15.0f, 1.3f);
    plane->renderReticle(8.0f, 1.0f);
    
    if (plane->isPlaneAlive() == false)
    {
      int textY = gRenderer.getScreenY()/2;
      IRectangle rect = IRectangle(0,textY,gRenderer.getScreenX()-1, textY + 30);
      gRenderer.drawText("Press \"Space Bar\" to Respawn",&rect, eTextAlignModeCenter, false);
    }
  }

  // render FPS and console ontop of everything
  gGame.GameBase::renderConsoleAndFPS();
}

void StatePlaying::renderScene(bool asReflection)
{
  terrain->render(); // render the terrain   
   
  m_objects->render();
  
  // render water
  if (asReflection == false)      
    water->render(gGame.m_currentCam->cameraPos, gGame.m_currentCam->cameraOrient.heading);
    
   //render particles
  gParticle.render(!asReflection);   
}

void StatePlaying::resetGame()
{ 
  m_objects->clear();
  m_objects->spawnTerrain(terrain);
  m_objects->spawnWater(water);
  
  // Create plane  
  unsigned int planeID = m_objects->spawnPlane(Vector3
    (0.0f,terrain->getHeight(0.0f,0.0f) + 10.0f,0.0f));
  m_tetherCamera->setTargetObject(planeID);

  // Load silos
  m_objects->spawnSilo(LocationOnterrain(-30.0f, 10.0f, 100.0f));
  m_objects->spawnSilo(LocationOnterrain(-10.0f, 10.0f, 100.0f));
  m_objects->spawnSilo(LocationOnterrain(10.0f, 10.0f, 100.0f));
  m_objects->spawnSilo(LocationOnterrain(30.0f, 10.0f, 100.0f));

  // Load windmill
  unsigned int windmillID = m_objects->spawnWindmill(LocationOnterrain(60.0f, 0.0f, 100.0f));
  
  // Load crows
  static const int numCrows = 20;
  for(int i = 0; i < numCrows; ++i)
  {
    // Distibute evenly over a hollow cylinder around the windmill
    
    GameObject *windmill = m_objects->getObjectPointer(windmillID);
    const Vector3 &windmillPos = windmill->getPosition();
    Vector3 boxSize = windmill->getBoundingBox().size();
    float minDist = sqrt(boxSize.x * boxSize.x + boxSize.z - boxSize.z); // At least the windmill's maximum diameter away from the windmill
    float maxDist = 175.0f; // Maximum distance; season to taste
    float radius = Random.getFloat(minDist, maxDist);
    float angle = Random.getFloat(0,k2Pi);
    float height = windmillPos.y + boxSize.y * Random.getFloat(1.5, 2.0f) + 10.0f; // Randomize crow height to somewhat above the windmill
    Vector3 crowPos(cos(angle) * radius + windmillPos.x, height, sin(angle) * radius + windmillPos.z);
    float crowSpeed = Random.getFloat(0.8f,2.5f);
    // One speedy crow, one slow crow
    if(i == numCrows - 2)
      crowSpeed = 6.0f;
    else if(i == numCrows - 1)
      crowSpeed = 0.5;
    
    //crowSpeed = 0.0;
    m_objects->spawnCrow(crowPos, windmillPos, crowSpeed, Random.getBool());
  }

  // reset camera following crow variables
  m_planeCrashed = false;
  m_timeSinceCrashed = 0.0f;
  m_crowID = -1;

  // Set the tether camera
  m_tetherCamera->minDist = 10.0f;
  m_tetherCamera->maxDist = 20.0f;
  m_tetherCamera->fov;
  m_tetherCamera->reset();
  m_tetherCamera->process(0.0f);
  gGame.m_currentCam = m_tetherCamera;   // select tether camera as the current camera
  
  // kill all active particle effects
  gParticle.killAll();

  // release our endgame sound, and stop all currently playing sounds
  gSoundManager.releaseInstance(m_failedSound,m_failedInstance);
  m_failedInstance = SoundManager::NOINSTANCE;
  gSoundManager.stop();
}

void StatePlaying::processInput()
{
  PlaneObject *planeObject = m_objects->getPlaneObject();

  // Exit to menu if requested
  if (gInput.keyJustUp(DIK_ESCAPE, true)) 
  {        
    gGame.changeState(eGameStateMenu);    
    return;
  }

  // If you press space bar after you die reset game
  if (gInput.keyJustUp(DIK_SPACE))
   if (planeObject->isPlaneAlive() == false)
    {
      resetGame();
      return;
    }    
}

/// \param dt Change in time
void StatePlaying::processCamera(float dt)
{
  PlaneObject *planeObject = m_objects->getPlaneObject();
  Camera * cam = gGame.m_currentCam;

  // this should never happen but if it does leave
  if (planeObject == NULL || cam == NULL) 
    return; 

  // Grab current states
  Vector3 cameraPosOld = cam->cameraPos;
  Vector3 planePosOld = planeObject->getPosition();
  
  cam->process(dt); // move camera

  // Test for camera collision with terrain
  const float CameraHeight = 2.0f;
  float fTerrainHt = terrain->getHeight(cam->cameraPos.x,cam->cameraPos.z);
  if(cam->cameraPos.y - fTerrainHt < CameraHeight) 
    cam->cameraPos.y = fTerrainHt + CameraHeight;
  
  // location is now above terrain, set it
  cam->setAsCamera();

  // Tell terrain where the camera is so that it can adjust for LOD
  terrain->setCameraPos(cam->cameraPos);
 
  // Set 3D sound parameters based on new camera position and velocity
  Vector3 cameraVel = (cam->cameraPos - cameraPosOld) / dt;  
  gSoundManager.setListenerPosition(cam->cameraPos);
  gSoundManager.setListenerVelocity(cameraVel);
  gSoundManager.setListenerOrientation(cam->cameraOrient);
}

/// This is used to help spawn objects.
/// \param x X Coordinate on the terrain.
/// \param y Value that is added to the Y component after it is set to the height
/// of the terrain under point (X, Z)
/// \param z Z Coordinate on the terrain.
Vector3 StatePlaying::LocationOnterrain(float x, float y, float z)
{
  return Vector3(x,terrain->getHeight(x,z)+y, z);
}
