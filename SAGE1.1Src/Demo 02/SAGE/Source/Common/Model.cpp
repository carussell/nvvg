/// File name:      Model.cpp
/// Last modified:  7/1/2005 6:53:31 PM

/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// Model.cpp - Model used for rendering
//
// Visit gamemath.com for the latest version of this file.
//
/////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "CommonStuff.h"
#include "Model.h"
#include "common/renderer.h"
#include "TriMesh.h"
#include "EditTriMesh.h"

/////////////////////////////////////////////////////////////////////////////
//
// class Model member functions
//
/////////////////////////////////////////////////////////////////////////////

Model::Model(BufferUsage bufferUsage)
{
	m_partCount = 0;
	m_partMeshList = NULL;
	m_partTextureList = NULL;
  m_nFrameCount = 0; //will be overridden in derived model classes

  m_vertexBuffer = NULL;
  m_indexBuffer  = NULL;
  m_bufferUsage = bufferUsage;
  m_totalVertices = 0;
  m_totalTris = 0;
}

Model::~Model() {
	freeMemory();
}

/// Allocates the part list.  The parts are not initialized with any
/// geometry.
/// \param nPartCount Specifies the number of parts.
void	Model::allocateMemory(int nPartCount) {

	// First, whack anything already allocated

	freeMemory();

	// Check if not allocating anything

	if (nPartCount < 1) {
		return;
	}

	// Allocate lists

	m_partMeshList = new TriMesh[nPartCount];
	m_partTextureList = new TextureReference[nPartCount];

	// Clear out texture list

	memset(m_partTextureList, 0, sizeof(m_partTextureList[0]) * nPartCount);

	// Remember number of parts

	m_partCount = nPartCount;
}

void	Model::freeMemory() {

	// Free arrays

	delete [] m_partMeshList;
	m_partMeshList = NULL;
	delete [] m_partTextureList;
	m_partTextureList = NULL;

  delete m_vertexBuffer;
  m_vertexBuffer = NULL;
  delete m_indexBuffer;
  m_indexBuffer = NULL;

	// Reset count

	m_partCount = 0;
}

/// \param index Specifies the index of the part to be fetched.
/// \return A pointer to the desired part.
TriMesh	*Model::getPartMesh(int index) {

	// Sanity check

	assert(index >= 0);
	assert(index < m_partCount);
	assert(m_partMeshList != NULL);

	// Return it

	return &m_partMeshList[index];
}

/// \param index Specifies the index of the part.
/// \return A pointer to the part's texture reference.
TextureReference *Model::getPartTexture(int index) {

	// Sanity check

	assert(index >= 0);
	assert(index < m_partCount);
	assert(m_partTextureList != NULL);

	// Return it

	return &m_partTextureList[index];
}

/// \param index Specifies the index of the part.
/// \param name Specifies the new name of the texture.
void	Model::setPartTextureName(int index, const char *name) {

	// Sanity check

	assert(index >= 0);
	assert(index < m_partCount);
	assert(m_partTextureList != NULL);

	// Copy in name
	//SECURITY-UPDATE:2/4/07
	//strcpy(m_partTextureList[index].name, name);
	strcpy_s(m_partTextureList[index].name,sizeof(m_partTextureList[index].name), name);
}

// Cache textures.  For best performance, always cache your textures
// before rendering

void	Model::cache() const {

	// Cache all textures

	for (int i = 0 ; i < m_partCount ; ++i) {
		gRenderer.cacheTexture(m_partTextureList[i]);
	}
}

void	Model::render() const
{
	// Render all the parts
	for (int i = 0 ; i < m_partCount ; ++i)
    renderPart(i);
}

/// \param vb Vertex buffer to render along with the static index buffer data.
void Model::render(VertexBufferBase *vb) const
{
	// Render all the parts
  for(int i=0; i<m_partCount; i++)
    renderPart(i, vb);
}

