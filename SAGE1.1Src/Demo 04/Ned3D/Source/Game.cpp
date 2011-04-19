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

/// \file Game.cpp
/// Code to run game logic.

#include <algorithm>
#include "Console/Console.h"
#include "Input/Input.h"
#include "Common/Random.h"
#include "Terrain/Terrain.h"
#include "WindowsWrapper/WindowsWrapper.h"
#include "Game.h"
#include "Common/MathUtil.h"
#include "Graphics/ModelManager.h"
#include "Objects/GameObject.h"

extern CRandom Random;

/// Global game object
Game gGame;

/// Global function that can be called from the console
bool Game::consoleSetFollowCamera(ParameterList* params,std::string* errorMessage)
{
  gGame.m_currentCam = gGame.m_tetherCamera;
	return true;
}

bool Game::consoleSetCameraTarget(ParameterList* params,std::string* errorMessage)
{
  unsigned int obj = gGame.objects->getObjectID(params->Strings[0]);
  if(obj == 0)
  {
    *errorMessage = "Object not found.";
    return false;
  }
  if(gGame.m_tetherCamera == NULL)
  {
    *errorMessage = "Tether camera not initialized.";
    return false;
  }
  gGame.m_tetherCamera->setTargetObject(obj);
  gGame.m_tetherCamera->reset();
  return true;
}

/// Game Constructor.
/// Clears member variables to default values
Game::Game():
GameBase()
{ 
  water = NULL;
  terrain = NULL;
  objects = NULL;

  m_tetherCamera = NULL;
}

/// Game Initiate.
/// Initiates all game logic.  This is called after the game engine is 
/// initiated.
/// \return True on success.  False on failure (application will terminate).
bool Game::initiate()
{   
	// Setup program
  float farClippingPlane = 2000.0f;

  // Set far clipping plane
  gRenderer.setNearFarClippingPlanes(1.0f,farClippingPlane);

  objects = new Ned3DObjectManager();
  objects->setNumberOfDeadFrames(2);
  m_tetherCamera = new TetherCamera(objects);
	
  // Create terrain
  terrain = new Terrain(8,"terrain.xml"); //powers of two for terrain size
  
  // Load models
  objects->setModelManager(gModelManager);
  gModelManager.importXml("models.xml");
  
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
  
  // set the camera's y position so it is above the terrain
  m_currentCam->cameraPos.y = 200.0f;

  // Start placing objects into our world
  objects->clear();
  objects->spawnTerrain(terrain);
  objects->spawnWater(water);
  
  // Create plane  
  unsigned int planeID = objects->spawnPlane(Vector3
    (0.0f,terrain->getHeight(0.0f,0.0f) + 10.0f,0.0f));

  // Load silos
  objects->spawnSilo(LocationOnterrain(-30.0f, 10.0f, 100.0f));
  objects->spawnSilo(LocationOnterrain(-10.0f, 10.0f, 100.0f));
  objects->spawnSilo(LocationOnterrain(10.0f, 10.0f, 100.0f));
  objects->spawnSilo(LocationOnterrain(30.0f, 10.0f, 100.0f));

  // Load windmill
  unsigned int windmillID = objects->spawnWindmill(LocationOnterrain(60.0f, 0.0f, 100.0f));
  
  // Load crows
  static const int numCrows = 20;
  for(int i = 0; i < numCrows; ++i)
  {
    // Distibute evenly over a hollow cylinder around the windmill

    GameObject *windmill = objects->getObjectPointer(windmillID);
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
    
    objects->spawnCrow(crowPos, windmillPos, crowSpeed, Random.getBool());
  }
  
  gConsole.addFunction("camerafollow","",consoleSetFollowCamera);
  gConsole.addFunction("cameratarget","s",consoleSetCameraTarget);

  // Set the tether camera
  m_tetherCamera->minDist = 10.0f;
  m_tetherCamera->maxDist = 20.0f;
  m_tetherCamera->fov;
  m_currentCam = m_tetherCamera;   // select tether camera as the current camera
  m_tetherCamera->setTargetObject(planeID);
  m_tetherCamera->reset();
  m_tetherCamera->process(0.0f);

  // Load console command comments from XML that are specific to NED3D
  gConsole.loadCommentsFromXml("nedConsoleDoc.xml");

	return true;
}

