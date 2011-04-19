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

/// \file WindowsWrapper.cpp
/// Define code declared in WindowsWrapper.h.  Isolates the game engine user
/// from windows.
/// Last updated June 26th, 2005

#include <assert.h>

#include "WindowsWrapper.h"
#include "common/Renderer.h"
#include "Resource/ResourceManager.h"
#include "particle/ParticleEngine.h"
#include "input/Input.h"
#include "Console/Console.h"
#include "directorymanager/directorymanager.h"

/// \brief WindowsWrapper global instance.
//
/// This can be accessed by anyone to get information such as the windows
/// handle or window isntance.
WindowsWrapper gWindowsWrapper;	

/// WindowsWrapper Constructor.
/// Clears all member variables to their default values
WindowsWrapper::WindowsWrapper()
{
	hWndApp = NULL;
	hInstApp = NULL;
	idleInBackground = NULL;
	appInForeground = true;
	quitFlag = false;
	m_pGame = NULL;
}

/// WindowsWrapper Destructor.  Does nothing.  Shutdown logic is in the
/// Shutdown method.
WindowsWrapper::~WindowsWrapper()
{
	
}

/// WindowsWrapper Shutdown.
/// Shuts down the game engine and Destroys the window.  Called in WinMainWrap.
void WindowsWrapper::Shutdown()
{


  gParticle.shutdown();
  gInput.shutdown();
  gConsole.shutdown();	
  gRenderer.shutdown();

  destroyAppWindow();

  return;
}

/// WindowsWrapper Initiate
/// Asks user to select windowed or none windowed mode.  Creates the window.
/// Initiates basic engine objects (ex. Renderer, Input ...)
/// \param shaderDebugging Specifies whether or not shader debugging is
/// required.
/// \param loadingTexture Name of an image file to be displayed on the screen
/// during loading.  This image should be located in the default image
/// directory.
void WindowsWrapper::Initiate(bool shaderDebugging, const char* loadingTexture)
{

	char directory[2048];
	GetCurrentDirectory(2048, directory);

	gDirectoryManager.initiate(directory,"directories.xml");

	createAppWindow("Ned 3D");
  
	// Create the main application window
	VideoMode mode;
	mode.xRes = 1024;
	mode.yRes = 768;
	mode.bitsPerPixel = 24;
	mode.refreshHz = kRefreshRateDefault;

	// Set the mode

  bool bWindowed = true; //set true for windowed, false for full screen

  bWindowed =
    MessageBox(NULL,"Do you want to run full screen?",
    "Full screen or windowed",MB_ICONQUESTION|MB_YESNO)==IDNO;

  //set last param to true for windowed mode
	gRenderer.init(mode, shaderDebugging, bWindowed); 

  // load loading texture if one is specified
  // then display it
  // then deallocate it because it is only needed once
  if (loadingTexture != NULL)
  {
    gDirectoryManager.setDirectory(eDirectoryTextures);
    gRenderer.setARGB(0XFFFFFFFF);
    gRenderer.beginScene();
    gRenderer.clear(kClearFrameBuffer | kClearDepthBuffer);
    int textureID = gRenderer.cacheTextureDX(loadingTexture);
    gRenderer.selectTexture(textureID);
    gRenderer.renderTextureOverScreen();
    gRenderer.endScene();
    gRenderer.flipPages();
    gRenderer.selectTexture(-1);
    gRenderer.freeTexture(textureID);
  }  

  gConsole.initiate();
  gInput.initiate(hInstApp, hWndApp);
  gParticle.init("particle.xml");
  gDirectoryManager.setDirectory(eDirectoryXML);
	
	return;
}

/// WindowsWrapper RunProgram.
/// Calls Initiate on the game.  Repetitively calls main on the game until
/// quitFlag is true.  Calls Shutdown on the game when finished.
void WindowsWrapper::RunProgram()
{

  // if the game is null then leave
  if (m_pGame == NULL) 
  {
    assert("Game pointer is NULL. Must bail!");
    return;
  }
		
  // call initiate, if it signals an error then leave
  if (m_pGame->initiate() == false) 
  {
    assert("GameBase::initiate Failed");
    return;
  }
  
  // Reset renderer's page flip timer
  for(int i = 0; i < 2; ++i)
    gRenderer.flipPages();

  while(!quitFlag)
  {
    // processes messages
    idle(); 
    
    // if the quit flag was set then leave
    if (quitFlag) break;

    // call the main function for the GameBase Object passed in
    // if it signals an error then leave
    if (m_pGame->main() == false) break;

  } // end while
	
	
	m_pGame->shutdown();
	
	return;
}


