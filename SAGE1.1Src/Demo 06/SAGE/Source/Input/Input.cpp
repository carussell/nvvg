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

/// \file input.cpp
/// \brief Code for the input manager class InputManager.

/// Copyright Ian Parberry, 2004.
/// Last updated November 12, 2004.

#include <objbase.h>
#include <stdio.h>

#include "input.h"

InputManager gInput;

/// Constructor.
InputManager::InputManager()
{ 
  m_bJoystickHasSlider = FALSE; //assume no slider unless we find one
  m_lpDirectInput = NULL;
  m_pKeyboard = NULL;
  m_pMouse = NULL;
  m_pJoystick = NULL;
  m_joystickEnable = true;
  m_keyBoardOn = true;
  m_bufferedCount = 0;
  
  for (int i = 0; i < DI_NUM_KEYBOARD_CODES; i++) // reset arrays
	  {m_down[i] = 0; m_keeptrack[i] = 0;}

  m_leftMouseDown = FALSE;
  m_rightMouseDown = FALSE;
}

InputManager::~InputManager()
{
	// call shutdown just in case it hasn't been shutdown already
	shutdown();
   
}

/// Initiates input
/// \param hInstance Instance handle
/// \param hwnd Window handle
void InputManager::initiate(HINSTANCE hInstance, HWND hwnd)
{
	  
  //create DirectInput object
  HRESULT hr=DirectInput8Create(hInstance, DIRECTINPUT_VERSION, 
    IID_IDirectInput8A, (void**)&m_lpDirectInput, NULL); 
  
  if(FAILED(hr)){
    m_lpDirectInput = NULL; //in case it fails
    return; //bail out
  }

  //device initializations
  InitKeyboardInput(hwnd); //set up keyboard
  InitMouseInput(hwnd); //set up mouse
  InitJoystickInput(hwnd); //set up joystick

	return;
}


/// Release input objects
void InputManager::shutdown()
{



  if(m_pKeyboard){ //release keyboard
    m_pKeyboard->Unacquire(); //unacquire device
    m_pKeyboard->Release(); //release it
	m_pKeyboard = NULL;
  }

  if(m_pMouse){ //release mouse
    m_pMouse->Unacquire(); //unacquire device
    m_pMouse->Release(); //release it
	m_pMouse = NULL;
  }

  if(m_pJoystick){ //release joystick
    m_pJoystick->Unacquire(); //unacquire device
    m_pJoystick->Release(); //release it
	m_pJoystick = NULL;
  }

  
  if(m_lpDirectInput){ //now release DirectInput
	m_lpDirectInput->Release();
	m_lpDirectInput = NULL;
  }
}

// checks if a key is currently being pressed
/// \param keyCode DirectInput keycode. See dinput.h for a complete list of all
/// (example: DIK_A, DIK_SHIFT ...)
/// \param override True to return keyboard input regardless of the the 
/// m_keyBoardOn flag.  This parameter defaults to false.
/// \return true if the key is down
BOOL InputManager::keyDown(DWORD keyCode, bool override)
{
  // return if the user doesn't want to override the keyboard state
  // and the keyboard is off.
  if (!override && !m_keyBoardOn) 
    return FALSE;


  // sanity check
  if (keyCode >= DI_NUM_KEYBOARD_CODES || keyCode < 0) 
	  return 0;

	return m_down[keyCode];
}

//  checks if a key has been pressed since the last input update
/// \param keyCode DirectInput keycode. See dinput.h for a complete list of all
/// (example: DIK_A, DIK_SHIFT ...)
/// \param override True to return keyboard input regardless of the the 
/// m_keyBoardOn flag.  This parameter defaults to false.
/// \return true if the key has just been pressed
BOOL InputManager::keyJustDown(DWORD keyCode, bool override)
{
  // return if the user doesn't want to override the keyboard state
  // and the keyboard is off.
  if (!override && !m_keyBoardOn) 
    return FALSE;

	// sanity check
	if (keyCode >= DI_NUM_KEYBOARD_CODES || keyCode < 0) 
		return 0;

  // if it is down and was down before return true
  // else return false
  return m_down[keyCode] && !m_keeptrack[keyCode];
}