/// Game Shutdown.
/// Game logic memory is deallocated here.
void Game::shutdown()
{   
  delete water; water = NULL;
	delete terrain; terrain = NULL;
  delete objects; objects = NULL;
 
	return;
}

void Game::renderScene(bool asReflection)
{
  terrain->render();
   
  objects->render();
  
  // render water
  if (asReflection == false)      
    water->render(gGame.m_currentCam->cameraPos, gGame.m_currentCam->cameraOrient.heading);
}

void Game::renderScreen()
{
  m_currentCam->setAsCamera();

  renderScene(false);

  PlaneObject *plane = objects->getPlaneObject();
  if(plane != NULL)
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

  gConsole.render();
  if (GameBase::m_renderInfo)
    GameBase::renderInfo();  
}


void Game::process()
{
  float dt = gRenderer.getTimeStep();
  
  // let console think
  gConsole.process();
  
  // call process and move on all objects in the object manager
  objects->update(dt); 
 
  // process camera movement
  m_currentCam->process(dt);

  // tell the terrain the location of camera so the LOD can be recalculated
  terrain->setCameraPos(m_currentCam->cameraPos);
  m_currentCam->setAsCamera();

  // allow water texture to move
  water->process(dt);

  // if you press escape exit
  if (gInput.keyJustUp(DIK_ESCAPE, true))     
    gWindowsWrapper.quit();

  // If you press space bar after you die reset game
  if (gInput.keyJustUp(DIK_SPACE))
    if (objects->getPlaneObject()->isPlaneAlive() == false)
    {
      resetGame();
      return;
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

void Game::resetGame()
{ 
  objects->clear();
  objects->spawnTerrain(terrain);
  objects->spawnWater(water);
  
  // Create plane  
  unsigned int planeID = objects->spawnPlane(Vector3
    (0.0f,terrain->getHeight(0.0f,0.0f) + 10.0f,0.0f));
  m_tetherCamera->setTargetObject(planeID);

  // Load silos
  objects->spawnSilo(LocationOnterrain(-30.0f, 10.0f, 100.0f));
  objects->spawnSilo(LocationOnterrain(-10.0f, 10.0f, 100.0f));
  objects->spawnSilo(LocationOnterrain(10.0f, 10.0f, 100.0f));
  objects->spawnSilo(LocationOnterrain(30.0f, 10.0f, 100.0f));

  // Load windmill
  unsigned int windmillID = objects->spawnWindmill(LocationOnterrain(60.0f, 0.0f, 100.0f));
  
  // Load crows
  static const int numCrows = 20;
  for(int i = 0; i < numCrows; ++i)
  {
    // Distibute evenly over a hollow cylinder around the windmill
    
    GameObject *windmill = objects->getObjectPointer(windmillID);
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
    objects->spawnCrow(crowPos, windmillPos, crowSpeed, Random.getBool());
  }

  // Set the tether camera
  m_tetherCamera->minDist = 10.0f;
  m_tetherCamera->maxDist = 20.0f;
  m_tetherCamera->fov;
  m_tetherCamera->reset();
  m_tetherCamera->process(0.0f);
  m_currentCam = m_tetherCamera;   // select tether camera as the current camera
}

/// This is used to help spawn objects.
/// \x X Coordinate on the terrain.
/// \y Value that is added to the Y component after it is set to the height
/// of the terrain under point (X, Z)
/// \z Z Coordinate on the terrain.
Vector3 Game::LocationOnterrain(float x, float y, float z)
{
  return Vector3(x,terrain->getHeight(x,z)+y, z);
}
