/// \file TriMesh.h
/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// TriMesh.h - Triangle mesh class for real-time use (rendering and
// collision detection)
//
// Visit gamemath.com for the latest version of this file.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __TRIMESH_H_INCLUDED__
#define __TRIMESH_H_INCLUDED__

#ifndef __AABB3_H_INCLUDED__
	#include "AABB3.h"
#endif

struct RenderVertex;
struct RenderTri;
class EditTriMesh;

/////////////////////////////////////////////////////////////////////////////
//
// TriMesh
//
// Triangle mesh class that can be rendered and collided with
//
/////////////////////////////////////////////////////////////////////////////

/// \brief Stores a triangular mesh in a format optimized for real-time use.
///
/// Stores a triangular mesh in a format optimized for real-time operations,
/// such as rendering and collision detection.
class TriMesh {
public:

	TriMesh();  ///< Constructs an empty mesh.
	~TriMesh();	///< Frees resources and destroys the mesh.

	// Memory allocation

	void	allocateMemory(int nVertexCount, int nTriCount);  ///< Allocates space for vertices and triangles.
	void	freeMemory();	///< Frees resources, deleting any mesh data.

	// Mesh accessors

  /// \brief Queries the mesh for the number of vertices contained.
  /// \return The number of vertices in the mesh.
	int		getVertexCount() const { return vertexCount; }
	
	/// \brief Queries the mesh for its vertex data.
	/// \return A pointer to the actual list of vertices.
	RenderVertex	*getVertexList() const { return vertexList; }
	
	/// \brief Queries the mesh for the number of triangles contained.
	/// \return The number of triangles in the mesh.
	int		getTriCount() const { return triCount; }
	
	/// \brief Queries the mesh for its triangle data.
	/// \return A pointer to the actual list of triangles.
	RenderTri	*getTriList() const { return triList; }

	// Rendering.  This will use the current 3D context.

	void	render() const;  ///< Renders the mesh.

	// Bounding box

	void		computeBoundingBox();  ///< Computes and internally stores the mesh's bounding box.
	
	/// \brief Queries the mesh for its bounding box.
	/// \return The last bounding box computed for the mesh.
	const AABB3	&getBoundingBox() const { return boundingBox; }

  AABB3 getBoundingBox(const Matrix4x3 &m) const;  ///< Queries the mesh for its bounding box, given a transformation matrix.

	// Conversion to/from an "edit" mesh.  Note that this class
	// doesn't know anything about parts or materials, so the
	// conversion is not an exact translation.

	void	fromEditMesh(const EditTriMesh &mesh);  ///< Sets this mesh to the equivalent of an edit mesh.
	void	toEditMesh(EditTriMesh &mesh) const;	///< Converts this mesh to an edit mesh.

  void moveVertices(Vector3 v); ///< Directly translates the vertices in the mesh by a given displacement vector.

protected:

	// Mesh data

	int		vertexCount;          ///< Specifies the number of vertices.
	RenderVertex	*vertexList;  ///< Contains the vertex data.
	int		triCount;             ///< Specifies the number of triangles.
	RenderTri	*triList;         ///< Contains the triangle data.
	
	AABB3	boundingBox;          ///< Stores the last computed bounding box.
	                            ///< Must be recomputed if the vertex list
	                            ///< is modified.
};

/////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __TRIMESH_H_INCLUDED__
