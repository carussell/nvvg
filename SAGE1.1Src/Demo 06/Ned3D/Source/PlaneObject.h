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

/// \file PlaneObject.h
/// \brief Interface for the PlaneObject.

/// Edited by the SAGE team, 2005
/// Last updated June 13, 2006


#ifndef __PLANEOBJECT_H_INCLUDED__
#define __PLANEOBJECT_H_INCLUDED__

#include "Objects/GameObject.h"

/// \brief Represents a plan object.
class PlaneObject : public GameObject
{
public:
  friend class Ned3DObjectManager;
  
  /// \brief Global flag; Specifies whether or not the plane takes damage.
  /// A value of true indicates to take damage.  A value of false specifies
  /// "god mode".  This value defaults to true.
  static bool takeDamage;
  
  PlaneObject(Model *m);
  ~PlaneObject();
  
  // Render functions
  
  void renderReticle(float distance, float size);

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
  void inputFire();
  
  // Plane condition functions
  void killPlane();
  bool isPlaneAlive() const;
  bool isCrashing() {return m_planeState == PS_CRASHING;}
  int getHP() const;
  int getMaxHP() const;
  void setMaxHP(int maxHP, bool recoverAll = true);
  void damage(int damage);
  void recover(int hp);
  void recoverAll();

  


protected:

  // Protected members
  
  const Vector3 m_gunPosition; ///< Location of gun relative to plane.
  const Vector3 m_enginePosition; ///< Location of engine relative to plane.
  float m_propOffset;
  bool m_isPlaneAlive; ///< This is true if the plane is stuck in the ground
  float m_maxTurnRate;
  float m_maxPitchRate;
  float m_maxBankRate;
  float m_maxSpeed;  
  float m_turnRate;
  float m_pitchRate;
  float m_speedRatio; // speed at which the plane is moving (0.0 - 1.0), 1.0 is m_maxSpeed
  Vector3 m_velocity; ///< Velocity recorded to play 3D sound correctly
  Vector3 m_oldPosition; ///< Last position of plane (used to calculate Velocity    
  
  int m_reticleTexture; ///< Handle of reticle texture
  
  /// \brief Flags if the gun ray has been checked against all crows.
  //
  /// If the plane's gun is lined up with a crow, the reticle will
  /// change to a flashing state.  To check if the gun lines up with any crow
  /// is time consuming.  Because we draw 2 reticles each frame, this flag is
  /// needed to signal if we have already checked if a crow lines up with
  /// the gun.  
  bool m_reticleLockOnUpdated;
  bool m_reticleLockedOn; ///< Signals if the gun lines up with a crow
  
  /// \brief Array of all textures used on the plane
  /// The plane texture is set to the texture that is located at index m_hp.
  /// For instance, if m_hp is 1, m_allTextures[1] will be set
  std::vector<std::string> m_allTextures; 
  /// \brief Array of all smoke particle engines the plane uses
  /// The plane smoke trail is set to the particle system that is located at
  /// index m_hp.
  /// For instance, if m_hp is 1, m_allParticles[1] will be set
  std::vector<std::string> m_allParticles; 
  int m_smokeID; ///< Handle to the smoke particle system (-1 if there isn't one)
  int m_timeSinceFired; ///< Enforces time between bullet firing
  

  int m_hp;
  int m_maxHP;

  /// \brief Sets the texture of the model and the smoke particle system based
  /// on the damage the plane has taken
  void setTextureAndSmoke(); 
  
  enum PlaneState
  {
    PS_FLYING, PS_CRASHING, PS_DEAD
  };

  PlaneState m_planeState;
  
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
    
  // sounds
  int m_gunSound;

  // Input states

  TurnState m_turnState;
  PitchState m_pitchState;
  MoveState m_moveState;
};

#endif