// checks if a key has been released since the last input update
/// \param keyCode DirectInput keycode. See dinput.h for a complete list of all
/// (example: DIK_A, DIK_SHIFT ...)
/// \param override True to return keyboard input regardless of the the 
/// m_keyBoardOn flag.  This parameter defaults to false.
/// \return true if the key has just been released
BOOL InputManager::keyJustUp(DWORD keyCode, bool override)
{
  // return if the user doesn't want to override the keyboard state
  // and the keyboard is off.
  if (!override && !m_keyBoardOn) 
    return FALSE;

	// sanity check
	if (keyCode >= DI_NUM_KEYBOARD_CODES || keyCode < 0) 
		return 0;

  // if it was down and now it isn't down
  return !m_down[keyCode] && m_keeptrack[keyCode];
}


int InputManager::keyBufferedInput(char* input, int maxCharacters)
{
  int count = 0;

  //TODO: implement

  return 0;
}

/// The keyboard can still be queried when the keyboard is disabled by setting
/// the override flag on keyboard queries.
/// \param enable True to enable keyboard, flase to disable.
void InputManager::keyBoardEnable(bool enable)
{
  m_keyBoardOn = enable;
}


// enables or disables joystick
/// \param enable True to enable joystick (Defaults to true)
void InputManager::joyEnable(bool enable)
{
  m_joystickEnable = enable;  
}

bool InputManager::joyEnabled()
{
  return m_joystickEnable;
}

// checks if a joystick button is currently being pressed
/// \param buttonNumber Button number to check
/// \return True if the button is down
BOOL InputManager::joyButtonDown(int buttonNumber)
{
  if (!m_joystickEnable) return FALSE;

  // sanity check
  if (buttonNumber >= 128 || buttonNumber < 0)
    return 0;

  return 
    m_joystickStateCurrent.rgbButtons[buttonNumber] & 0x80;
}
	
// checks if a joystick button has been pressed since the last input update
/// \param buttonNumber Button number to check
/// \return True if the button has just been pressed
BOOL InputManager::joyButtonJustDown(int buttonNumber)
{
  if (!m_joystickEnable) return FALSE;

  // sanity check
  if (buttonNumber >= 128 || buttonNumber < 0)
    return 0;

  return 
    (m_joystickStateCurrent.rgbButtons[buttonNumber] & 0x80) &&
    !(m_joystickStateLast.rgbButtons[buttonNumber] & 0x80);
}

// checks if a joystick button has been released since the last input update
/// \param buttonNumber Button number to check
/// \return True if the button has just been released
BOOL InputManager::joyButtonJustUp(int buttonNumber)
{
  if (!m_joystickEnable) return FALSE;

  // sanity check
  if (buttonNumber >= 128 || buttonNumber < 0)
    return 0;

  return 
    !(m_joystickStateCurrent.rgbButtons[buttonNumber] & 0x80) &&
    (m_joystickStateLast.rgbButtons[buttonNumber] & 0x80);
}


float InputManager::joyPadPositionX()
{
  if (!m_joystickEnable) return 0.0f;
  
  return ((float)m_joystickStateCurrent.lX) / JOYSTICK_AXIS_RANGE;   
}

float InputManager::joyPadPositionY()
{
  if (!m_joystickEnable) return 0.0f;

  return ((float)m_joystickStateCurrent.lY) / JOYSTICK_AXIS_RANGE;
  
}

bool InputManager::joySlider(float* slideValue)
{
  float ret;
  if (!slideValue) 
    return false;
  if (!m_bJoystickHasSlider) 
    return false;  
  
  ret = ((float)m_joystickStateCurrent.rglSlider[0]) / JOYSTICK_SLIDER_RANGE;
    
  // invert the slider
  ret = 1.0f - ret;
  
  // cap the input just incase if goes over 1
  if (ret > 1.0f) ret = 1.0f;
  if (ret < 0.0f) ret = 0.0f; 
  
  (*slideValue) = ret;
  return true;
}



// Queries directInput devices
void InputManager::updateInput()
{

	ProcessKeyboardInput();
	ProcessMouseInput();
	ProcessJoystickInput();
}


//DirectInput setup routines

/// Initialize keyboard input.
/// Create the DirectInput keyboard device, set the cooperative level,
///  set for buffered input, and acquire the keyboard.
/// \param hwnd Window handle

