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
/// \brief Code to run game logic.

/// Last updated June 13th, 2006

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

/// Global game object
Game gGame;

/// Game Constructor.
/// Clears member variables to default values
Game::Game():
GameBase(),
m_gameState(eGameStatePlaying),
m_nextGameState(eGameStateNone),
m_mustChangeGameState(false)
{ 
  // set up array of pointers to all the game state we have
  m_allStates.resize(4);
  m_allStates[eGameStateNone] = NULL;
  m_allStates[eGameStatePlaying] = &m_statePlaying;
  m_allStates[eGameStateIntro] = &m_stateIntro;
  m_allStates[eGameStateMenu] = &m_stateMenu;
    
}

/// Game Initiate.
/// Initiates all game logic.  This is called after the game engine is 
/// initiated.
/// \return True on success.  False on failure (application will terminate).
bool Game::initiate()
{
  // Load all sounds
  gSoundManager.parseXML("sounds.xml");  
 
  // Change current state to the intro state
  m_state = NULL;
  changeState(eGameStateIntro);

  // Let all states initiate so that they're ready to go
  m_statePlaying.initiate();
  m_stateMenu.initiate();
  m_stateIntro.initiate();

  // Load console command comments from XML that are specific to NED3D
  gConsole.loadCommentsFromXml("nedConsoleDoc.xml");
    
	return true;
}

/// Game Shutdown.
/// Game logic memory is deallocated here.
void Game::shutdown()
{  
  // Shutdown states
  m_statePlaying.shutdown();
  m_stateIntro.shutdown();
  m_stateMenu.shutdown();

	return;
}

void Game::renderScreen()
{
  if (m_state != NULL)
    m_state->renderScreen();
}


void Game::process()
{
  
  // process state switching
  if (m_mustChangeGameState)
  {
    if (m_nextGameState != m_gameState)
    {
      if (m_state) // if there is a current state tell it we're leaving
        m_state->exitState();
      m_gameState = m_nextGameState;
      m_state = m_allStates[(int)m_gameState];
      if (m_state) 
        m_state->enterState(); // tell state we're starting
    } 
    m_mustChangeGameState = false;
    m_nextGameState = eGameStateNone;
  }

  // call process for current state
  if (m_state != NULL)
    m_state->process(gRenderer.getTimeStep());

}

 // Can be called to change the game state
/// \param state State to change to
void Game::changeState(EGameState state)
{
 m_nextGameState = state;
 m_mustChangeGameState = true;
}