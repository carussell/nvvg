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
/// Last updated June 26th, 2005

#include "Common/MathUtil.h"
#include "Common/Renderer.h"
#include "Common/Random.h"
#include "Common/RotationMatrix.h"
#include "DirectoryManager/DirectoryManager.h"
#include "Input/Input.h"
#include "WindowsWrapper/WindowsWrapper.h"
#include "Game.h"

Game gGame; ///< Global game object

Game::Game()
: GameBase(), m_curModel(NULL), m_vertexBuffer(NULL), 
  m_minViewDistance(5.0f), m_maxViewDistance(200.0f),
  m_nColorIndex (0),
  m_helpText    (&gRenderer)
{
  m_currentCam = new Camera();
  m_modelOrient.identity();
}

bool Game::initiate()
{
	
	// Call parent's init

	GameBase::initiate();

	// Set the window

	gRenderer.setFullScreenWindow();

  //Renderer settings

  gRenderer.setNearFarClippingPlanes(1.0f,2000.0f);

  //initialize the world

  float fclip = gRenderer.getFarClippingPlane();
 
  gRenderer.setFogEnable(false);
  gRenderer.setFogColor(MAKE_RGB(0, 0, 0));

  m_currentCam->cameraPos.z = -20.0f;
  
  m_textColor = MAKE_RGB(255,255,255); // set default text color to white

	return true;
}



/// Game Shutdown.
///  Game logic memory is deallocated here.

void Game::shutdown()
{
  // Call parent's shutdown
  GameBase::shutdown();

  delete m_curModel;
  m_curModel = NULL;

  delete m_vertexBuffer;
  m_vertexBuffer = NULL;
 
	return;
}

void Game::renderScene()
{
  // our scene is pretty simple with a single model. Just set the orientation
  // and render the model
  if(m_curModel != NULL)
  {
    gRenderer.instance(m_modelOrient);
    m_curModel->render(m_vertexBuffer);
    gRenderer.instancePop();    
  }
  gRenderer.setARGB(m_textColor);
  m_helpText.Draw();
}

void Game::process()
{
  gInput.updateInput();

  float dt = gRenderer.getTimeStep();

  // process camera 
  m_currentCam->process(dt);
  
  // set camera
  m_currentCam->setAsCamera();

  static float totalTime = 0; // keep track of the time for animation
  totalTime += dt;

  // load our vertex buffer with the interpolated model
  if(m_curModel != NULL)
    m_curModel->selectAnimationFrame(totalTime, 0, *m_vertexBuffer);


  /////////////////////////////////////////////////////////////
  // process input
  /////////////////////////////////////////////////////////////

  // escape is our exit key
  if(gInput.keyDown(DIK_ESCAPE))
    gWindowsWrapper.quit();

  // load model
  if(gInput.keyJustDown(DIK_F2))
    loadModel();

  // toggle background color
  if(gInput.keyJustDown(DIK_F3))
  {
    m_nColorIndex++;
    if(m_nColorIndex > 4)
      m_nColorIndex = 0;
    
    unsigned int bkColor;
    switch(m_nColorIndex)
    {
    case 0:
      bkColor = MAKE_RGB(0, 0, 0);      
      break;
    case 1:
      bkColor = MAKE_RGB(255, 0, 0);
      break;
    case 2:
      bkColor = MAKE_RGB(0, 255, 0);
      break;
    case 3:
      bkColor = MAKE_RGB(0, 0, 255);
      break;
    case 4:
      bkColor = MAKE_RGB(255, 255, 255);
      break;
    }

	  gRenderer.setFogColor(bkColor);
    
	  if (bkColor == MAKE_RGB(255,255,255))
      m_renderInfoColor = m_textColor = MAKE_RGB(0,0,0);
	  else
	    m_renderInfoColor = m_textColor = MAKE_RGB(255,255,255);
	
  }

  if(gInput.keyJustDown(DIK_TAB))
    m_helpText.ToggleVisibility();

  if(gInput.getLeftMouseDown()) // rotate the model
  {
    EulerAngles orientDelta(
      wrapPi(gInput.getMouseLX() / 100.0f),
      -wrapPi(gInput.getMouseLY() / 100.0f),
      0);

    Matrix4x3 mOrientDelta;
    mOrientDelta.setupLocalToParent(Vector3::kZeroVector, orientDelta);

    m_modelOrient = m_modelOrient * mOrientDelta;
  }
  else if(gInput.getRightMouseDown()) // move camera in/out
  {
    m_currentCam->cameraPos.z += gInput.getMouseLY();

    // remember our camera stays in the negative z direction, looking in the
    // direction of positive z.
    if(m_currentCam->cameraPos.z < -m_maxViewDistance)
      m_currentCam->cameraPos.z = -m_maxViewDistance;
    else if(m_currentCam->cameraPos.z > -m_minViewDistance)
      m_currentCam->cameraPos.z = -m_minViewDistance;
  }
}


void Game::loadModel()
{
  // get file names of frames to animate
  std::list<std::string> filenames;
  std::string directory;
  gWindowsWrapper.showOpenFileDialog(true, "S3D Files (*.s3d)\0*.s3d\0", directory, filenames);

  // make sure the user didn't press cancel
  if((int)filenames.size() == 0)
    return;

  delete m_curModel; m_curModel = NULL;
  delete m_vertexBuffer; m_vertexBuffer = NULL;

  std::list<const char*> frames;
  while(filenames.begin() != filenames.end())
  {
    std::string *fullpath = new std::string();
    *fullpath = directory + "\\" + (*(filenames.begin()));
    frames.push_back(fullpath->c_str());
    filenames.pop_front();
  }

  m_curModel = new AnimatedModel((int)frames.size());
  m_curModel->m_bModelLerp = true;
  m_curModel->importS3d(frames, false);

  if(!m_curModel->isValid())
  {
    delete m_curModel;
    m_curModel = NULL;
    m_vertexBuffer = NULL;
    return;
  }

  while(frames.begin() != frames.end())
  {
    delete (*(frames.begin()));
    frames.pop_front();
  }

  m_vertexBuffer = m_curModel->getNewVertexBuffer();
}