BOOL InputManager::InitKeyboardInput(HWND hwnd){ //set up keyboard input

  //create keyboard device
  HRESULT hr=m_lpDirectInput->CreateDevice(GUID_SysKeyboard,&m_pKeyboard,NULL); 
  if(FAILED(hr)){
    m_pKeyboard=NULL; //in case it fails
    return FALSE; //bail out
  }

  //set keyboard data format, c_dfDIKeyboard is a "predefined global"
  //yucko ugly part of DirectInput, thanks a lot Microsoft
  hr=m_pKeyboard->SetDataFormat(&c_dfDIKeyboard); 
  if(FAILED(hr))return FALSE; //fail and bail

  //set the cooperative level 
  hr=m_pKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);  
  if(FAILED(hr))return FALSE; //fail and bail

  //set buffer size for buffered input
  DIPROPDWORD dipdw;
  dipdw.diph.dwSize = sizeof(DIPROPDWORD);
  dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
  dipdw.diph.dwObj = 0;
  dipdw.diph.dwHow = DIPH_DEVICE;
  dipdw.dwData = DI_BUFSIZE; //buffer size
  hr = m_pKeyboard->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);  
  if(FAILED(hr))return FALSE; //fail and bail

  //acquire the keyboard
  hr=m_pKeyboard->Acquire();  
  if(FAILED(hr))return FALSE; //fail and bail

  return TRUE;
}

/// Initialize mouse input.
/// Create the DirectInput mouse device, set the cooperative level,
/// set for buffered input, and acquire the mouse. Also creates the
/// mouse event to handle mouse interrupts, and sets the mouse cursor.
/// \param hwnd Window handle

BOOL InputManager::InitMouseInput(HWND hwnd){ //set up mouse input

  //create mouse device 
  HRESULT hr=m_lpDirectInput->CreateDevice(GUID_SysMouse, &m_pMouse,NULL);
    if(FAILED(hr)){
    m_pMouse = NULL; //in case it fails
    return FALSE; //bail out
  }

  //set mouse data format
  hr=m_pMouse->SetDataFormat(&c_dfDIMouse);
  if(FAILED(hr))return FALSE;  //fail and bail

  //set the cooperative level
  hr=m_pMouse->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
  if(FAILED(hr))return FALSE; //fail and bail

  //set up mouse event
  m_hMouseEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
  if(!m_hMouseEvent)return FALSE; //fail and bail
  hr = m_pMouse->SetEventNotification(m_hMouseEvent);
  if(FAILED(hr))return FALSE; //fail and bail

   //set buffer size
  DIPROPDWORD dipdw;
  dipdw.diph.dwSize = sizeof(DIPROPDWORD);
  dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
  dipdw.diph.dwObj = 0;
  dipdw.diph.dwHow = DIPH_DEVICE;
  dipdw.dwData = DI_BUFSIZE; //buffer size
  hr = m_pMouse->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);  
  if(FAILED(hr))return FALSE; //fail and bail

  //acquire the mouse
  hr=m_pMouse->Acquire();  
  if(FAILED(hr))return FALSE; //fail and bail


  return TRUE;
}
//next 2 globals are a kluge for callback functions
LPDIRECTINPUT8 g_lpDirectInput; ///< DirectInput object, for callback function.
LPDIRECTINPUTDEVICE8 g_pJoystick; ///< Joystick device, for callback function.
BOOL g_bJoystickHasSlider=FALSE; ///< Whether joystick has slider, for callback function.

///< Callback function for joystick enumeration.
///< \param pdidInstance Pointer to a DirectInput device instance.
///< \param pContext Context pointer, unused in this function but DirectInput requires it
///< \return DIENUM_STOP if we find a joystick, DIENUM_CONTINUE otherwise

BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext){

  //attempt to create joystick device.
  HRESULT hr = g_lpDirectInput->CreateDevice(pdidInstance->guidInstance, &g_pJoystick, NULL);
  
  //if it failed, then we can't use this joystick. Maybe the user unplugged it.
  if(FAILED(hr))return DIENUM_CONTINUE;

  //otherwise take the first joystick we get
  return DIENUM_STOP;
}

/// Callback function to enumerate joystick objects (eg axes, slider).
/// \param pdidoi Pointer to a DirectInput device object
/// \param pContext Context pointer, unused in this function but DirectInput requires it
///< \return DIENUM_STOP if we find a slider, DIENUM_CONTINUE otherwise

