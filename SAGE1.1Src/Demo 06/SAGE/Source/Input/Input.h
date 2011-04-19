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

/// \file input.h
/// \brief Interface for the input manager class InputManager.

/// Copyright Ian Parberry, 2004
/// Last updated November 12, 2004

#pragma once

#define DIRECTINPUT_VERSION 0x0800 //put before #include dinput.h

#include <windows.h>
#include <dinput.h> //DirectInput

#define DI_BUFSIZE 16 ///< Buffer size for DirectInput events from devices
#define DI_NUM_KEYBOARD_CODES 256 ///< Number of keyboard codes supported
#define JOYSTICK_AXIS_RANGE 10000 ///< maximum gamepad postion value
#define JOYSTICK_SLIDER_RANGE 100 ///< maximum position of the slider
#define JOYSTICK_DEADZONE 2000 ///< Deadzone is 20 percent (2000 out of 10000)

/// \brief Input manager class.  Manages keyboard, mouse, and joystick input
/// using DirectInput.
class InputManager
{  
public:
  InputManager(); ///< Constructor.
  ~InputManager(); ///< Destructor.

  void initiate(HINSTANCE hInstance, HWND hwnd); ///< Initiates input
  void shutdown(); ///< Release input objects
	
  /// \name Keyboard Queries 
  /// Used to obtain input from the keyboard
  //@{
  BOOL keyDown(DWORD keyCode, bool override = false); ///< Checks if a key is currently being pressed
	
  /// \brief Checks if a key has been pressed since the last input update
  BOOL keyJustDown(DWORD keyCode, bool override = false); 

  /// \brief Checks if a key has been released since the last input update
  BOOL keyJustUp(DWORD keyCode, bool override = false);

  /// \brief Allows user to obtain buffered input
  int keyBufferedInput(char* input, int maxCharacters);

  /// \brief Enables and disables keyboard input.
  void keyBoardEnable(bool enable);
  //@}

  /// \name Joystick Queries 
  /// Used to obtain input from the joystick
  //@{
  void joyEnable(bool enable = true); ///< enables or disables joystick
  bool joyEnabled(); ///< Returns true if a joystick is plugged in

  BOOL joyButtonDown(int buttonNumber); ///< Checks if a joystick button is currently being pressed
	
  /// \brief Checks if a joystick button has been pressed since the last input update
  BOOL joyButtonJustDown(int buttonNumber); 

  /// \brief Checks if a joystick button has been released since the last input update
  BOOL joyButtonJustUp(int buttonNumber);

  float joyPadPositionX(); ///< Returns X location of joystick/gamepad postion
  float joyPadPositionY(); ///< Returns Y location of joystick/gamepad postion

  bool joySlider(float* slideValue);///< Returns the slider location (0 - 1)
  
  //@}

  /// \brief Returns movement on X axis
  /// \return movement on X axis
  float getMouseLX() { return m_mouseLX; }
  
  /// \brief Returns movement on Y axis
  /// \return movement on Y axis
  float getMouseLY() { return m_mouseLY; };

  BOOL getLeftMouseDown() { return m_leftMouseDown; }
  BOOL getRightMouseDown() { return m_rightMouseDown; }

  void updateInput(); ///< updates directInput devices

private:
  LPDIRECTINPUT8 m_lpDirectInput; ///< DirectInput object.
    
  LPDIRECTINPUTDEVICE8 m_pKeyboard; ///< Keyboard device.    
  LPDIRECTINPUTDEVICE8 m_pMouse; ///< Mouse device.
  LPDIRECTINPUTDEVICE8 m_pJoystick; ///< Joystick device.    
	
  DIDEVICEOBJECTDATA m_DIod[DI_BUFSIZE]; ///< Input device buffer.

  
  /// \name Keyboard data
  /// Hold keyboard data
  //@{
      
  BOOL m_down[DI_NUM_KEYBOARD_CODES];     ///< Holds keys that are currently down  
  BOOL m_keeptrack[DI_NUM_KEYBOARD_CODES]; ///< Keeps track of last update's keys
  char m_bufferedInput[DI_BUFSIZE]; ///< Holds buffered input as characters
  int m_bufferedCount; ///< number of characters in m_bufferedInput
  /// \brief Whether the keyboard is taking input.  This can be set to true if
  /// you want the game to stop taking input.  An example would be when the
  /// console is down.
  bool m_keyBoardOn; ///< Whether the keyboard is taking input
  //@}

  HANDLE m_hMouseEvent; ///< Mouse interrupt event.
  POINT m_pointMouse; ///< Mouse point.
  float m_mouseLX; ///< Mouse movement on x axis
  float m_mouseLY; ///< Mouse movement on y axis
  BOOL m_leftMouseDown; ///< TRUE if the left mouse button is down
  BOOL m_rightMouseDown; ///< TRUE if the right mouse button is down

  // Joystick data          
  BOOL m_bJoystickHasSlider; ///< TRUE if the joystick has a slider.
  DIJOYSTATE2 m_joystickStateCurrent; ///< Holds the current state of the joystick  
  /// \brief Holds last current state (allows us to calculate what is new)
  DIJOYSTATE2 m_joystickStateLast;
  bool m_joystickEnable; ///< True is the joystick is enabled

  /// \brief converts a DirectInput keycode to a character and adds it to
  /// m_bufferedInput
  void AddCodeToBuffer(DWORD keystroke);
    
  //for text input
  BOOL letter(WPARAM keystroke,char& ascii); ///< Keystroke is a letter.
  BOOL number(WPARAM keystroke,char& ascii); ///< Keystroke is a digit.
    
	
  void InputManager::MouseMove(int x, int y);///< called when the mouse moves

  // update functions
  void ProcessKeyboardInput(); ///< Process buffered keyboard events.
  BOOL ProcessMouseInput(); ///< Process buffered mouse events.
  BOOL ProcessJoystickInput(); ///< Process polled joystick events.


  //setup functions
  BOOL InitKeyboardInput(HWND hwnd); ///< Set up keyboard input.
  BOOL InitMouseInput(HWND hwnd); ///< Set up mouse input.
  BOOL InitJoystickInput(HWND hwnd); ///< Set up joystick input

};

extern InputManager gInput;