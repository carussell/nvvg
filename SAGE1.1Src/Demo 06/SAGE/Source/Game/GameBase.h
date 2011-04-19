/// \file GameBase.h
/// \brief Interface for the SoundManager class.

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

#ifndef __GAMEBASE_H_INCLUDED__
#define __GAMEBASE_H_INCLUDED__

#include <stdio.h>
#include "common/camera.h"
#include "DerivedCameras/freecamera.h"

class GameObjectManager;
class ModelManager;

/// \brief Game class that a user can build off of.
/// GameBase is an abstract class and is to be used as a base class for a game
/// logic controlling class.  A derived class must override functions GameBase.
/// The windows wrapper will then call these overridden functions which allows
/// game logic to be proccessed.
class GameBase
{
public:

	GameBase();	///< Constructor
		
  /// \brief Called by the windows wrapper after the game engine is initiated
  /// \return True on success and false on failure.  If a user overrides this
  /// function they can return false to exit the application.   
  virtual bool initiate() {return true;}
	
  /// \brief Called repetitively by the windows wrapper after initiate is 
  /// called
  /// \return This value is always true.  If a user overrides this function 
  /// they can return false to exit the application.  
	virtual bool main();	
  	
  /// \brief called when the program is exiting
  virtual void shutdown() {};	
  
  /// \brief Renders the game scene and any overlays (e.g., console).
  virtual void renderScreen() {}  
  
  /// \brief Processes per-frame logic.
  virtual void process() {}

  /// \brief Sets the current camera to the free camera.
  void setFreeCamera() {m_currentCam = &m_freeCamera;}

  /// \brief Sets the speed of the free camera
  /// \param unitsPerSecond Specifies the speed of the camera's movement in
  /// vertex units per second.
  void setFreeSpeed(float unitsPerSecond) 
    {m_freeCamera.setSpeed(unitsPerSecond);}

  /// \brief Toggles the display of rendering information.
  /// \param enable Specifies whether to enable (true) or disable (false) the 
  /// display.
  void enableRenderInfo(bool enable) { m_renderInfo = enable; }

  /// \brief Renders console and FPS to the screen.
  void renderConsoleAndFPS();

    
protected:

  /// \brief Renders framerate and triangles rendered to the screen
  void renderInfo(); 

  /// \name Camera members
  //@{ 
  /// \brief Instance of a free camera.  This is the default camera
  FreeCamera m_freeCamera; 
  /// \brief Pointer to the camera that is currently being used.
  Camera * m_currentCam; 
  //@} 
  
  bool m_renderInfo; ///< True to render framerate and triangles rendered
  /// \brief Used to control rendering of framerate so it doesn't render every
  /// frame
  float m_fpsTime; 
  int m_fps; ///< Frames Per Second as currently being displayed
};

// extern a global pointer to the game object
extern GameBase* gGameBase; ///< global pointer to game object

#endif