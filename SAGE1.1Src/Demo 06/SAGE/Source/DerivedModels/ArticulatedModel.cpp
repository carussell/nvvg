/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// ArticulatedModel.cpp - Model used for rendering by part
//
// Visit gamemath.com for the latest version of this file.
//
/////////////////////////////////////////////////////////////////////////////

/// \file ArticulatedModel.cpp
/// \brief Code for the ArticulatedModel class.

#include <assert.h>
#include <stdlib.h>

#include "articulatedmodel.h"
#include "common/TriMesh.h"
#include "common/EditTriMesh.h"

/// \param count Specifies the number of submodels.
ArticulatedModel::ArticulatedModel(int count)
: m_nSubmodelCount(count){
  m_nSubmodelPartCount = new int[count];
  m_nNextSubmodelPart = new int[count];
  m_nSubmodelPart = new int* [count];
  for(int i = 0; i < count; i++){ 
    m_nSubmodelPart[i] = NULL;
    m_nSubmodelPartCount[i] = 0;
    m_nNextSubmodelPart[i] = 0;
  }
}

ArticulatedModel::~ArticulatedModel(){
  delete [] m_nSubmodelPartCount;
  delete [] m_nNextSubmodelPart;
  for(int i = 0; i < m_nSubmodelCount; i++) 
    delete [] m_nSubmodelPart[i];
  delete [] m_nSubmodelPart;
}

  
/// \param nSubmodel Specifies the index of the submodel.
/// \param count Specifies the number of parts.
void ArticulatedModel::setSubmodelPartCount(int nSubmodel,int count){ 
  // set number of parts in submodel

  if (nSubmodel >= m_nSubmodelCount) return; // bail out if bad index

  // check to see if already used
  if ( m_nSubmodelPart[nSubmodel] && m_nSubmodelPartCount[nSubmodel] != count) 
    delete [] m_nSubmodelPart[nSubmodel]; // clean up if already used

  // go ahead and set it
  m_nSubmodelPartCount[nSubmodel] = count; //record number of parts
  m_nSubmodelPart[nSubmodel] = new int[count]; // grab enough memory for parts list
}

/// \param nSubmodel Specifies the index of the submodel.
/// \param nPart Specifies the index of the part in the main model.
void ArticulatedModel::addPartToSubmodel(int nSubmodel,int nPart){ // add single part
  
  //consistency check
  if (nSubmodel >= m_nSubmodelCount) return; // bail out if bad index
  if(m_nNextSubmodelPart[nSubmodel] >= m_nSubmodelPartCount[nSubmodel]) return; //bail if no room

  // go ahead and add the part
  m_nSubmodelPart[nSubmodel][m_nNextSubmodelPart[nSubmodel]] = nPart;
  ++m_nNextSubmodelPart[nSubmodel];
}

/// \param nSubmodel Specifies the index of the submodel.
/// \param lower Specifies the lower index of the part in the main model.
/// \param upper Specifies the upper index of the part in the main model.
void ArticulatedModel::addPartToSubmodel(int nSubmodel,int lower,int upper){ //add range of parts
  for(int i=lower; i<=upper; i++)
    addPartToSubmodel(nSubmodel,i);
}

/// \param nSubmodel Specifies the index of the submodel.
void ArticulatedModel::renderSubmodel(int nSubmodel){ //render submodel from parts
  if (nSubmodel >= m_nSubmodelCount) return; // bail out if bad index
  for(int i = 0; i < m_nNextSubmodelPart[nSubmodel]; i++ ) //for i'th part
    renderPart(m_nSubmodelPart[nSubmodel][i]); //render i'th part
}

/// \param nSubmodel Specifies the index of the submodel.
/// \param v Specifies the vector of displacement.
void ArticulatedModel::moveSubmodel(int nSubmodel,const Vector3 &v){ //move origin of submodel
  if (nSubmodel >= m_nSubmodelCount) return; // bail out if bad index

  //EditTriMesh editMesh;
  for(int i = 0; i < m_nNextSubmodelPart[nSubmodel]; i++ )
  {
    TriMesh	*t = &m_partMeshList[m_nSubmodelPart[nSubmodel][i]];
    //t->toEditMesh(editMesh);
    t->moveVertices(v);

    if(m_vertexBuffer == NULL)
      continue;

    int partID = m_nSubmodelPart[nSubmodel][i];
    int offset = m_vertexOffsets[partID];

    m_vertexBuffer->lock();

    for(int i=0; i < t->getVertexCount(); i++)
    {
      (*m_vertexBuffer)[offset+i].p = t->getVertexList()[i].p;
    }

    m_vertexBuffer->unlock();
  }
}

/// \param submodel Specifies the submodel whose bounding box is to be fetched.
AABB3 ArticulatedModel::getSubmodelBoundingBox(int submodel) const
{
  assert(submodel < m_nSubmodelCount);
  AABB3 bb;
  bb.empty();
  for(int i = 0; i < m_nNextSubmodelPart[submodel]; ++i)
    bb.add(m_partMeshList[m_nSubmodelPart[submodel][i]].getBoundingBox());
  return bb;
}

/// \param submodel Specifies the submodel whose bounding box is to be fetched.
/// \param m Specifies the transformation matrix representing the submodel in world space.
AABB3 ArticulatedModel::getSubmodelBoundingBox(int submodel, const Matrix4x3 &m) const
{
  assert(submodel < m_nSubmodelCount);
  AABB3 bb;
  bb.empty();
  for(int i = 0; i < m_nNextSubmodelPart[submodel]; ++i)
    bb.add(m_partMeshList[m_nSubmodelPart[submodel][i]].getBoundingBox(m));
  return bb;
}