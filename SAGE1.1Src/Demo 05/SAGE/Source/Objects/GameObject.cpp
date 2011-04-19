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

#include "gameobject.h"
#include "common/rotationmatrix.h"
#include "common/MathUtil.h"
#include "derivedmodels/animatedmodel.h"
#include <stdlib.h>
#include <assert.h>

/// \param m Specifies the model used by the object.
/// \param parts Specifies the number of parts in the object.
/// \param frames Specifies the number of animation frames stored by the object.
GameObject::GameObject(Model *m, int parts, int frames):
  m_nNumParts(parts),
  m_nNumFrames(frames),
  m_fDeltaTime(0.0f),
  m_fCurFrame(0.0f),
  m_pModel(m),
  m_modelOrient(EulerAngles::kEulerAnglesIdentity),
  m_eaOrient(NULL),
  m_eaAngularVelocity(NULL),
  m_v3Position(NULL),
  m_fSpeed(0.0f),
  m_animFreq(1.0f),
  m_lifeState(LS_NEW),
  m_id(0),
  m_className("Object"),
  m_type(0),
  m_manager(NULL),
  m_vertexBuffer(NULL)
{
  m_eaOrient = new EulerAngles[m_nNumParts];
  m_eaAngularVelocity = new EulerAngles[m_nNumParts];
  m_v3Position = new Vector3[m_nNumParts];
  for(int i=0; i<m_nNumParts; i++){
    m_eaOrient[i] = EulerAngles::kEulerAnglesIdentity;
    m_eaAngularVelocity[i] = EulerAngles::kEulerAnglesIdentity;
    m_v3Position[i] = Vector3::kZeroVector;
  }

  if(frames > 1)
    m_vertexBuffer = ((AnimatedModel*)m)->getNewVertexBuffer();
  
  computeBoundingBox();
}

GameObject::~GameObject(void){
  // If managed, only the object manager can delete it
  assert(m_manager == NULL);
  delete [] m_eaOrient;
  delete [] m_eaAngularVelocity;
  delete [] m_v3Position;

  delete m_vertexBuffer;
}

/// \param m Specifies the new model for the object.
void GameObject::setModel(Model *m)
{
  m_pModel = m;
}

/// \param v Specifies the new position for the object.
/// \param part Specifies the part to be set.
void GameObject::setPosition(const Vector3& v,int part){
  assert(part >= 0 && part < m_nNumParts);
  m_v3Position[part] = v;
}

/// \param x Specifies the x-coordinate.
/// \param y Specifies the y-coordinate.
/// \param z Specifies the z-coordinate.
/// \param part Specifies the part to be set.
void GameObject::setPosition(float x, float y, float z, int part){
  assert(part >= 0 && part < m_nNumParts);
  setPosition(Vector3(x,y,z),part);
}

/// \param part Specifies the part number to be queried.
/// \return The position of the object/part.
const Vector3& GameObject::getPosition(int part) const {
  assert(part >= 0 && part < m_nNumParts);
  return m_v3Position[part];
}

/// \return The last recorded position of the object before its current position.
/// \note The previous position is typically updated on a call to move().
///     This value is undefined if move() hasn't been called on the object yet.
const Vector3 &GameObject::getPreviousPosition() const
{
  return m_oldPosition;
}

/// \param orient Specifies the new orientation of the object.
/// \param part Specifies the part to be set.
void GameObject::setOrientation(const EulerAngles &orient, int part){
  assert(part >= 0 && part < m_nNumParts);
  m_eaOrient[part] = orient;
}

/// \param part Specifies the part to be queried.
/// \return The orientation of the object/part.
const EulerAngles& GameObject::getOrientation(int part) const{
  assert(part >= 0 && part < m_nNumParts);
  return m_eaOrient[part];
}

/// \return The last recorded orientation of the object before its current orientation.
/// \note The previous orientation is typically updated on a call to move().
///     This value is undefined if move() hasn't been called on the object yet.
const EulerAngles &GameObject::getPreviousOrientation() const
{
  return m_oldOrient;
}

/// \param orient Sets the default orientation of the object's model.
void GameObject::setModelOrientation(const EulerAngles &orient)
{
  m_modelOrient = orient;
}

/// \param speed Specifies the new speed of the obectj.
void GameObject::setSpeed(float speed){
  m_fSpeed = speed;
}

/// \param speed Specifies the new rate of heading change of the object.
/// \param part Specifies the part to be set.
void GameObject::setRotationSpeedHeading(float speed, int part){
  assert(part >= 0 && part < m_nNumParts);
  m_eaAngularVelocity[part].heading = speed;
}

/// \param speed Specifies the new rate of pitch change of the object.
/// \param part Specifies the part to be set.
void GameObject::setRotationSpeedPitch(float speed, int part){
  assert(part >= 0 && part < m_nNumParts);
  m_eaAngularVelocity[part].pitch = speed;
}

