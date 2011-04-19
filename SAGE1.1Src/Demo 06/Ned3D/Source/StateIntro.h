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

/// \file StateIntro.h
/// \brief Interface for the StateIntro class, which is the state of the game
/// when it is first loaded.

#ifndef __STATEINFO_H_INCLUDED__
#define __STATEINFO_H_INCLUDED__

#include "GameState.h"
#include <vector>

/// \brief Represents the intro game state.
class StateIntro : public GameState
{
public:
  StateIntro(); ///< Constructs a StateIntro object.

  void initiate(); ///< Initiates the object.

  void enterState(); ///< Handles preparing the state to be active.

  void process(float dt); ///< Handles the logic for the state object each frame.
  void renderScreen(); ///< Renders the screen each frame for this state.

private:
  
  std::vector<int> m_textureHandles; ///< Holds all the texture to be displayed.
  std::vector<int> m_soundHandles; ///< Holds all the sounds to be played.
    
  int m_currentTexture; ///< The texture that we're currently on
  int m_numTextures; ///< Number of textures to transition between
  float m_transitionTime; ///< Time for transition
  float m_elapsedTime; ///< Time since last texture change
  

};


#endif