BOOL CALLBACK EnumObjectsCallback(const DIDEVICEOBJECTINSTANCE* pdidoi,
                                  VOID* pContext)
{
  //if object is an axis
  if(pdidoi->dwType&DIDFT_AXIS)
  { 
    //specify range
    DIPROPRANGE diprg; //range property structure    
    DIPROPDWORD dZoneProp; // deadzone property structure
    
    if (pdidoi->guidType != GUID_Slider)
    {
      diprg.lMin = -JOYSTICK_AXIS_RANGE; //minimum
      diprg.lMax = JOYSTICK_AXIS_RANGE; //maximum
      dZoneProp.dwData = JOYSTICK_DEADZONE;
    }
    else
    {
      diprg.lMin = 0; //minimum
      diprg.lMax = JOYSTICK_SLIDER_RANGE; //maximum
      dZoneProp.dwData = 20;
    }    
    
    diprg.diph.dwSize = sizeof(DIPROPRANGE); 
    diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
    diprg.diph.dwHow = DIPH_BYID; //by identifier
    diprg.diph.dwObj = pdidoi->dwType; //specify the enumerated axis
    
    

    if (pdidoi->guidType == GUID_Slider)
      g_bJoystickHasSlider = TRUE;

    //now set range
    if(FAILED(g_pJoystick->SetProperty(DIPROP_RANGE, &diprg.diph))) 
      return DIENUM_STOP; //player may have unplugged joystick
    
    // specify deadzone    
    dZoneProp.diph.dwSize		= sizeof(dZoneProp);
		dZoneProp.diph.dwHeaderSize	= sizeof(dZoneProp.diph);
		dZoneProp.diph.dwObj		= pdidoi->dwOfs;
		dZoneProp.diph.dwHow		= DIPH_BYOFFSET;
		
        
    //now set dead zone.  this may fail if a the axis doesn't need a deadzone
    g_pJoystick->SetProperty(DIPROP_DEADZONE, &dZoneProp.diph);      
  }
  
  return DIENUM_CONTINUE;
}  
                                   
/// Initialize joysticl input.
/// Create the DirectInput joystick device, set the cooperative level,
/// set the data format, get the slider, and acquire the joystick. 
/// \param hwnd Window handle
/// \return TRUE if it succeeds

BOOL InputManager::InitJoystickInput(HWND hwnd){ //set up joystick input

  HRESULT hr; //return result from DirectInput calls

  //here we go through contortions because a callback function cannot be a member function
  g_lpDirectInput = m_lpDirectInput; //save member variable to global

  //enumerate joysticks using EnumJoysticksCallback function
  hr = m_lpDirectInput->EnumDevices(DI8DEVCLASS_GAMECTRL, 
    EnumJoysticksCallback, NULL, DIEDFL_ATTACHEDONLY);
  if(FAILED(hr))return FALSE; //fail and bail
  m_pJoystick = g_pJoystick; //save resulting device as member variable
  if(m_pJoystick==NULL)return FALSE; //make sure we got one

  //set the data format to "simple joystick" - a predefined data format 
  hr = m_pJoystick->SetDataFormat(&c_dfDIJoystick2);
  if(FAILED(hr))return FALSE; //fail and bail   

  //set the cooperative level
  hr = m_pJoystick->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
  if(FAILED(hr))return FALSE; //fail and bail 

  //enumerate joystick components (eg axes, slider)
  hr = m_pJoystick->EnumObjects(EnumObjectsCallback, (VOID*)hwnd, DIDFT_ALL);
  if(FAILED(hr))return FALSE; //fail and bail 
  m_bJoystickHasSlider = g_bJoystickHasSlider;

  //acquire the joystick
  hr = m_pJoystick->Acquire();  
  if(FAILED(hr))return FALSE; //fail and bail

  return TRUE;
}

//keyboard processing


/// Process keyboard input.
/// Process buffered and unbuffered keypresses since the last time
/// this function was called
void InputManager::ProcessKeyboardInput()
{
  if(!m_pKeyboard)return; //no device so fail and bail
  
  DWORD dwElements=DI_BUFSIZE; //number of events to process
  UCHAR buffer[DI_NUM_KEYBOARD_CODES]; // holds unbuffered key presses
  HRESULT hr;

  // save backup of previous key presses
  memcpy(m_keeptrack, m_down,sizeof(BOOL) *DI_NUM_KEYBOARD_CODES);
  
  // clear arrays
  ZeroMemory(m_down,sizeof(BOOL) * DI_NUM_KEYBOARD_CODES);
  ZeroMemory(buffer,sizeof(UCHAR) * DI_NUM_KEYBOARD_CODES);

  // Unbuffered (RAW)  
   //get number of events to process into member buffer m_DIod
   hr=m_pKeyboard->GetDeviceState(DI_NUM_KEYBOARD_CODES, buffer);
    
  for (int a = 0; a < DI_NUM_KEYBOARD_CODES; a++) // update just down
	   m_down[a] = buffer[a] & 0x80;
	
	
  
	
  
  // Buffered input, good for typing

  //get number of events to process into member buffer m_DIod
  hr=m_pKeyboard->GetDeviceData(
    sizeof(DIDEVICEOBJECTDATA) ,m_DIod,&dwElements, 0);

  //if it failed, we may be able to acquire the device
  if(FAILED(hr)){ //lost the device somehow
    hr = m_pKeyboard->Acquire(); //try to reacquire
    while(hr == DIERR_INPUTLOST) hr = m_pKeyboard->Acquire(); //try real hard
    return; //live to fight another day
  }
  if(FAILED(hr))return; //fail and bail paranoia
  //process inputs from m_DIod now
  for(int i=0; (i<(int)dwElements); i++){ //for each input
    if(m_DIod[i].dwData & 0x80) //if key went down
    {
      // save that the key is down in the array      
      AddCodeToBuffer(m_DIod[i].dwOfs);
    }     
  }





  return;
}