/////////////////////////////////////////////////////////////////////////////
//
// global variables
//
/////////////////////////////////////////////////////////////////////////////

extern char g_errMsg[]; //error message if we had to abort
extern bool g_errorExit; // true if we had to abort

/////////////////////////////////////////////////////////////////////////////
//
// local stuff
//
/////////////////////////////////////////////////////////////////////////////


/// WindowsWrapper windows procedure.
/// Handler for messages from the Windows API. 
/// This function is called by the windows message pump
/// to process messages.  Do not call this function from your code!  Only
/// windows should call this function!
///  \param hWnd window handle
///  \param message message code
///  \param wParam parameter for message 
///  \param lParam second parameter for message
///  \return 0 if message is handled
LRESULT CALLBACK WindowsWrapper::WindowProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) 
{

	// Make sure this message is for our app
	if (hWnd != gWindowsWrapper.hWndApp) {
		return DefWindowProc(hWnd,message,wParam,lParam);
	}

	// Process the message
	switch(message)	
  {
		case WM_CLOSE:
		case WM_DESTROY:
    {
			// Display error message dialog box, if there is one
			if(g_errorExit)
				MessageBox(NULL, g_errMsg, "FATAL ERROR", MB_OK | MB_ICONERROR);
			// Now gettaoutahere
			gWindowsWrapper.quitFlag = true;
    } break;

		case WM_CHAR :
		{
      // Send the character to the console
			gConsole.pressedChar((char)wParam);	
		} break;
	}

	// Return the default window procedure
	return DefWindowProc(hWnd,message,wParam,lParam);
}

/// WindowsWrapper idle.
/// Contains windows message pump.  Gives time back to windows to process 
/// other programs.
void WindowsWrapper::idle() 
{

	// Check if there is a message for us, repeat if program is not
	// in the foreground

	for (;;) 
  {

		// Process all pending Windows messages

		MSG	msg;
		while (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) {
			TranslateMessage(&msg); 
			DispatchMessage(&msg);
		}

		// If we're in the foreground, return to normal message processing
		if (appInForeground) break;

		// If we've been told to quit the app, then bail this loop
		if (quitFlag) break;

		// We're in the background - always give back some time
		Sleep(20);

		// Check if we're supposed to idle in the background
		if (!idleInBackground) break;

		// Repeat until program is in the foreground,
		// or we signal program termination
	}

}

/// WindowsWrapper createAppWindow
/// Creates a new window for the application
void	WindowsWrapper::createAppWindow(const char *title) 
{

	// Register the window class

	WNDCLASS	wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstApp;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL,IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = title;
	RegisterClass(&wc);

	// Create our window

	hWndApp = CreateWindowEx(
		WS_EX_APPWINDOW | WS_EX_DLGMODALFRAME,
		title,
		title,
		WS_POPUP | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW,
		0,
		0,
		0,
		0,
		NULL,
		NULL,
		hInstApp,
		NULL
	);
}


/// WindowsWrapper destroyAppWindow.
/// Destroys the application window.  Called from shutdown.
void	WindowsWrapper::destroyAppWindow() 
{
	// Make sure we exist
	if (hWndApp != NULL) 
  {
		// Destroy it
		DestroyWindow(hWndApp);
		hWndApp = NULL;
	}
}

/// WindowsWrapper WinMainWrap
/// This function should be called from the user's WinMain Function.
/// It handles initiating, running and shutting down the engine and game.
/// \param hInstance Handle to the window.
/// \param pGame Pointer to an object derived from GameBase.
/// \param loadingTexture A image name to be used as the loading screen.
/// The texture will be loaded from the default texture directory.  This
/// parameter can be se to NULL resulting in no loading screen.
/// \param shaderDebugging Optional parameter specifying whether or not you 
/// will be debugging a pixel/vertex shader.  Setting this to true will create
/// the device in software.  This parameter defaults to false.
void WindowsWrapper::WinMainWrap(HINSTANCE hInstance, GameBase* pGame, const char* loadingTexture, bool shaderDebugging)
{
  // save pointer to game object derived from class GameBase
  m_pGame = pGame;
  
  // save a global pointer to the game
  gGameBase = pGame;
	
  // save the instance of the window
  hInstApp = hInstance;

  // Create the window and initate the engine
  Initiate(shaderDebugging, loadingTexture);

  // Run the actual game
  RunProgram();

  // Shutdown the game engine and destroy the window
  Shutdown();

  return;
}

