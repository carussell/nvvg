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

/// \file terrainsubmesh.cpp
#include "terrainsubmesh.h"

/// Creates a vertex buffer, an index buffer, and an array of vertices.
/// \param side Vertices per side
/// \param parentSide Vertices per side of the parent mesh.
/// \param lod Level of detail of the submesh.
/// \todo Fix description of parameter \p side
TerrainSubmesh::TerrainSubmesh(int vertsPerSide,int parentSide,int lod):
m_nSide(vertsPerSide),
m_nReducedSide(vertsPerSide>>lod),
m_nParentSide(parentSide),
m_nVPS(m_nReducedSide + 1),
m_nNumVertices(m_nVPS*m_nVPS),
m_nNumQuads(m_nReducedSide*m_nReducedSide),
m_nNumTriangles(2*m_nNumQuads),
m_nParentVerticesPerSide(parentSide+1),
m_lastLodCrack(-1)
{
  // vertex buffer to be filled with smoothed vertices at render time
  m_vertexBuffer = new VertexBuffer<RenderVertex>(m_nNumVertices,true);
  
  m_triangles = new IndexBuffer(m_nNumTriangles);
  
  // array of vertices that make up the unsmoothed submesh
  m_vertices = new RenderVertex[m_nNumVertices]; 
}

TerrainSubmesh::~TerrainSubmesh()
{
  delete m_vertexBuffer; m_vertexBuffer = NULL;
  delete [] m_vertices; m_vertices = NULL;
  delete m_triangles; m_triangles = NULL;
}


/// \param row Row that the submesh is in relative to the entire mesh.
/// \param col Column that the submesh is in relative to the entire mesh.
/// \param lod Level of detail of the submesh
/// \param v Mesh array.  The submesh vertices are extracted from this array.
void TerrainSubmesh::setMesh(int row, int col, int lod, RenderVertex *v)
{   
  // copy vertices passed in into the m_vertices array
  int nTopLeft = row * m_nSide * m_nParentVerticesPerSide + col * m_nSide;
  for(int i=0; i<m_nVPS; i++)
    for(int j=0; j<m_nVPS; j++)
      m_vertices[i*m_nVPS + j] = 
        v[ nTopLeft + (i<<lod) * m_nParentVerticesPerSide + (j<<lod)];
    
  // make sure the vertex buffer gets refilled next render
  m_lastLodCrack = -1;

  // fill in the index buffer with information on how to render the submesh
  m_triangles->lock();
  for (int i = 0 ; i < m_nVPS-1 ; i++) 
	  for (int j = 0 ; j < m_nVPS-1 ; j++) 
    {
      (*m_triangles)[i*(m_nVPS-1) + j].index[0] = i*m_nVPS+j;
      (*m_triangles)[i*(m_nVPS-1) + j].index[1] = (i+1)*m_nVPS + j + 1;
      (*m_triangles)[i*(m_nVPS-1) + j].index[2] = (i+1)*m_nVPS + j;
      (*m_triangles)[m_nNumQuads+i*(m_nVPS-1)+j].index[0] = i*m_nVPS + j; 
      (*m_triangles)[m_nNumQuads+i*(m_nVPS-1)+j].index[1] = i*m_nVPS + j + 1;
      (*m_triangles)[m_nNumQuads+i*(m_nVPS-1)+j].index[2] = (i+1)*m_nVPS + j + 1;
    }  
  m_triangles->unlock();
}

// renders submesh
/// \param lodcrack Combination of the LOD flags (see top of TerrainSubmesh.h)
/// that signify how to render the submesh.  For example, if the flag
/// LODCRACKTOP is set, a crack must be smoothed on the top side of the
/// submesh.
void TerrainSubmesh::render(unsigned int lodcrack)
{
  // if the vertex buffer needs to be changed
  if (lodcrack != m_lastLodCrack || m_vertexBuffer->isEmpty())
  {
  
    m_vertexBuffer->lock();

    // fill in all the vertices
    for (int x = 0; x < m_nNumVertices; x++)
      (*m_vertexBuffer)[x] = m_vertices[x];
        
    if(lodcrack & LODCRACKPRESENT)
    { 
      //there's a crack present
      //smooth out vertices on side with the crack

      if(lodcrack & LODCRACK_TOP){
        for(int i=1; i<m_nVPS; i+=2)
          (*m_vertexBuffer)[i].p.y = (m_vertices[i-1].p.y + m_vertices[i+1].p.y)/2.0f;
      }

      if(lodcrack & LODCRACK_RIGHT){
        for(int i=2*m_nVPS-1; i<m_nNumVertices; i+=2*m_nVPS)
          (*m_vertexBuffer)[i].p.y = (m_vertices[i-m_nVPS].p.y + m_vertices[i+m_nVPS].p.y)/2.0f;
      }

      if(lodcrack & LODCRACK_BOTTOM){
        for(int i=m_nVPS*(m_nVPS-1)+1; i<m_nNumVertices; i+=2)
          (*m_vertexBuffer)[i].p.y = (m_vertices[i-1].p.y + m_vertices[i+1].p.y)/2.0f;
      }

      if(lodcrack & LODCRACK_LEFT){
        for(int i=m_nVPS; i<m_nNumVertices-m_nVPS; i+=2*m_nVPS)
          (*m_vertexBuffer)[i].p.y = (m_vertices[i-m_nVPS].p.y + m_vertices[i+m_nVPS].p.y)/2.0f;
      }
    }

    m_vertexBuffer->unlock();
  }
  
  gRenderer.render(m_vertexBuffer, m_triangles); //render geometry

  // record what information has been put in the vertex buffer for next time
  // that way we will know if the vertex buffer needs to be refilled next 
  // render.
  m_lastLodCrack = lodcrack;
}



