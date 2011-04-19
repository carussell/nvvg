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


#ifndef __PLANEOBJECT_H_INCLUDED__
#define __PLANEOBJECT_H_INCLUDED__

#include "Objects/GameObject.h"

class PlaneObject : public GameObject
{
public:
  friend class Ned3DObjectManager;
  
  PlaneObject(Model *m);
  ~PlaneObject();
  
  // Standard frame functions
  
  virtual void process(float dt);
  virtual void move(float dt);
  virtual void reset();  // resets object to default values

  // Input-handling functions (call on keyJustDown/Up)
  
  void inputTurnLeft(float turnRatio);
  void inputTurnRight(float turnRatio);
  void inputStraight();
  void inputClimb(float climbRatio);
  void inputDive(float diveRatio);
  void inputLevel();
  void inputSpeedUp();
  void inputSpeedDown();
  void inputForward(float speed);
  void inputBackward();
  void inputStop();

protected:

  // Protected members
  
  float m_propOffset;
  float m_maxTurnRate;
  float m_maxPitchRate;
  float m_maxBankRate;
  float m_maxSpeed;  
  float m_turnRate;
  float m_pitchRate;
  float m_speedRatio; // speed at which the plane is moving (0.0 - 1.0), 1.0 is m_maxSpeed
  
  // Input states

  enum TurnState
  {
    TS_STRAIGHT, TS_LEFT, TS_RIGHT,
  };
  enum PitchState
  {
    PS_LEVEL, PS_CLIMB, PS_DIVE
  };
  enum MoveState
  {
    MS_STOP, MS_FORWARD, MS_BACKWARD
  };

  TurnState m_turnState;
  PitchState m_pitchState;
  MoveState m_moveState;
};

#endif