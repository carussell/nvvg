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
#include "GameObject.h"
#include "GameObjectManager.h"
#include "common/Renderer.h"

bool GameObjectManager::renderBB = false;

GameObjectManager::GameObjectManager() :
  m_numDeadFrames(0),
  m_frameCount(0)
{
}

GameObjectManager::~GameObjectManager()
{
  clear();
}

void GameObjectManager::clear()
{
  while(m_objects.begin() != m_objects.end())
    deleteObject(*(m_objects.begin()));
  m_objects.clear();
  m_movableObjects.clear();
  m_processableObjects.clear();
  m_renderableObjects.clear();
  m_objectIDs.clear();
  m_objectNames.clear();
  m_nameToID.clear();
  m_idToObject.clear();
  m_frameCount = 0;
}

void GameObjectManager::setNumberOfDeadFrames(unsigned int numFrames)
{
  m_numDeadFrames = numFrames;
}

/// \param dt Specifies the amount of time since last update, in seconds.
void GameObjectManager::update(float dt)
{
  updateObjectLifeStates();
  if(m_frameCount >= m_numDeadFrames)
  {
    process(dt);
    move(dt);
    computeBoundingBoxes();
    computeBoundingBoxes();
  }
  ++m_frameCount;
}

void GameObjectManager::render()
{
  for(ObjectSetIter it = m_renderableObjects.begin(); it != m_renderableObjects.end(); ++it)
    if((*it)->m_lifeState == GameObject::LS_ALIVE)
      (*it)->render();

  if (renderBB)
    renderBoundingBoxes();
}

void GameObjectManager::computeBoundingBoxes()
{
  for(ObjectSetIter it = m_objects.begin(); it != m_objects.end(); ++it)
    if((*it)->isAlive())
      (*it)->computeBoundingBox();
}

void GameObjectManager::renderBoundingBoxes()
{
  gRenderer.setARGB(0xFF000000);
  for(ObjectSetIter it = m_renderableObjects.begin(); it != m_renderableObjects.end(); ++it)
    if((*it)->isAlive())
      gRenderer.renderBoundingBox((*it)->getBoundingBox());
}

// Documentation for public addObject functions

/// \fn unsigned int GameObjectManager::addObject(GameObject *object)
/// \brief Gives ownership of an object to the manager and generates an ID and name for the object.
/// \param object Specifies the object.
/// \return The ID of the object.
/// \note Once the object has been given to the manager, it is owned by the manager; do not delete it,
///     as the manager will take care of deleting it when the need arises.

/// \fn unsigned int GameObjectManager::addObject(GameObject *object, const std::string &name)
/// \brief Gives ownership of an object to the manager and generates an ID and name for the object.
/// \param object Specifies the object.
/// \param name Specifies the desired name of the object; if this name is taken, another will be generated.
/// \return The ID of the object.
/// \note Once the object has been given to the manager, it is owned by the manager; do not delete it,
///     as the manager will take care of deleting it when the need arises.
/// \note The provided name will be granted unless already taken; if already taken, a number will be appended.
///     You can check the object's name after creation with GameObject::getName.

/// \fn unsigned int GameObjectManager::addObject(GameObject *object, bool canMove, bool canProcess, bool canRender)
/// \brief Gives ownership of an object to the manager and generates an ID and name for the object.
/// \param object Specifies the object.
/// \param canMove True iff the object needs move() to be called.  Turning immobile objects to false speeds up processing.
/// \param canProcess True iff the object needs process() to be called.  Turning "dumb" objects to false speeds up processing.
/// \param canRender True iff the object needs render() to be called.  Turning invisible objects to false speeds up rendering.
/// \return The ID of the object.
/// \note Once the object has been given to the manager, it is owned by the manager; do not delete it,
///     as the manager will take care of deleting it when the need arises.

/// \fn unsigned int GameObjectManager::addObject(GameObject *object, bool canMove, bool canProcess, bool canRender, const std::string &name)
/// \brief Gives ownership of an object to the manager and generates an ID and name for the object.
/// \param object Specifies the object.
/// \param canMove True iff the object needs move() to be called.  Turning immobile objects to false speeds up processing.
/// \param canProcess True iff the object needs process() to be called.  Turning "dumb" objects to false speeds up processing.
/// \param canRender True iff the object needs render() to be called.  Turning invisible objects to false speeds up rendering.
/// \param name Specifies the desired name of the object; if this name is taken, another will be generated.
/// \return The ID of the object.
/// \note Once the object has been given to the manager, it is owned by the manager; do not delete it,
///     as the manager will take care of deleting it when the need arises.
/// \note The provided name will be granted unless already taken; if already taken, a number will be appended.
///     You can check the object's name after creation with GameObject::getName.

