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

/// \file Ned3DObjectManager.h
/// Defines the object management class for Ned3D
/// Last updated July 12, 2005

#ifndef __NED3DOBJECTMANAGER_H_INCLUDED__
#define __NED3DOBJECTMANAGER_H_INCLUDED__

#include "Common/Vector3.h"
#include "Common/EulerAngles.h"
#include "Objects/GameObjectManager.h"
#include "ObjectTypes.h"

class Model;
class ModelManager;
class PlaneObject;
class CrowObject;
class BulletObject;
class TerrainObject;
class WaterObject;
class SiloObject;
class WindmillObject;
class Terrain;
class Water;

class Ned3DObjectManager : public GameObjectManager
{
  public:
    
    Ned3DObjectManager();
    
    void setModelManager(ModelManager &models);
  
    virtual void clear();

    unsigned int spawnPlane(const Vector3 &position = Vector3::kZeroVector, const EulerAngles &orientation = EulerAngles::kEulerAnglesIdentity);
    unsigned int spawnCrow(const Vector3 &position = Vector3::kZeroVector, const EulerAngles &orientation = EulerAngles::kEulerAnglesIdentity, float speed = 0.0f);
    unsigned int spawnCrow(const Vector3 &position, const Vector3 &circleCenter, float speed = 0.0f, bool flyLeft = true);
    unsigned int spawnTerrain(Terrain *terrain);
    unsigned int spawnWater(Water *water);
    unsigned int spawnSilo(const Vector3 &position = Vector3::kZeroVector, const EulerAngles &orientation = EulerAngles::kEulerAnglesIdentity);
    unsigned int spawnWindmill(const Vector3 &position = Vector3::kZeroVector, const EulerAngles &orientation = EulerAngles::kEulerAnglesIdentity);

    unsigned int getCrow(); ///< Returns an index to the first crow in the list

    PlaneObject *getPlaneObject() { return m_plane; }
    
    virtual void deleteObject(GameObject *object);

  protected:
    // Ned3D-specific references
    
    ModelManager *m_models;

    Model *m_planeModel;
    Model *m_crowModel;
    Model *m_siloModel;
    Model *m_windmillModel;
    
    PlaneObject *m_plane;  ///> Points to the sole plane object.
    ObjectSet m_crows;     ///> Tracks the evil crows
    TerrainObject *m_terrain; ///> Points to the sole terrain object.  (not owned)
    WaterObject *m_water; ///> Points to the sole water object.  (not owned)
    ObjectSet m_furniture; ///> Silos, windmills, etc.
};


#endif