/// \param keystroke DirectInput keycode to add
void InputManager::AddCodeToBuffer(DWORD keystroke)
{
  // make sure no buffer overflow
  if (m_bufferedCount >= DI_BUFSIZE) 
    return;



  m_bufferedInput[m_bufferedCount] ;

  m_bufferedCount++;



  

  return;
} 

// converts a DirectInput keycode to a character and adds it to m_bufferedInput
/// Process a letter.
/// \param keystroke player keystroke event
/// \param ascii keystroke as an ASCII letter character
/// \return TRUE if keystroke is a letter

BOOL InputManager::letter(WPARAM keystroke,char& ascii){
  BOOL result=FALSE;
  switch(keystroke){
    case DIK_A: result = TRUE; ascii = 'a'; break;
    case DIK_B: result = TRUE; ascii = 'b'; break;
    case DIK_C: result = TRUE; ascii = 'c'; break;
    case DIK_D: result = TRUE; ascii = 'd'; break;
    case DIK_E: result = TRUE; ascii = 'e'; break;
    case DIK_F: result = TRUE; ascii = 'f'; break;
    case DIK_G: result = TRUE; ascii = 'g'; break;
    case DIK_H: result = TRUE; ascii = 'h'; break;
    case DIK_I: result = TRUE; ascii = 'i'; break;
    case DIK_J: result = TRUE; ascii = 'j'; break;
    case DIK_K: result = TRUE; ascii = 'k'; break;
    case DIK_L: result = TRUE; ascii = 'l'; break;
    case DIK_M: result = TRUE; ascii = 'm'; break;
    case DIK_N: result = TRUE; ascii = 'n'; break;
    case DIK_O: result = TRUE; ascii = 'o'; break;
    case DIK_P: result = TRUE; ascii = 'p'; break;
    case DIK_Q: result = TRUE; ascii = 'q'; break;
    case DIK_R: result = TRUE; ascii = 'r'; break;
    case DIK_S: result = TRUE; ascii = 's'; break;
    case DIK_T: result = TRUE; ascii = 't'; break;
    case DIK_U: result = TRUE; ascii = 'u'; break;
    case DIK_V: result = TRUE; ascii = 'v'; break;
    case DIK_W: result = TRUE; ascii = 'w'; break;
    case DIK_X: result = TRUE; ascii = 'x'; break;
    case DIK_Y: result = TRUE; ascii = 'y'; break;
    case DIK_Z: result = TRUE; ascii = 'z'; break;
  }
  return result;
}

/// Process a digit.
/// \param keystroke player keystroke event
/// \param ascii keystroke as an ASCII digit character
/// \return TRUE if keystroke is a digit

BOOL InputManager::number(WPARAM keystroke, char& ascii){
  BOOL result=FALSE;
  switch(keystroke){
    case DIK_0: result=TRUE; ascii='0'; break;
    case DIK_1: result=TRUE; ascii='1'; break;
    case DIK_2: result=TRUE; ascii='2'; break;
    case DIK_3: result=TRUE; ascii='3'; break;
    case DIK_4: result=TRUE; ascii='4'; break;
    case DIK_5: result=TRUE; ascii='5'; break;
    case DIK_6: result=TRUE; ascii='6'; break;
    case DIK_7: result=TRUE; ascii='7'; break;
    case DIK_8: result=TRUE; ascii='8'; break;
    case DIK_9: result=TRUE; ascii='9'; break;
  }
  return result;
}


