/// \file Model.h
/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// Model.h - Model used for rendering
//
// Visit gamemath.com for the latest version of this file.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __MODEL_H_INCLUDED__
#define __MODEL_H_INCLUDED__

#include "graphics/VertexBuffer.h"
#include "graphics/VertexTypes.h"
#include "graphics/IndexBuffer.h"
#include <vector>

// Forward declarations

class EditTriMesh;
class TriMesh;
struct TextureReference;

class Matrix4x3;
class AABB3;

/////////////////////////////////////////////////////////////////////////////
//
// class Model
//
// Renderable model class.  This class consists of a list of "parts."
// Each part is a triangle mesh and can have its own texture.
//
/////////////////////////////////////////////////////////////////////////////

/// \brief An encapsulation of a renderable model.
///
/// This class consists of a list of "parts."  Each part is a triangle mesh
/// and can have its own texture.
class Model
{
  friend class AnimatedModel;
  typedef std::vector<int> PartOffsetArray;

public:
  /// \brief Type of usage for the model's vertex buffer
  enum BufferUsage
  {
    NoBuffers,
    StaticIndexBuffer,
    StaticBuffers
  };

	Model(BufferUsage bufferUsage = StaticBuffers);  ///< Constructs an empty model.
	~Model();  ///< Frees allocated resources and destroys the model.

	// Memory allocation

	void	allocateMemory(int nPartCount);  ///< Allocates the part list.
	void	freeMemory();  ///< Free any resources and reset variables to empty state.

	// Part accessors

	/// \brief Queries the model for the number of parts.
	/// \return The number of parts in the model.
	int	getPartCount() const { return m_partCount; }

	TriMesh	*getPartMesh(int index);  ///< Queries the model for one of its parts.

	// Texture accessors

	TextureReference	*getPartTexture(int index);  ///< Queries the model for the texture of one of its parts
	void			setPartTextureName(int index, const char *name);  ///< Sets the texture name for a part.

	// Cache textures.  For best performance, always cache your textures
	// before rendering

	void	cache() const;  ///< Caches the textures used by the model.

	// Render the entire model, or a single part.  This will use
	// the current 3D context.  The current texture will be changed.

  /// Renders the parts of the model using the curent 3D context.  This can
  /// change the current texture.
  virtual void render() const;	///< Renders the parts of the model using the curent 3D context.
  virtual void render(VertexBufferBase *vb) const;  ///< Renders the parts of the model using the curent 3D context.
  void renderPart(int index) const;  ///< Renders the parts of the model using the curent 3D context.
  void renderPart(int index, VertexBufferBase *vb) const;  ///< Renders the parts of the model using the curent 3D context.


	// Conversion to/from an "edit" mesh

	void	fromEditMesh(EditTriMesh &mesh);  ///< Converts an EditTriMesh to a Model.
	void	toEditMesh(EditTriMesh &mesh) const;  ///< Converts the model to an EditTriMesh.

	// Shorthand for importing an S3D.  (Uses EditTriMesh)

	void	importS3d(const char *s3dFilename, bool defaultDirectory = true);  ///< Imports a model from an S3D file (.S3D).

  AABB3 getBoundingBox(const Matrix4x3 &m) const;  ///< Queries a model for its bounding box.
  const AABB3 &getPartBoundingBox(int part) const;  ///< Queries a model for the bounding box of one of its parts.
  AABB3 getPartBoundingBox(int part, const Matrix4x3 &m) const;  ///< Queries a model for the bounding box of one of its parts.

protected:

	// Parts and textures

  int m_partCount;                     ///< Specifies the number of parts
  TriMesh *m_partMeshList;             ///< Contains the part data
  TextureReference *m_partTextureList; ///< Contains the texture references 
  int m_nFrameCount;                   ///< Number of frames for multi-frame models

  PartOffsetArray m_vertexOffsets;
  PartOffsetArray m_indexOffsets;

  int m_totalVertices;
  int m_totalTris;

  StandardVertexBuffer *m_vertexBuffer;
  IndexBuffer *m_indexBuffer;
  BufferUsage m_bufferUsage;
};

/////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __MODEL_H_INCLUDED__