/// \param speed Specifies the new rate of bank change of the object.
/// \param part Specifies the part to be set.
void GameObject::setRotationSpeedBank(float speed, int part){
  assert(part >= 0 && part < m_nNumParts);
  m_eaAngularVelocity[part].bank = speed;
}

/// \param speed Specifies the amount to add to the current speed.
void GameObject::incrementSpeed(float speed){
  m_fSpeed += speed;
}


/// \param position Vector relative to the object origin to transform 
/// to inertial (world) space
/// \return The vector passed in transformed from object space to 
/// interial (world) space
const Vector3 GameObject::transformObjectToInertial(const Vector3& position) const
{
  RotationMatrix rMat;
  Vector3 ret;
  rMat.setup(getOrientation());

  ret = rMat.objectToInertial(position);
  ret += getPosition();

  return ret;
}

void GameObject::computeBoundingBox()
{
  if(m_pModel == NULL) return;
  if(m_nNumFrames > 1) return;
  Matrix4x3 world, modelOrient, sub;
  world.setupLocalToParent(m_v3Position[0], m_eaOrient[0]);
  modelOrient.setupLocalToParent(Vector3::kZeroVector, m_modelOrient);
  world = modelOrient * world;
  if(m_nNumParts > 1)
    m_boundingBox = ((ArticulatedModel*)m_pModel)->getSubmodelBoundingBox(0,world);
  else
    m_boundingBox = m_pModel->getBoundingBox(world);
  for(int i = 1; i < m_nNumParts; ++i)
  {
    sub.setupLocalToParent(m_v3Position[i], m_eaOrient[i]);
    m_boundingBox.add(((ArticulatedModel*)m_pModel)->getSubmodelBoundingBox(i,sub * world));
  }
}

/// \return The last computed bounding box of the object.
const AABB3 &GameObject::getBoundingBox() const
{
  return m_boundingBox;
}

/// \param dt Specifies the amount of time since the last call to process, in seconds.
void GameObject::process(float dt)
{
}

void GameObject::move(float dt)
{
  move(dt, true);
}

void GameObject::render(){
  if(!m_pModel)return;

  gRenderer.instance(m_v3Position[0], m_eaOrient[0]);
  gRenderer.instance(Vector3::kZeroVector, m_modelOrient);
  if(m_nNumParts > 1) //articulated model
    ((ArticulatedModel*)m_pModel)->renderSubmodel(0);
  else if(m_nNumFrames > 1) // animated model
    ((AnimatedModel*)m_pModel)->render(m_vertexBuffer);
  else
    m_pModel->render(); //vanilla model

  for(int i=1; i<m_nNumParts; i++){
    gRenderer.instance(m_v3Position[i], m_eaOrient[i]);
    ((ArticulatedModel*)m_pModel)->renderSubmodel(i);
    gRenderer.instancePop(); // submodel i
  }

  gRenderer.instancePop(); // submodel 0
  gRenderer.instancePop(); // object
}

/// \param dt Specifies the amount of time since the last call to move, in seconds.
void GameObject::move(float dt, bool savePreviousState){
  if(savePreviousState)
  {
    m_oldPosition = m_v3Position[0];
    m_oldOrient = m_eaOrient[0];
  }

  //orientation
  float rotStep = dt;
  for(int i=0; i<m_nNumParts; i++){
    m_eaOrient[i].heading += m_eaAngularVelocity[i].heading * rotStep;
    m_eaOrient[i].pitch += m_eaAngularVelocity[i].pitch * rotStep;
    m_eaOrient[i].bank += m_eaAngularVelocity[i].bank * rotStep;
  }

  //displacement
  Vector3 bDisplacement(0, 0, 20.0f * dt * m_fSpeed);
  RotationMatrix Matrix;
  Matrix.setup(m_eaOrient[0]);
  Vector3 addend = Matrix.objectToInertial(bDisplacement);
  m_v3Position[0] += Matrix.objectToInertial(bDisplacement);

  //select animation frame, if necessary
  if(m_nNumFrames > 1)
  {
    m_fCurFrame += dt * ((AnimatedModel*)m_pModel)->numFramesInAnimation() * m_animFreq;
    Matrix4x3 world, modelOrient;
    world.setupLocalToParent(m_v3Position[0], m_eaOrient[0]);
    modelOrient.setupLocalToParent(Vector3::kZeroVector,m_modelOrient);
    world = modelOrient * world;
    ((AnimatedModel*)m_pModel)->selectAnimationFrame(m_fCurFrame, 0, *m_vertexBuffer, m_boundingBox, world); // TODO figure which frame to render based on state
    world;
  }
}
