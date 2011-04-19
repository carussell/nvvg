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
#include "Terrain/Terrain.h"
#include "WindowsWrapper/WindowsWrapper.h"
#include "Game.h"

/// Global game object
Game gGame;

/// Game Constructor.
/// Clears member variables to default values
Game::Game():
GameBase()
{ 
  water = NULL;
  terrain = NULL;
    
}

/// Game Initiate.
/// Initiates all game logic.  This is called after the game engine is 
/// initiated.
/// \return True on success.  False on failure (application will terminate).
bool Game::initiate()
{   
	// Setup program
  float farClippingPlane = 10000.0f;

  // Set far clipping plane
  gRenderer.setNearFarClippingPlanes(1.0f,farClippingPlane);    
	
  // Create terrain
  terrain = new Terrain(8,"grass.tga", "heightmap.tga",400.0f,20.0f); //powers of two for terrain size
  
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
  water = new Water();
 
  // setup the water how we want it
  water->alpha = 0.5f;
  water->centerLocation = Vector3(0,99.5,0);
  water->dimensions = terrain->getDimensions();
  water->textureScale = 2.0f;
  water->textureVelocity = Vector2(0.1f,0.2f);
  water->textureHandle = gRenderer.cacheTextureDX("water.tga");
  
  // set the camera's y position so it is above the terrain
  m_currentCam->cameraPos.y = 200.0f;

	return true;
}

/// Game Shutdown.
/// Game logic memory is deallocated here.
void Game::shutdown()
{   
  delete water; water = NULL;
	delete terrain; terrain = NULL;
 
	return;
}

void Game::renderScreen()
{
  m_currentCam->setAsCamera();

  terrain->render();
  water->render();

  gConsole.render();
  if (GameBase::m_renderInfo)
    GameBase::renderInfo();  
}


void Game::process()
{
  float dt = gRenderer.getTimeStep();
  
  // let console think
  gConsole.process();
 
  // process camera movement
  m_currentCam->process(dt);

  // tell the terrain the location of camera so the LOD can be recalculated
  terrain->setCameraPos(m_currentCam->cameraPos);
  
  // allow water texture to move
  water->process(dt);

  // if you press escape exit
  if (gInput.keyJustUp(DIK_ESCAPE, true))     
    gWindowsWrapper.quit();
   
}