/// \param object Specifies the object to be deleted.
void GameObjectManager::deleteObject(GameObject *object)
{
  if(object == NULL) return;
  m_nameToID.erase(object->m_name);
  m_idToObject.erase(object->m_id);
  m_objectIDs.releaseID(object->m_id);
  m_objectNames.releaseName(object->m_name);
  m_objects.erase(object);
  m_movableObjects.erase(object);
  m_processableObjects.erase(object);
  m_renderableObjects.erase(object);
  object->m_manager = NULL;
  delete object;
}

/// \param name Specifies the name of the object.
/// \return The object's ID.
unsigned int GameObjectManager::getObjectID(const std::string &name)
{
  NameToIDMapIter it = m_nameToID.find(name);
  if(it == m_nameToID.end())
    return 0;
  else
    return it->second;
}

/// \param id Specifies the id of the object.
/// \return A pointer to the object.
/// \warning Do not call \c delete on this function's return value.
GameObject *GameObjectManager::getObjectPointer(unsigned int id)
{
  IDToObjectMapIter it = m_idToObject.find(id);
  if(it == m_idToObject.end())
    return NULL;
  else
    return it->second;
}

/// \param name Specifies the name of the object.
/// \return A pointer to the object.
/// \warning Do not call \c delete on this function's return value.
GameObject *GameObjectManager::getObjectPointer(const std::string &name)
{
  return getObjectPointer(getObjectID(name));
}

/// This function handles any internal processing each object should perform before movement.
/// Typically, this function won't need to be overridden in a derived class.
/// \param dt Specifies the amount of time since the last call to process().
void GameObjectManager::process(float dt)
{
  // Process live objects (new objects spawned during this loop will be skipped until next frame)
  for(ObjectSetIter it = m_processableObjects.begin(); it != m_processableObjects.end(); ++it)
    if((*it)->m_lifeState == GameObject::LS_ALIVE)
      (*it)->process(dt);
}

/// This function handles all "normal" movement of each object--that is, any movement that
/// doesn't depend on other objects or other outside influences.  Typically, this function
/// won't need to be overridden in a derived class.
/// \param dt Specifies the amount of time since the last call to render().
void GameObjectManager::move(float dt)
{
  for(ObjectSetIter it = m_movableObjects.begin(); it != m_movableObjects.end(); ++it)
    if((*it)->m_lifeState == GameObject::LS_ALIVE)
      (*it)->move(dt);
}

// Documentation for protected addObject() function, used to implement the public overloads

/// \param object Specifies the object.
/// \param canMove True iff the object needs move() to be called.  Turning immobile objects to false speeds up processing.
/// \param canProcess True iff the object needs process() to be called.  Turning "dumb" objects to false speeds up processing.
/// \param canRender True iff the object needs render() to be called.  Turning invisible objects to false speeds up rendering.
/// \param name Points to the name to be requested.  If NULL, or if pointing to the empty string, generates a name from
///     the object's class name.
/// \note Once the object has been given to the manager, it is owned by the manager; do not delete it,
///     as the manager will take care of deleting it when the need arises.
/// \note If a name is provided, it will be granted unless already taken; if already taken, a number will be appended.
///     You can check the object's name after creation with GameObject::getName.
unsigned int GameObjectManager::addObject(GameObject *object, bool canMove, bool canProcess, bool canRender, const std::string *name)
{
  assert(object != NULL);
  if(object->m_manager == this)
    return object->m_id;
  assert(object->m_manager == NULL);
  object->m_manager = this;
  m_objects.insert(object);
  if(canMove)
    m_movableObjects.insert(object);
  if(canProcess)
    m_processableObjects.insert(object);
  if(canRender)
    m_renderableObjects.insert(object);
  object->m_id = m_objectIDs.generateID();
  if(name == NULL || name->length() == 0)
    object->m_name = m_objectNames.generateName(object->m_className);  // Generate default name
  else if(m_objectNames.requestName(*name))
    object->m_name = *name;                                            // Requested name accepted
  else
    object->m_name = m_objectNames.generateName(*name);                // Append number to requested name
  // Ensure new object status
  object->m_lifeState = GameObject::LS_NEW;
  // Add id and name mappings
  m_nameToID[object->m_name] = object->m_id;
  m_idToObject[object->m_id] = object;
  return object->m_id;
}

/// Updates the life state of all objects.  Newly-spawned objects are promoted to full "alive" status,
/// and objects marked as "dead" are culled from the manager.
void GameObjectManager::updateObjectLifeStates()
{
  // Promote new objects to "fully alive" and cull dead objects
  for(ObjectSetIter it = m_objects.begin(); it != m_objects.end();)
  {
    switch((*it)->m_lifeState)
    {
      case GameObject::LS_DEAD:
      {
        ObjectSetIter murderDeathKill = it;
        ++it;
        deleteObject(*murderDeathKill);
      } break;
      case GameObject::LS_NEW:
      {
        (*it)->m_lifeState = GameObject::LS_ALIVE;
        ++it;
      } break;
      default:
      {
       ++it;
      } break;
    };
  }
}