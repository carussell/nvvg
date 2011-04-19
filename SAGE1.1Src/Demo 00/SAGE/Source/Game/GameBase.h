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

/// \file GameBase.h
/// Defines class GameBase.
/// Last updated June 26th, 2005

#ifndef __GAMEBASE_H_INCLUDED__
#define __GAMEBASE_H_INCLUDED__

#include <stdio.h>
#include "common/camera.h"

/// \brief Game class that a user can build off of.
/// GameBase is an abstract class and is to be used as a base class
/// for a game logic controlling class.  A derived class must override
/// functions GameBase.  The windows wrapper will then call these
/// overridden functions which allows game logic to be proccessed.
/// 3 Member functions should be overridden ( Initiate(), Main(), and Shutdown() ).
/// The WindowsWrapper class will Initiate the game engine and then call the initiate
/// function of the Game object.  It will then repetitively call the Main method until
/// the program terminates.  Upon termination, the Shutdown method will be called.

class GameBase
{
public:

	GameBase();	///< Constructor
	~GameBase();	///< Destructor
	
	virtual bool initiate();	///< called by the windows wrapper after the game engine is initiated
	
	virtual bool main();	///< called repetitively by the windows wrapper after initiate is called
  	
	virtual void shutdown();	///< called when the program is exiting
  
  virtual void renderScene();  ///< Renders the game scene.

  virtual void renderScreen();  ///< Renders the game scene and any overlays (e.g., console).

  virtual void process();  ///< Processes per-frame information.

  /// \brief Toggles the display of rendering information.
  /// \param enable Specifies whether to enable (true) or disable (false) the display.
  virtual void enableRenderInfo(bool enable) { m_renderInfo = enable; }
  
  /// \brief Toggles the display of object bounding boxes.
  /// \param enable Specifies whether to enable (true) or disable (false) the display.
  virtual void setBoundingBoxFlag(bool enable) { m_boundingBoxFlag = enable; }
protected:

  void renderInfo(); ///< Renders framerate and triangles rendered to the screen

  /// \name Camera members
  /// \brief Used to pass values to the effect file.
  //@{
  
  Camera * m_currentCam; ///< Pointer to the camera that is currently being used.

  //@}
  
  bool m_renderInfo; ///< True to render framerate and triangles rendered
  unsigned int m_renderInfoColor; ///< Color used to render info

  bool m_boundingBoxFlag; ///< True to render object bounding boxes
  float m_fpsTime; ///< Used to control rendering of framerate so it doesn't render every frame
  int m_fps; ///< Frames Per Second as currently being displayed
};

// extern a global pointer to the game object
extern GameBase* gGameBase; ///< global pointer to game object

#endif