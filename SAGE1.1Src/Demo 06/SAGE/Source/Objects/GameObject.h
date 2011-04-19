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

/// \file GameObject.h
/// \brief Interface for the GameObject class.

#ifndef __GAMEOBJECT_H_INCLUDED__
#define __GAMEOBJECT_H_INCLUDED__

#include <string>
#include "DerivedModels/ArticulatedModel.h"
#include "Common/EulerAngles.h"
#include "Common/AABB3.h"
#include "Common/Vector3.h"
#include "Common/Renderer.h"
#include "Graphics/VertexTypes.h"

class GameObjectManager; /// \brief Represents a game entity, usually represented visually by a model.

/// Represents a game entity, usually represented visually by a model.  An object has one or more parts.
/// The first part (part 0) is the primary part, and all other parts are positioned and oriented relative
/// to the primary part.  An object, if managed, has a unique ID number; it also has a unique name,
/// either requested by the application or generated from its class name.  Finally, a derived class
/// may set the object's \c m_type field to perform rudimentary runtime type identification.
class GameObject{
public:
  friend class GameObjectManager;

  GameObject(Model *m, int parts=1, int frames=1);  ///< Constructs a new object.  Mostly used by derived classes.
  virtual ~GameObject(void);  ///< Destroys the object.
  void setModel(Model* m);  ///< Sets the model for the object.
  void setPosition(const Vector3& v, int part=0);  ///< Sets the position of the object (or one of its parts).
  void setPosition(float x, float y, float z, int part=0);  ///< Sets the position of the object (or one of its parts).
  const Vector3& getPosition(int part=0) const;  ///< Queries the object (or one of its parts) for its position.
  const Vector3 &getPreviousPosition() const; ///< Queries the object for its previous position.
  void setOrientation(const EulerAngles &orient, int part=0);  ///< Sets the orientation of the object (or one of its parts).
  const EulerAngles& getOrientation(int part=0) const;  ///< Queries the object (or one of its parts) for its orientation.
  const EulerAngles& getPreviousOrientation() const; ///< Queries the object for its previous orientation.
  void setModelOrientation(const EulerAngles &orient);  ///< Sets the default orientation of the object's model.
  void setSpeed(float speed);  ///< Sets the object's forward speed.
  void setRotationSpeedHeading(float speed, int part=0);  ///< Sets the rotation speed for the object (or one of its parts) on the heading axis.
  void setRotationSpeedPitch(float speed, int part=0);  ///< Sets the rotation speed for the object (or one of its parts) on the pitch axis.
  void setRotationSpeedBank(float speed, int part=0);  ///< Sets the rotation speed for the object (or one of its parts) on the bank axis.
  void incrementSpeed(float speed);  ///< Adjusts the forward speed of the object.
  const Vector3 transformObjectToInertial(const Vector3& position) const; ///< Transforms a position relative to this object to inertial (world) space.
  virtual void killObject() {m_lifeState = LS_DEAD;} ///< Sets the object's m_lifeState variable to LS_DEAD.  The object manager will then remove the object.
  
  virtual void computeBoundingBox();  ///< Updates the object's bounding box.
  const AABB3 &getBoundingBox() const;  ///< Queries the object for its axially-aligned bounding box.
  
  bool isAlive() const { return m_lifeState == LS_ALIVE; }  ///< Returns true iff the object is fully-grown and alive.
  virtual void process(float dt);  ///< Performs internal logic updates.
  virtual void move(float dt);  ///< Updates the object's position and other physical characteristics.
  virtual void render();  ///< Renders the object.

  unsigned int getID() const { return m_id; }  ///< Queries the object for its ID number.
  const std::string &getName() const { return m_name; }  ///< Queries the object for its name.
  const std::string &getClassName() const { return m_className; }  ///< Queries the object for its class name.
  int getType() const { return m_type; }  ///< Queries the object for its type.
  void setClassName(const std::string &className) { m_className = className; }  ///< Sets the object's class name.

protected:

  virtual void move(float dt, bool savePreviousState);

  // Object stage of life
  enum LifeState ///< Represents the stage of an object's life.
  {
    LS_NEW = 0,  ///< Object is spawned but not yet fully formed.
    LS_ALIVE,    ///< Object is alive and ready for processing.
    LS_DEAD      ///< Object is dead and ready for culling.
  };
  
  int m_nNumParts;  ///< Holds the number of parts.
  int m_nNumFrames; ///< Holds the total number of animation frames.
  Model* m_pModel;  ///< Points to the primary model.
  EulerAngles m_modelOrient; ///< Holds the relative orientation of main model (fixes disoriented models)
  EulerAngles* m_eaOrient; ///< Holds the orientations of parts
  EulerAngles* m_eaAngularVelocity; ///< Holds the angular velocity of parts
  Vector3* m_v3Position; ///< Holds the position of parts, first in world space, others in object space  
  float m_fSpeed; ///< Speed in view direction.
  float m_fCurFrame; ///< Current frame.
  float m_fDeltaTime; ///< Time change since last animation, in seconds.
	AABB3 m_boundingBox; ///< Contains the last computed bounding box.
	float m_animFreq; ///< Number of times an animation cycles per second.
	
  
	Vector3 m_oldPosition; ///< Previous position of the object.
	EulerAngles m_oldOrient; ///< Previous orientation of the object.

  LifeState  m_lifeState; ///< State used by object manager to, e.g., cull dead objects.
  unsigned int m_id; ///< Unique ID number.
  std::string m_name; ///< Unique name.
  std::string m_className; ///< Typically the name of the class, but can be changed; used to generate name.
  int m_type;              ///< Optionally used by games for runtime type identification.
  GameObjectManager *m_manager; ///< Points to this object's manager (if any).

  StandardVertexBuffer *m_vertexBuffer; ///< Dynamic vertex buffer to hold animated model data
};

#endif