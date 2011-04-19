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

/// \file WindowsWrapper.h
/// Code to interface the windows wrapper class WindowsWrapper

#ifndef __WINMAIN_H_INCLUDED__
#define __WINMAIN_H_INCLUDED__

#include <windows.h>
#include "Game/GameBase.h"

/// The windows wrapper class.  This class isolates the game engine from windows.

/// \mainpage SAGE: A Simple Academic %Game Engine
///
/// Students learning game programming in academia need an engine that is flexible, extensible, stable, 
/// and well-documented. Industry game engines are production code, code that is designed merely to work.
/// An academic game engine should be written to be easy to understand and modify, especially to relatively
/// inexperienced students. It should obey the educational principle “Proceed from the known into the unknown”. 
///
/// SAGE consists of a game engine developed as a sequence of demos, each built on its predecessor, 
/// a process called incremental development. Incremental Development has been used by Ian Parberry 
/// in his game programming classes since 1993, and his 2000 and 2001 books on 2D game programming. 
/// SAGE brings this experience to a fully 3D game engine, based on an educational pedagogy that has a 
/// proven track record of placing students in the game industry. 
///
/// SAGE Version 1 demos include:
///
/// - Demo 0: %Model viewer
/// - Demo 1: %Terrain
/// - Demo 2: Shaders
/// - Demo 3: %Game engine architecture
/// - Demo 4: Collision detection
/// - Demo 5: %Particle engine
/// - Demo 6: Sound
///
/// SAGE is open source code released under the BSD license. It is written in C++ using Visual Studio 2005 and
/// DirectX 9.0. The SAGE website http://larc.csci.unt.edu/sage includes full source code, tutorials, and additional 
/// information. SAGE was created by faculty and students in the Laboratory for Recreational Computing, Department of
/// Computer Science & Engineering, University of North Texas. 
///
/// Credits: Ian Parberry (Principal Investigator), Jeremy Nunn, Joe Scheinberg, Erik Carson, Jason Cole.
///
/// If you make use of SAGE in your class or research project, and/or derive new works from it, we would love to
/// hear from you. Please contact Ian Parberry, contact information at http://www.eng.unt.edu/ian.
///
/// SAGE was funded by a grant from Microsoft Research.

class WindowsWrapper{ ///class for the windows wrapper
public:

  WindowsWrapper(); ///< Constructor.
  ~WindowsWrapper(); ///< Destructor.
	
  HWND getHandle() {return hWndApp;} ///< Returns a handle to the windows
  HINSTANCE getHInstance() {return hInstApp;} ///< Returns the instance of the window
  void quit() {quitFlag = true;} ///< Forces the windows loop to exit

  bool isQuiting() {return quitFlag;} ///< Returns true if application is about to quit

  void WinMainWrap(HINSTANCE hInstance, GameBase* pGame, const char* loadingTexture, bool shaderDebugging = false); ///< Runs the entire program, This should be called from the global winmain function
  static LRESULT CALLBACK WindowProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam); ///< Callback method used to process windows messages

  void	idle(); ///< Perform per-frame tasks such as windows message processing.

private:
	
  HWND hWndApp; ///< Main application window handle
  HINSTANCE hInstApp; ///< Main application instance handle
  bool idleInBackground; ///< Should our program idle when we are in the background?
  bool quitFlag; ///< if this is true, the windows message pump will exit
  bool appInForeground; ///< Is our app in the foreground?
  GameBase* m_pGame; ///< Saves a pointer to the object derived from GameBase that was passed into WinMainWrap.

  void Initiate(bool shaderDebugging, const char* loadingTexture); ///< Creates the window and initiates the game engine
  void RunProgram(); ///< Runs the game by interfaces the Game object passed into winmain	
  void Shutdown(); ///< Shutdown the window	
  void createAppWindow(const char *title); ///< Create the main application window
  void destroyAppWindow(); ///< Destroy the main application window	
	

	
};


extern WindowsWrapper gWindowsWrapper;	
#endif