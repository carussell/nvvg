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

/// \file Terrain.h
#ifndef __TERRAIN_H_INCLUDED__
#define __TERRAIN_H_INCLUDED__

#include "common/renderer.h"
#include "HeightMap.h"
#include "terrainsubmesh.h"
#include "common/vector3.h"

/// \class Terrain
/// \brief Represents a heightmap based landscape
class Terrain
{
public:

  /// \name Global Variables
  /// These values are global so that they can be changed directly be
  /// global functions.  This is important so that we can change these
  /// values from the console.  
  //@{  
  /// \ brief Global flag; specifies if the textures on the terrain need to be
  /// distorted.
  static bool terrainTextureDistortion;
  /// \ brief Global flag; specifies if the Level of detail to render the
  /// terrain at.    
  static int LOD;  
  /// \ brief Texture handle requested to be used when rendering
  /// the terrain.  m_textureHandle eventually becomes this value.  
  static int textureHandle;
  /// \brief Value to multiply all texture coordinates by.  m_textureScale will
  /// eventually become this value.   
  static float textureScale;  
  /// \brief Specifies if we should morph the sub-meshes vertices to avoid
  /// cracks between sub-meshes.
  static bool crackRepair;
  //@}

  Terrain(int submeshPerSide, const char* textureName, 
    const char* heightmapName, float maxHeight, float stretch);
  ~Terrain();  
    
  /// \name Level of Detail
  //@{
  void setCurrentLOD(int n); ///< Sets the level of detail of the entire terrain  
  /// \brief Enables and disables crack repair between different levels of
  /// detail
  /// \param b On or Off
  void setCrackRepair(bool b) {m_bCrackRepair = b;}    
  //@}  

  void render(); ///< Renders the terrain
  void clearNormals(); ///< Sets all normals to the up vector
  void initNormals(); ///< Calculates the normals from the heights  
  float getHeight(float x, float z); ///< Get height of terrain at (x,z)
  Vector3 getNormal(float x, float z); ///< Get normal of terrain at (x,z)
  
  void setSubMeshes(); ///< Loads submeshes from current terrain data.
  /// \brief Used to set the location of the current camera.
  void setCameraPos(const Vector3& p); 
  /// \brief Checks to see if a point is over or under the terrain.
  
  /// \brief Gets the total width/length in world units of the terrain
  /// \return Width/Length of the terrain in world units
  float getDimensions() {return (m_nVPS - 1) * m_fDelta;}
private:  
  int m_nSide; ///< Number of quads per side
  int m_nSubmeshSide; ///< Number of quads per submesh side
  int m_nNumQuads; ///< Number of quads in total
  int m_nVPS; ///< Number of vertices per side
  int m_nSubmeshRatio; ///< Ratio of submesh side to parent
  int m_nMaxLOD; ///< Maximum LOD (Level of Detail)
  int m_nNumVertices; ///< Total number of vertices
  int m_nNumTriangles; ///< Total number of triangles
  float m_fDelta; ///< Distance between vertices
  float m_fOriginOffset; ///< Origin offset to center
  bool m_bDistanceLOD; ///< True for distance based lod
  bool m_bCrackRepair; ///< True for crack repair in distance LOD
  int m_nCurrentLOD; ///< Current LOD level
  HeightMap* m_pHeightMap; ///< Height map
  /// \brief Array of arrays of all the submeshes.  This is needed because
  /// of multiple levels of detail.
  TerrainSubmesh*** m_pSubmesh;
  RenderVertex *m_vertices; ///< The entire terrain as one mesh
  RenderTri *m_triangles; ///< Triangles represented by indices into the m_vertex array
  Vector3 *m_triangleNormals; ///< Triangle normal for every triangle
  
  /// \brief Texturing Variables
  //{@
  bool m_TextureDistorted; ///< True if the texture is distorted
  float m_textureScale; ///< Scaling for texture coordinates
  int m_textureHandle; ///< Handle to current texture
  //@}
    
  float m_maxHeight; ///< Maximum height of terrain
  Vector3 m_v3CameraPos; ///< Camera position.  This is used to computer LOD
  int m_nCameraSubmeshRow; ///< Subgrid row for camera position
  int m_nCameraSubmeshCol; ///< Subgrid column for camera position
  int **m_pSubmeshLODLevel; ///< LOD level for each submesh
  
  /// \name Texture Distortion
  //@{
  void setTextureDistortion(); ///< Enables texture coordinate distortion
  void clearTextureDistortion(); ///< Turns off texture distortion
  //@}
  /// \brief Returns the row and column of the location (x, z)
  void getSubmeshIndex(float x, float z,int& row, int& col);  
  void initMeshVertices(); ///< Sets mesh vertices to a grid 
  void initMeshTriangles(); ///< Sets up the triangle list m_triangles
  /// \brief Calculates the normals of every triangle in the triangle list m_triangles
  void initTriangleNormals(); 
  /// \brief Calculates vertex normals based on triangle normals
  void initVertexNormals();
  /// \brief Calculates the index into the triangle list of the triangle that 
  /// is located at (x,z) in world space  
  int getTriangleIndex(float x, float z);  
  /// \brief Sets the Y coordinates of all vertices using m_pHeightMap.  It is
  /// assumed that a height map has already been loaded.
  void setTerrainFromHeightMap();   
  
};

#endif