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

/// \file TerrainSubmesh.h
/// \brief Interface for the TerrainSubmesh class.

#ifndef __TERRAINSUBMESH_H_INCLUDED__
#define __TERRAINSUBMESH_H_INCLUDED__

#include "graphics/VertexBuffer.h"
#include "graphics/IndexBuffer.h"
#include "Common/Renderer.h"
#include "graphics/VertexTypes.h"
#include "TerrainVertex.h"

const unsigned int LOD0 = 0x01;
const unsigned int LOD1 = 0x02;
const unsigned int LOD2 = 0x03;
const unsigned int LOD_DRAW = 0x03;
const unsigned int LODCRACK_TOP = 0x04;
const unsigned int LODCRACK_RIGHT = 0x8;
const unsigned int LODCRACK_BOTTOM = 0x10;
const unsigned int LODCRACK_LEFT = 0x20;
const unsigned int LODCRACKPRESENT = 0x3C;


//-----------------------------------------------------------------------------
/// \class TerrainSubmesh
/// \brief Holds a square mesh that makes up a part of the entire terrain.
class TerrainSubmesh
{
public:
  TerrainSubmesh(int vertsPerSide,int parentSide,int lod); ///< Basic Constructor
  ~TerrainSubmesh(); ///< Basic Destructor

  /// \brief Sets vertices for the submesh.
  void setMesh(int row, int col, int lod, TerrainVertex *v); //set mesh from parent
  
  /// \brief Renders the submesh
  void render(unsigned int lodcrack = 0);

private:
  int m_nSide; ///< Number of quads per side
  int m_nReducedSide; ///< Number of quads per side after lod
  int m_nParentSide; ///< Number of quads per side in parent
  int m_nVPS; /// Number of vertices per side
  int m_nParentVerticesPerSide; ///< Vertices per side of the parent grid
  int m_nNumVertices; ///< Total number of vertices
  int m_nNumQuads; ///< Total number of quads
  int m_nNumTriangles; ///< Total number of triangles

  unsigned int m_lastLodCrack;   ///< Records how the submesh was rendered last render
  
  VertexBuffer<TerrainVertex> *m_vertexBuffer; ///<Holds all the vertices to be rendered
  TerrainVertex* m_vertices; ///< Vertices before cracks have been smoothed
  IndexBuffer *m_triangles; ///< Static index buffer used to render submesh
  

};

#endif
