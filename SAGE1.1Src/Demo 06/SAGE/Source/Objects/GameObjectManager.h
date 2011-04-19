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

/// \file GameObjectManager.h
/// \brief Defines the object management class

/// Last updated July 12, 2005

#ifndef __GAMEOBJECTMANAGER_H_INCLUDED__
#define __GAMEOBJECTMANAGER_H_INCLUDED__

#include <hash_map>
#include <hash_set>
#include <list>
#include <string>
#include "Generators/IDGenerator.h"
#include "Generators/NameGenerator.h"

class GameObject;

/// \brief Manages a group of objects for a game.
class GameObjectManager
{
  public:
    static bool renderBB;
    
    // Nested types

    // Constructers/destructor
    
    GameObjectManager();  ///< Constructs an object manager.
    virtual ~GameObjectManager();  ///< Destroys the manager and all managed objects.
    
    virtual void clear();  ///< Destroys all managed objects.

    // State update functions
        
    void setNumberOfDeadFrames(unsigned int numFrames);  ///< Sets the number of frames to skip before processing begins.
    virtual void update(float dt);  ///< Updates the state of all objects.
    virtual void render();  ///< Renders all renderable objects.
    
    void computeBoundingBoxes(); ///< Updates all objects' bounding boxes.
    void renderBoundingBoxes();  ///< Renders all objects' bounding boxes.

    // addObject() -- Gives control of an object to the manager.  (doxygen comments in GameObjectManager.cpp)
    
    unsigned int addObject(GameObject *object) { return addObject(object, true, true, true, NULL); }
    unsigned int addObject(GameObject *object, const std::string &name) { return addObject(object, true, true, true, &name); }
    unsigned int addObject(GameObject *object, bool canMove, bool canProcess, bool canRender) { return addObject(object, canMove, canProcess, canRender, NULL); }
    unsigned int addObject(GameObject *object, bool canMove, bool canProcess, bool canRender, const std::string &name) { return addObject(object, canMove, canProcess, canRender, &name); }
    virtual void deleteObject(GameObject *object);  ///< Deletes an object.

    unsigned int getObjectID(const std::string &name);  ///< Queries the manager for an object's ID.
    GameObject *getObjectPointer(unsigned int id);  ///< Queries the manager for an object's pointer.
    GameObject *getObjectPointer(const std::string &name);  ///< Queries the manager for an object's pointer.

  protected:
    // Nested types
    
    typedef stdext::hash_set<GameObject *> ObjectSet;  ///< Represents a set of objects.
    typedef ObjectSet::iterator ObjectSetIter;  ///< Set iterator.
    typedef stdext::hash_map<std::string, unsigned int> NameToIDMap;  ///< Maps object names to object IDs.
    typedef NameToIDMap::iterator NameToIDMapIter;  ///< Map iterator.
    typedef stdext::hash_map<unsigned int, GameObject *> IDToObjectMap;  ///< Maps object IDs to object pointers.
    typedef IDToObjectMap::iterator IDToObjectMapIter;  ///< 
    
    virtual void process(float dt);  ///< Processes all objects.
    virtual void move(float dt);  ///< Moves all objects.
    virtual void handleInteractions();  ///< Processes interactions (such as collision) between objects and other post-movement processing.
    virtual bool interact(GameObject &obj1, GameObject &obj2);  ///< Processes interactions (such as collision) between two objects.

    virtual unsigned int addObject(GameObject *object, bool canMove, bool canProcess, bool canRender, const std::string *namePtr);  ///< Gives control of an object to the manager.
    virtual void updateObjectLifeStates();  ///< Updates new objects to "alive", and culls dead objects.

    /// \brief Contains and owns all managed objects.
    ///
    /// Contains and owns all managed objects.  Created objects are
    /// deleted from here.
    ObjectSet m_objects;
    
    /// \brief Lists all objects that can move.
    ///
    /// Lists all objects that can move.  The pointers in this list
    /// point to elements of objects; therefore, one should never
    /// call \p delete on a pointer in this list.
    ObjectSet m_movableObjects;

    /// \brief Lists all objects that will think during behavioral updates.
    ///
    /// Lists all objects that can think.  The pointers in this list
    /// point to elements of objects; therefore, one should never
    /// call \p delete on a pointer in this list.
    ObjectSet m_processableObjects;

    /// \brief Lists all objects that will be rendered.
    ///
    /// Lists all objects that can render.  The pointers in this list
    /// point to elements of objects; therefore, one should never
    /// call \p delete on a pointer in this list.
    ObjectSet m_renderableObjects;
    
    NameToIDMap m_nameToID;       ///< Maps object names to their IDs.
    IDToObjectMap m_idToObject;   ///< Maps object IDs to their pointers.
    
    IDGenerator m_objectIDs;      ///< Generates IDs for the objects.
    NameGenerator m_objectNames;  ///< Generates names for the objects.
    
    unsigned int m_numDeadFrames;  ///< Number of frames to skip processing at creation.
    unsigned int m_frameCount;  ///< Tracks the number of frames processed.
};

#endif