//mouse processing

/// Process mouse input.
/// Processes mouse events that have been buffered since the last
/// time this function was called.
/// \return TRUE if we still have control of the mouse

BOOL InputManager::ProcessMouseInput(){ //process buffered mouse events

  HRESULT hr;

  // Get immediate mouse data
  // reset mouse movement from last time
  m_mouseLX = 0.0f;
  m_mouseLY = 0.0f;

  DIMOUSESTATE mouseinput;
  m_pMouse->Poll();

  hr = m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouseinput);
  if(FAILED(hr)){ //lost the device somehow
    hr=m_pMouse->Acquire(); //try to reacquire
    while(hr == DIERR_INPUTLOST) hr = m_pMouse->Acquire(); //try real hard
    return FALSE; //live to fight another day
  }  
	
 	m_mouseLX = (float)mouseinput.lX;
	m_mouseLY = (float)mouseinput.lY;
	


  // now get buffered mouse data

  BOOL result=FALSE; //TRUE if game is over
  if(!m_pMouse)return FALSE; //no device so fail and bail
  DWORD dwElements=DI_BUFSIZE; //number of events to process

  

  //get number of events to process into member buffer m_DIod
  hr=m_pMouse->GetDeviceData(
    sizeof(DIDEVICEOBJECTDATA), m_DIod,&dwElements, 0);

  //if it failed, we may be able to acquire the device
  if(FAILED(hr)){ //lost the device somehow
    hr=m_pMouse->Acquire(); //try to reacquire
    while(hr == DIERR_INPUTLOST) hr = m_pMouse->Acquire(); //try real hard
    return FALSE; //live to fight another day
  }
  if(FAILED(hr))return FALSE; //fail and bail (paranoia)

  //process inputs from m_DIod now
  for(int i=0; i<(int)dwElements; i++){ //for each input
    switch(m_DIod[i].dwOfs){
      case DIMOFS_BUTTON0: //left mouse button
        if(m_DIod[i].dwData & 0x80)
          m_leftMouseDown = TRUE;
        else 
          m_leftMouseDown = FALSE;
        break;
      case DIMOFS_BUTTON1: //right mouse button
        if(m_DIod[i].dwData & 0x80)
          m_rightMouseDown = TRUE;
        else 
          m_rightMouseDown = FALSE;
        break;
      case DIMOFS_X: MouseMove(m_DIod[i].dwData, 0); break;
      case DIMOFS_Y: MouseMove(0, m_DIod[i].dwData); break;
    }
  }

  
  


  return result;
}

//mouse handlers

/// Handle mouse motion.
/// \param x Mouse x position
/// \param y Mouse y position

void InputManager::MouseMove(int x, int y){

  //process mouse move
  m_pointMouse.x += x; m_pointMouse.y += y;
  
  //m_mouseLX = (float)x;
  //m_mouseLY = (float)y;

  // TODO: implement mouse cursor
  //g_d3ddevice->SetCursorPosition(m_pointMouse.x, m_pointMouse.y, 0);
  
  
}


/// Process joystick input.
/// Poll the joystick. Try to reacquire it if its been lost.
/// Process button 0 as if it were the space bar. Process
/// plane speed from the slider (horizontal) and the y axis
/// (vertical). If there's no slider, use the x axis, which
/// works best for the gamepad and is weird to play but works for
/// a regular joystick.

BOOL InputManager::ProcessJoystickInput(){ //process polled joystick events

  HRESULT hr; //result
  
  // save the new on for next update
  m_joystickStateLast = m_joystickStateCurrent;
  
  // clear out the last joystick state
  ZeroMemory(&m_joystickStateCurrent, sizeof (m_joystickStateCurrent));

  if(g_pJoystick == NULL)return FALSE; //bail and fail

  //poll the device to read the current state
  hr = m_pJoystick->Poll(); 
  if(FAILED(hr)){ //something bad has happened
    hr = g_pJoystick->Acquire(); //try to reacquire
    while(hr == DIERR_INPUTLOST)
      hr = g_pJoystick->Acquire(); //try real hard
    //if something *very* bad has happened, pretend it hasn't 
    return FALSE; 
  }

  //get the input's device state
  hr = g_pJoystick->GetDeviceState(sizeof(DIJOYSTATE2), &m_joystickStateCurrent);
  if(FAILED(hr))return FALSE; //should have been reacquired during the Poll()
  

  
  return TRUE;
}



