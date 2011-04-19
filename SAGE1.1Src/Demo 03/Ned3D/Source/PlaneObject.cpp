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

#include <assert.h>
#include "Common/MathUtil.h"
#include "PlaneObject.h"
#include "Input/Input.h"
#include "ObjectTypes.h"
#include "common/RotationMatrix.h"
#include "game.h"
#include "directorymanager/directorymanager.h"

PlaneObject::PlaneObject(Model *m):
  GameObject(m,2),
  m_propOffset(0.4f),
  m_maxTurnRate(kPi * 0.25f),
  m_maxPitchRate(kPi * 0.25f),
  m_maxBankRate(kPi * 0.25f),
  m_turnState(TS_STRAIGHT),
  m_pitchState(PS_LEVEL),
  m_moveState(MS_STOP)
{
  assert(m);
  assert(m->getPartCount() >= 2);
  setModelOrientation(EulerAngles(kPi, 0.0f, 0.0f));
  setPosition(0,m_propOffset,0,1);
  m_fSpeed = 0.0f;
  m_maxSpeed = 1.0f;
  m_className = "Plane";
  m_type = ObjectTypes::PLANE;
  m_pitchRate = m_turnRate = 0.0f;

  // load texture that will be used on the plane.
  gRenderer.cacheTextureDX("plane2.1.tga");
}

PlaneObject::~PlaneObject()
{
}

void PlaneObject::process(float dt)
{
  m_oldPosition = getPosition();

  // Put any non-movement logic here

  // Unbuffered input (remove if handling input elsewhere)  
  inputStraight();
  inputLevel();
  inputStop();
 
  if (gInput.keyDown(DIK_A))  
    inputTurnLeft(1.0f);
  if (gInput.keyDown(DIK_D))  
    inputTurnRight(1.0f);
  if (gInput.keyDown(DIK_S))  
    inputDive(1.0f);
  if (gInput.keyDown(DIK_W))  
    inputClimb(1.0f);
    
  if (gInput.keyJustUp(DIK_EQUALS))
    inputSpeedUp();
  if (gInput.keyJustUp(DIK_MINUS))
    inputSpeedDown();
  if (gInput.keyDown(DIK_RETURN))
    inputForward(1.0f);

  // process all joystick input if it is disabled
  if (gInput.joyEnabled())
  {
    // set plane speed based on slider
    float speed = 1.0f;
    if (gInput.joySlider(&speed))
    {
      // if there is a slider
      inputForward(speed);    
    }
    else // there isn't a slider
    {
      // if button 1 is down then the plane flys foward
      if (gInput.joyButtonDown(1))
        inputForward(1.0f);         
    }

    if(gInput.joyPadPositionX() < 0.0f)  
      inputTurnLeft(-gInput.joyPadPositionX());  
    if(gInput.joyPadPositionX() > 0.0f)
      inputTurnRight(gInput.joyPadPositionX());
    if(gInput.joyPadPositionY() > 0.0f)
      inputDive(gInput.joyPadPositionY());
    if(gInput.joyPadPositionY() < 0.0f)
      inputClimb(-gInput.joyPadPositionY());
  }
}

void PlaneObject::move(float dt)
{
  EulerAngles &planeOrient = m_eaOrient[0];
  Vector3 displacement = Vector3::kZeroVector;
  
  switch(m_turnState)
  {
    case TS_LEFT:
    {
      planeOrient.heading -= m_maxTurnRate * m_turnRate * dt;
      if(planeOrient.bank < kPi * 0.25f)
        planeOrient.bank += m_maxBankRate * m_turnRate * dt;
    } break;
    case TS_RIGHT:
    {
      planeOrient.heading += m_maxTurnRate * m_turnRate * dt;
      if(planeOrient.bank > kPi * -0.25f)
        planeOrient.bank -= m_maxBankRate * m_turnRate * dt;
    }
  }
  switch(m_pitchState)
  {
    case PS_CLIMB:
    {
      planeOrient.pitch += m_pitchRate * m_maxPitchRate * dt;
    } break;
    case PS_DIVE:
    {
      planeOrient.pitch -= m_pitchRate * m_maxPitchRate * dt;
    }
  }
  clamp(planeOrient.pitch,-kPi * 0.125f,kPi * 0.125f);
  float bankCorrect = 0.5f * m_maxBankRate * dt;
  float pitchCorrect = 0.5f * m_maxPitchRate * dt;
  if(m_turnState == TS_STRAIGHT)
    if(planeOrient.bank > bankCorrect)
      planeOrient.bank -= bankCorrect;
    else if(planeOrient.bank < -bankCorrect)
      planeOrient.bank += bankCorrect;
    else planeOrient.bank = 0.0f;
  if(m_pitchState == PS_LEVEL)
    if(planeOrient.pitch > pitchCorrect)
      planeOrient.pitch -= pitchCorrect;
    else if(planeOrient.pitch < -pitchCorrect)
      planeOrient.pitch += pitchCorrect;
    else planeOrient.pitch = 0.0f;

  
  // Check move state
  
  switch(m_moveState)
  {
    case MS_STOP:
    {
      m_fSpeed = 0.0f;
    } break;
    case MS_FORWARD:
    {
      m_fSpeed = m_maxSpeed * m_speedRatio;
    } break;
    case MS_BACKWARD:
    {
      m_fSpeed = -m_maxSpeed * m_speedRatio;
    } break;
  }

  // Update propeller orientation

  m_eaOrient[1].bank += kPi * dt * 4.0f;
  

  // Move it
  
  GameObject::move(dt);
}

void PlaneObject::reset()
{
  m_v3Position[0].zero(); // collision handling will fix height
  m_eaOrient[0].identity();
  m_eaOrient[1].identity();
  m_fSpeed = 0.0f;
  m_maxSpeed = 1.0f;
  m_turnState = TS_STRAIGHT;
  m_pitchState = PS_LEVEL;
  m_moveState = MS_STOP;
}

void PlaneObject::inputTurnLeft(float turnRatio)
{
  m_turnRate = turnRatio;
  m_turnState = m_turnState == TS_RIGHT ? TS_STRAIGHT : TS_LEFT;
}

void PlaneObject::inputTurnRight(float turnRatio)
{
  m_turnRate = turnRatio;
  m_turnState = m_turnState == TS_LEFT ? TS_STRAIGHT : TS_RIGHT;
}

void PlaneObject::inputStraight()
{
  m_turnState = TS_STRAIGHT;
}

void PlaneObject::inputClimb(float climbRatio)
{
  m_pitchRate = climbRatio;
  m_pitchState = m_pitchState == PS_DIVE ? PS_LEVEL : PS_CLIMB;
}

void PlaneObject::inputDive(float diveRatio)
{
  m_pitchRate = diveRatio;
  m_pitchState = m_pitchState == PS_CLIMB ? PS_LEVEL : PS_DIVE;
}

void PlaneObject::inputLevel()
{
  m_pitchState = PS_LEVEL;
}

void PlaneObject::inputSpeedDown()
{
  m_maxSpeed -= 1.0f;
}

void PlaneObject::inputSpeedUp()
{
  m_maxSpeed += 1.0f;
}

void PlaneObject::inputForward(float speed)
{
  m_speedRatio = speed;
  m_moveState = m_moveState == MS_BACKWARD ? MS_STOP : MS_FORWARD;
}

void PlaneObject::inputBackward()
{
  m_moveState = m_moveState == MS_FORWARD ? MS_STOP : MS_BACKWARD;
}

void PlaneObject::inputStop()
{
  m_moveState = MS_STOP;
}
