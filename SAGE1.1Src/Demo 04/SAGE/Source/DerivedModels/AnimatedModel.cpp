/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// AnimatedModel.cpp - Model used for rendering stillframe animations
//
// Visit gamemath.com for the latest version of this file.
//
/////////////////////////////////////////////////////////////////////////////

/// \file AnimatedModel.cpp
/// \brief Code for the AnimatedModel class.

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "animatedmodel.h"
#include "common/TriMesh.h"
#include "common/EditTriMesh.h"
#include "common/renderer.h"
#include "common/CommonStuff.h"

bool AnimatedModel::m_bModelLerp = true; //true for linear interpolation of model frames

/// \param framecount Specifies the number of submodels.
/// \param animationcount Specifies the number of animation sequences.
AnimatedModel::AnimatedModel(int framecount, int animationcount)
: Model(StaticIndexBuffer)
{
  m_nFrameCount = framecount % 100; //this is declared in the base class Model

  //create models in frame array
  m_pModelArray = new Model*[m_nFrameCount];
  for(int i = 0; i < m_nFrameCount; i++)
    m_pModelArray[i] = new Model(NoBuffers);

  //create animation sequences
  if(animationcount<=0){ //default animation sequence

    //create faux single animation sequence
    m_nNumAnimations = 1; //one animation sequence
    m_nAnimationFrameCount = new int;
    m_nAnimationFrameCount[0] = m_nFrameCount;
    m_nAnimationFrame = new int*;
    m_nAnimationFrame[0] = new int[m_nFrameCount];

    //set animation sequence to default sequence 0,1,2, ...
    for(int i=0; i<m_nFrameCount; i++)
      m_nAnimationFrame[0][i] = i;

  }
  else{ //create space for scripted animation sequences

    m_nNumAnimations = animationcount;
    
    m_nAnimationFrameCount = new int[m_nNumAnimations];
    m_nAnimationFrame = new int*[m_nNumAnimations];
    for(int i=0; i<m_nNumAnimations; i++)
      m_nAnimationFrame[i] = NULL;

  }
  m_animation = 0;
  m_frame = 0.0f;
}

AnimatedModel::~AnimatedModel(){

  //delete models
  for(int i = 0; i < m_nFrameCount; i++)
    delete m_pModelArray[i];
  delete [] m_pModelArray;

  //delete animation frame sequences
  delete[] m_nAnimationFrameCount;
  for(int i=0; i<m_nNumAnimations; i++)
    delete [] m_nAnimationFrame[i];
  delete[] m_nAnimationFrame;
  
}


/// \param s3dFilename Specifies the base name of the S3D file.
/// \param defaultDirectory Specifies whether or not to load the model from the default
///     model directory
void AnimatedModel::importS3d(const char *s3dFilename, bool defaultDirectory) {
	char	text[256]; //text buffer

  //import animation frames
  for(int i = 0; i < m_nFrameCount; i++){
	  //SECURITY-UPDATE:2/3/07
	  //sprintf(text, "%s%c%c.s3d", s3dFilename, i/10 + '0', i%10 + '0'); //assumes at most 100 frames
	  sprintf_s(text,sizeof(text), "%s%c%c.s3d", s3dFilename, i/10 + '0', i%10 + '0'); //assumes at most 100 frames
	  m_pModelArray[i]->importS3d(text, defaultDirectory);
  }

  //copy first model over to local model for rendering

  m_partCount = m_pModelArray[0]->m_partCount;
	m_partMeshList = new TriMesh[m_partCount];
	m_partTextureList = new TextureReference[m_partCount];

  for(int i = 0; i < m_partCount; i++){ //for each part, do a deep copy of trimesh

    //allocate memory

    int vc = m_pModelArray[0]->m_partMeshList[i].getVertexCount(); //vertex count
    int tc = m_pModelArray[0]->m_partMeshList[i].getTriCount(); //triangle count
    m_partMeshList[i].allocateMemory(vc, tc); //allocate memory to copy into

    //copy vertex list

    RenderVertex *destV = m_partMeshList[i].getVertexList(); //destination for copy
    RenderVertex *srcV = m_pModelArray[0]->m_partMeshList[i].getVertexList(); //source for copy

    for(int j=0; j<vc; j++) //for each vertex
      destV[j] = srcV[j]; //copy vertex

    //copy triangle list

    RenderTri *destT = m_partMeshList[i].getTriList(); //destination for copy
    RenderTri *srcT = m_pModelArray[0]->m_partMeshList[i].getTriList(); //source for copy

    for(int j=0; j<tc; j++) //for each triangle
    {
      destT[j] = srcT[j]; //copy triangle
    }

    //copy texture list

	  m_partTextureList[i] = m_pModelArray[0]->m_partTextureList[i];
  }
}