/// Renders the parts of the model using the curent 3D context.  This can
/// change the current texture.
/// \param index Specifies the index of the part to be rendered.
void	Model::renderPart(int index) const {

	// Sanity check

	assert(index >= 0);
	assert(index < m_partCount);
	assert(m_partMeshList != NULL);
	assert(m_partTextureList != NULL);

	// Select the texture

	gRenderer.selectTexture(m_partTextureList[index]);

	// Render the part
  gRenderer.render(
    m_vertexBuffer,
    0,
    m_vertexBuffer->getCount(),
    m_indexBuffer,
    m_indexOffsets[index],
    m_partMeshList[index].getTriCount());
}

/// Renders the parts of the model using the curent 3D context.  This can
/// change the current texture.
/// \param index Specifies the index of the part to be rendered.
/// \param vb Vertex buffer to render along with the static index buffer data.
void	Model::renderPart(int index, VertexBufferBase *vb) const {

	// Sanity check

	assert(index >= 0);
	assert(index < m_partCount);
	assert(m_partMeshList != NULL);
	assert(m_partTextureList != NULL);
  assert(vb != NULL);

	// Select the texture

	gRenderer.selectTexture(m_partTextureList[index]);

	// Render the part
  gRenderer.render(
    vb,
    0,
    vb->getCount(),
    m_indexBuffer,
    m_indexOffsets[index],
    m_partMeshList[index].getTriCount());
}

/// Convert an EditTriMesh to a Model.  Note that this function may need
/// to make many logical changes to the mesh, such as number of actual
/// parts, ordering of vertices, materials, faces, etc.  Faces may need
/// to be detached across part boundaries.  Vertices may need to be duplicated
/// to place UV's at the vertex level.  However, the actual mesh geometry will
/// not be modified as far as number of faces, vertex positions,
/// vertex normals, etc.  The input mesh is not modified, except for possibly
/// the mark fields.
/// \param mesh Specifies the mesh to be converted.
void	Model::fromEditMesh(EditTriMesh &mesh) {
	int	i;

	// Free up anything already allocated

	freeMemory();

	// Make sure something exists in the destination mesh

	if (mesh.partCount() < 1) {
		return;
	}

	// Extract the part meshes

	EditTriMesh	*partMeshes = new EditTriMesh[mesh.partCount()];
	mesh.extractParts(partMeshes);

	// Figure out how many parts we'll need.  Remember,
	// each of our parts must have a single material,
	// so we must duplicate parts for multiple materials.

	int	numParts = 0;
	for (i = 0 ; i < mesh.partCount() ; ++i) {
		numParts += partMeshes[i].materialCount();
	}

	// Allocate

	allocateMemory(numParts);

	// Convert each part

	int	destPartIndex = 0;

  m_totalVertices = 0;
  m_totalTris = 0;

	for (i = 0 ; i < mesh.partCount() ; ++i) {
		EditTriMesh *srcMesh = &partMeshes[i];
		for (int j = 0 ; j < srcMesh->materialCount() ; ++j) {

			// Get a mesh consisting of the faces
			// in this part that use this material

			EditTriMesh	onePartOneMaterial;
			srcMesh->extractOnePartOneMaterial(0, j, &onePartOneMaterial);

			// Sanity check the output mesh

			assert(onePartOneMaterial.vertexCount() > 0);
			assert(onePartOneMaterial.triCount() > 0);
			assert(onePartOneMaterial.partCount() == 1);
			assert(onePartOneMaterial.materialCount() == 1);

			// Convert the mesh to a trimesh

			getPartMesh(destPartIndex)->fromEditMesh(onePartOneMaterial);

			// Convert the material

			setPartTextureName(destPartIndex, onePartOneMaterial.material(0).diffuseTextureName);

      m_totalVertices += onePartOneMaterial.vertexCount();
      m_totalTris += onePartOneMaterial.triCount();

			// !FIXME! Need to implement part names!

			// Next destination part, please

			++destPartIndex;
		}
	}
	assert(destPartIndex == getPartCount());

  if(m_bufferUsage == StaticBuffers)
  {
    assert(m_vertexBuffer == NULL);
    assert(m_indexBuffer == NULL);

    int totalVc = 0;
    int totalTc = 0;
    for(i=0; i<m_partCount; i++)
    {
      totalVc += m_partMeshList[i].getVertexCount();
      totalTc += m_partMeshList[i].getTriCount();
    }

    m_vertexBuffer = new StandardVertexBuffer(totalVc);
    m_indexBuffer = new IndexBuffer(totalTc);

    int curVc = 0;
    int curTc = 0;

    m_vertexBuffer->lock();
    m_indexBuffer->lock();

    m_vertexOffsets.resize(m_partCount);
    m_indexOffsets.resize(m_partCount);

    for (i = 0 ; i < m_partCount ; ++i)
    {
      RenderVertex *srcV = m_partMeshList[i].getVertexList();
      RenderTri *srcT = m_partMeshList[i].getTriList();

      int vc = m_partMeshList[i].getVertexCount();
      int tc = m_partMeshList[i].getTriCount();

      for(int j=0; j<vc; j++)
      {
        (*m_vertexBuffer)[j+curVc].p = srcV[j].p;
        (*m_vertexBuffer)[j+curVc].n = srcV[j].n;
        (*m_vertexBuffer)[j+curVc].u = srcV[j].u;
        (*m_vertexBuffer)[j+curVc].v = srcV[j].v;
      }

      for(int j=0; j<tc; j++)
      {
        (*m_indexBuffer)[j+curTc].index[0] = srcT[j].index[0] + curVc;
        (*m_indexBuffer)[j+curTc].index[1] = srcT[j].index[1] + curVc;
        (*m_indexBuffer)[j+curTc].index[2] = srcT[j].index[2] + curVc;
      }

      m_vertexOffsets[i] = curVc;
      m_indexOffsets[i] = curTc;

      curVc += vc;
      curTc += tc;
	  }

    m_vertexBuffer->unlock();
    m_indexBuffer->unlock();
  }
  else if(m_bufferUsage == StaticIndexBuffer)
  {
    assert(m_indexBuffer == NULL);

    m_indexBuffer = new IndexBuffer(m_totalTris);
  }


	// Free uindividual part meshes

	delete [] partMeshes;
}

