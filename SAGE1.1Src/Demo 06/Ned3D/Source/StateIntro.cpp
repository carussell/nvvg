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

/// \file StateIntro.cpp
/// \brief Code for the StateIntro class.

/// Edited by the SAGE team, 2005
/// Last updated June 13, 2006

#include "StateIntro.h"
#include "Common/Renderer.h"
#include "Input/Input.h"
#include "game.h"
#include "Sound/SoundManager.h"
#include "DirectoryManager/DirectoryManager.h"

StateIntro::StateIntro()
{
  m_elapsedTime = 0.0f;
  m_numTextures =0;

}

void StateIntro::initiate()
{
      
  m_textureHandles.push_back(gRenderer.cacheTextureDX("Larc2.jpg"));
  gDirectoryManager.setDirectory(eDirectorySounds); // set directory to sound directory
  m_soundHandles.push_back(gSoundManager.requestSoundHandle("Larc.wav"));
  
  m_textureHandles.push_back(gRenderer.cacheTextureDX("Title.bmp"));
  m_soundHandles.push_back(-1);
  
  m_textureHandles.push_back(gRenderer.cacheTextureDX("Credits.jpg"));
  m_soundHandles.push_back(-1);
  
  m_numTextures = (int)m_textureHandles.size();

}


void StateIntro::enterState()
{
  m_elapsedTime = 0;
  m_currentTexture = 0;
  if (m_soundHandles[0] != - 1)
    gSoundManager.play(m_soundHandles[0],0);

}

void StateIntro::process(float dt)
{
  
  m_elapsedTime += dt;

  if (gInput.keyJustDown(DIK_ESCAPE, true))
    gGame.changeState(eGameStateMenu);
}

void StateIntro::renderScreen()
{
  float fadeTime = 2.0f; // Time for transition between textures
  float displayTime = 4.0f; // Time that each texture is displayed for
  float totalTimePerDisplay = fadeTime + displayTime;
  bool fading = false; // flags fading between textures
  float fadeRatio;

  
  // increment to next texture if necessary
  if (m_elapsedTime > totalTimePerDisplay)
  {
    m_currentTexture++;
    m_elapsedTime = 0;
    if (m_currentTexture >= (int)m_soundHandles.size())
      if (m_soundHandles[m_currentTexture] != -1)
      {
        gSoundManager.play(m_soundHandles[m_currentTexture],0);
      }

  }

  // check for fading between textures
  if (m_elapsedTime > displayTime)
  {
    fadeRatio = 1.0f - (totalTimePerDisplay - m_elapsedTime) / fadeTime; 
    fading = true;
  }

  // if we're on the last texture and trying to fade to the next texture,
  // change states.  This is because there is not another texture.
  if (fading == true && m_currentTexture == m_numTextures - 1)
  {
    gGame.changeState(eGameStateMenu);
    // turn off fading for this frame
    fading = false;
  }
  // if we are trying to render a texture that isn't there 
  if (m_currentTexture >= m_numTextures)
  {
    gGame.changeState(eGameStateMenu);
    // turn off fading and set texture for this frame
    fading = false;
    m_currentTexture = m_numTextures - 1;
  } 


  if (fading)
  {
    // first half fade out from current texture
    if (fadeRatio < 0.5f)
    {
      fadeRatio *= 2.0f; // get in range 0 and 1
      fadeRatio = 1.0f - fadeRatio; //complement it
      fadeRatio *= 255.0f; // get in range 0 to 255;
      if (fadeRatio > 255.0f) fadeRatio = 255.0f;
      gRenderer.setRGB(MAKE_RGB(fadeRatio,fadeRatio,fadeRatio));
      gRenderer.selectTexture(m_textureHandles[m_currentTexture]);
      gRenderer.renderTextureOverScreen();
    }
    // fade into next texture
    else 
    {
      fadeRatio = (fadeRatio - 0.5f) * 2.0f; // get in range 0 and 1
      fadeRatio *= 255.0f; // get in range 0 to 255;
      if (fadeRatio > 255.0f) fadeRatio = 255.0f;
      gRenderer.setRGB(MAKE_RGB(fadeRatio,fadeRatio,fadeRatio));
      gRenderer.selectTexture(m_textureHandles[m_currentTexture + 1]);
      gRenderer.renderTextureOverScreen();

    }
   

  }
  else // we're not fading plain jane render
  {
  
    gRenderer.setRGB(0XFFFFFFFF);
    gRenderer.selectTexture(m_textureHandles[m_currentTexture]);
    gRenderer.renderTextureOverScreen();
  }
 
}
