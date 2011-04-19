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

#include "GameBase.h"
#include "console/console.h"
#include "input/input.h"
#include "common/Renderer.h"
#include "WindowsWrapper/WindowsWrapper.h"

// set pointer to null
GameBase* gGameBase = 0;

GameBase::GameBase()
{
  m_currentCam = &m_freeCamera;
  m_renderInfo = true;
  m_fpsTime = 0.0f;
  m_fps = 0;   
}

// renders the console and frames per second to the screen
void GameBase::renderConsoleAndFPS()
{
   gConsole.render();

  // render FPS information
  if (m_renderInfo)
    renderInfo();
}


bool GameBase::main()
{  
  // This makes sure the device is valid.
  gRenderer.validateDevice();

  // Update Input
  gInput.updateInput();

  // process game logic
  process();
 
  // return if quit flag was set
  if (gWindowsWrapper.isQuiting())
    return true;

  // draw the screen
  gRenderer.beginScene();
  gRenderer.clear(kClearFrameBuffer | kClearDepthBuffer | kClearToFogColor);
  renderScreen();
  gRenderer.endScene();
  gRenderer.flipPages();

  return true;
}

// Renders framerate and triangles rendered to the screen
void GameBase::renderInfo()
{
  gRenderer.setARGB(0XFFFFFFFF);

  float dt = gRenderer.getTimeStep();
  float updatesPerSecond = 4.0f;

  
  // increment time since last fps update
  m_fpsTime += dt;
    
  // update frames for fps
 if (m_fpsTime >= 1.0f / updatesPerSecond)
  {
    m_fpsTime = 0.0f;
    m_fps = (int)(1.0f / dt);
  }

  int tri = gRenderer.GetTrianglesRenderedLastScene();

  // calculate string
  char text[1024];
  //SECURITY-UPDATE:2/4/07
  //sprintf(text, "FPS: %d\nTriangles Per Frame: %d", m_fps, tri, 2);
  sprintf_s(text,sizeof(text), "FPS: %d\nTriangles Per Frame: %d", m_fps, tri, 2);
  
  // draw the text
  gRenderer.drawText(text, 10,10);

}