/// \param mesh Specifies the mesh to be replaced by this model.
/// \bug Not yet implemented.
void	Model::toEditMesh(EditTriMesh &mesh) const {
	// !FIXME!
	assert(false);
}

/// \param s3dFilename Specifies the name of the S3D file.
/// \param defaultDirectory wheter or not to load the model from the default
/// model directory
void	Model::importS3d(const char *s3dFilename, bool defaultDirectory) {

	char	text[256];

	// Load up the S3D into an EditTriMesh

	EditTriMesh editMesh;
	if (!editMesh.importS3d(s3dFilename, text, defaultDirectory)) {
		ABORT("Can't load %s.  %s.", s3dFilename, text);
	}

	// Optimize it for rendering

	editMesh.optimizeForRendering();

	// Convert it to renderable Model format

	fromEditMesh(editMesh);
}

/// \param m Specifies the transformation matrix applied to the model.
/// \return The bounding box of the model in transformed space.
AABB3 Model::getBoundingBox(const Matrix4x3 &m) const
{
  AABB3 bb;
  bb.empty();
  for(int i = 0; i < m_partCount; ++i)
    bb.add(m_partMeshList[i].getBoundingBox(m));
  return bb;
}

/// \param part Specifies the number of the part.
/// \return The bounding box of the part.
const AABB3 &Model::getPartBoundingBox(int part) const
{
  return m_partMeshList[part].getBoundingBox();
}

/// \param part Specifies the number of the part.
/// \param m Specifies the transformation matrix applied to the part.
/// \return The bounding box of the part in transformed space.
AABB3 Model::getPartBoundingBox(int part, const Matrix4x3 &m) const
{
  return m_partMeshList[part].getBoundingBox(m);
}