/// \param s3dFilenames Contains the names of the S3D files, starting at frame 0.
/// \param defaultDirectory Specifies whether or not to load the model from the default
///     model directory
void AnimatedModel::importS3d(const std::list<const char *> &s3dFilenames, bool defaultDirectory)
{
  assert((int)(s3dFilenames.size()) >= m_nFrameCount);
  std::list<const char *>::const_iterator it = s3dFilenames.begin();
  for(int i = 0; i < m_nFrameCount; ++i)
  {
    m_pModelArray[i]->importS3d(*it, defaultDirectory);
    ++it;
  }
  m_totalTris = m_pModelArray[0]->m_totalTris;

  // verify consistent models
  int lastVc = m_pModelArray[0]->m_totalVertices;
  for(int i = 1; i < m_nFrameCount; ++i)
  {
    if(lastVc != m_pModelArray[i]->m_totalVertices)
    {
      m_isValid = false;
      return;
    }

    lastVc = m_pModelArray[i]->m_totalVertices;
  }

  m_isValid = true;
  assert(m_indexBuffer == NULL);
  m_indexBuffer = new IndexBuffer(m_totalTris);

  //copy first model over to local model for rendering

  m_partCount = m_pModelArray[0]->m_partCount;
	m_partMeshList = new TriMesh[m_partCount];
	m_partTextureList = new TextureReference[m_partCount];

  int totalTc = 0;
  int totalVc = 0;
  m_vertexOffsets.clear();
  m_indexOffsets.clear();

  if(!m_indexBuffer->lock())
    ABORT("AnimatedModel failed to lock index buffer");

  for(int i = 0; i < m_partCount; i++){ //for each part, do a deep copy of trimesh

    //allocate memory

    int vc = m_pModelArray[0]->m_partMeshList[i].getVertexCount(); //vertex count
    int tc = m_pModelArray[0]->m_partMeshList[i].getTriCount(); //triangle count
    m_partMeshList[i].allocateMemory(vc, tc); //allocate memory to copy into

    //copy vertex list

    RenderVertex *destV = m_partMeshList[i].getVertexList(); //destination for copy
    RenderVertex *srcV = m_pModelArray[0]->m_partMeshList[i].getVertexList(); //source for copy

    for(int j=0; j<vc; j++) //for each vertex
      destV[j] = srcV[j]; //copy vertex

    //copy triangle list
    RenderTri *srcT = m_pModelArray[0]->m_partMeshList[i].getTriList(); //source for copy

    for(int j=0; j<tc; j++) //for each triangle
    {
      (*m_indexBuffer)[j+totalTc].index[0] = srcT[j].index[0] + totalVc; //copy triangle
      (*m_indexBuffer)[j+totalTc].index[1] = srcT[j].index[1] + totalVc; //copy triangle
      (*m_indexBuffer)[j+totalTc].index[2] = srcT[j].index[2] + totalVc; //copy triangle
    }

    m_vertexOffsets.push_back(totalVc);
    m_indexOffsets.push_back(totalTc);
    totalTc += tc;
    totalVc += vc;

    //copy texture list

	  m_partTextureList[i] = m_pModelArray[0]->m_partTextureList[i];
  }
  
  m_totalVertices = totalVc;

  m_indexBuffer->unlock();
}

/// \param vb Vertex buffer to be rendered.
void AnimatedModel::render(VertexBufferBase *vb) const
{
  Model::render(vb);
}

/// \param seqno The sequence number in the list of sequences.
/// \param length The number of frames in the animation sequence.
/// \param sequence The frames in the animation sequence
void AnimatedModel::setAnimationSequence(int seqno, int length, int* sequence){
  if( seqno < 0 || seqno >= m_nNumAnimations ) return; //bail if wrong sequence number
  
  //create space for animation sequence
  m_nAnimationFrameCount[seqno] = length; //record length
  m_nAnimationFrame[seqno] = new int[length]; //create space for sequence
  for(int i=0; i<length; i++)
    m_nAnimationFrame[seqno][i] = sequence[i]; //copy sequence
}

/// \param seqno The sequence number in the list of sequences.
/// \param sequence The frames in the animation sequence
void AnimatedModel::setAnimationSequence(int seqno, const std::list<int> &sequence){
  if(seqno < 0 || seqno >= m_nNumAnimations) return; // bail if bad sequence number
  
  int length = (int)sequence.size();
  m_nAnimationFrameCount[seqno] = length; //record length;
  m_nAnimationFrame[seqno] = new int[length];
  std::list<int>::const_iterator it = sequence.begin();
  for(int i=0; i < length; ++i)
  {
    m_nAnimationFrame[seqno][i] = *it;
    ++it;
  }
}

/// \param frame Specifies the frame number as a fractional value for interpolation.
/// \param animation Specifies the animation sequence to be run.
/// \param vb Specifies the vertex buffer to be filled with interpolated data.
void AnimatedModel::selectAnimationFrame(float frame, int animation, StandardVertexBuffer &vb)
{
  selectAnimationFrame(frame, animation, vb, NULL, NULL);
}

/// \param frame Specifies the frame number as a fractional value for interpolation.
/// \param animation Specifies the animation sequence to be run.
/// \param vb Specifies the vertex buffer to be filled with interpolated data.
/// \param boundingBox Specifies the bounding box to be updated.
/// \param world Specifies the world (or other parent) transform of the model.
void AnimatedModel::selectAnimationFrame(float frame, int animation, StandardVertexBuffer &vb, AABB3 &boundingBox, const Matrix4x3 &world)
{
  selectAnimationFrame(frame, animation, vb, &boundingBox, &world);
}

/// \param frame Specifies the frame number as a fractional value for interpolation.
/// \param animation Specifies the animation sequence to be run.
/// \param vb Specifies the vertex buffer to be filled with interpolated data.
/// \param boundingBox If non-NULL, specifies the bounding box to be updated.
/// \param world If non-NULL, specifies the world (or other parent) transform of the model.
void AnimatedModel::selectAnimationFrame(float frame, int animation, StandardVertexBuffer &vb, AABB3 *boundingBox, const Matrix4x3 *world)
{
  if(boundingBox != NULL)
  {
    assert(world != NULL);
    boundingBox->empty();
  }    

  //set vertex positions and normals from model frames, lerping if necessary

  if(animation < 0) animation = 0;
  if(animation >= m_nNumAnimations) animation = m_nNumAnimations-1;

  if (!m_bModelLerp) frame = (float) ((int)frame); //cancel out lerping

  int prevFrameIndex = (int)frame; //integer part of previous frame index
  float fraction = frame - (float)prevFrameIndex; //fraction between frames
  int prevFrame = m_nAnimationFrame[animation][ prevFrameIndex % m_nAnimationFrameCount[animation]]; //previous frame
  int nextFrame = m_nAnimationFrame[animation][
    ( prevFrameIndex + 1 ) % m_nAnimationFrameCount[animation]]; //next frame 

  if(!vb.lock())
    ABORT("AnimatedModel failed to lock vertex buffer");

  int totalVc = 0;
  
  for(int i = 0; i < m_partCount; i++){ //for each part, do a deep copy of trimesh
    
    int vc = m_partMeshList[i].getVertexCount(); //vertex count
  
    RenderVertex *srcV1 = m_pModelArray[prevFrame]->m_partMeshList[i].getVertexList(); //source 1
    RenderVertex *srcV2 = m_pModelArray[nextFrame]->m_partMeshList[i].getVertexList(); //source 2

     //compute weighted average of vertex positions
    for(int j=0; j<vc; j++)
    { //for each vertex
      Vector3 pos = (1.0f - fraction) * srcV1[j].p + fraction * srcV2[j].p;
      vb[j+totalVc].p = pos;
      vb[j+totalVc].n = (1.0f - fraction) * srcV1[j].n + fraction * srcV2[j].n;
      vb[j+totalVc].u = srcV1[j].u;
      vb[j+totalVc].v = srcV1[j].v;
      
      // Update bounding box
      
      if(boundingBox != NULL)
        boundingBox->add(pos * (*world));
    }
    totalVc += vc;
  }

  vb.unlock();

  m_animation = animation;
  m_frame = frame;
}

/// \return The number of frames in the current animation.  
int AnimatedModel::numFramesInAnimation() const
{
  return m_nAnimationFrameCount[m_animation];
}

/// \note The caller will be responsible for deleting the allocated vertex buffer
/// \return A new dynamic vertex buffer of the proper size
StandardVertexBuffer *AnimatedModel::getNewVertexBuffer()
{
  StandardVertexBuffer *vb = new StandardVertexBuffer(m_totalVertices, true);

  return vb;
}