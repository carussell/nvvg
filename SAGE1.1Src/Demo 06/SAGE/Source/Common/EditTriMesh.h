/// \file EditTriMesh.h
/// \brief Interface for the EditTriMesh class.

/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// EditTriMesh.h - Declarations for class EditTriMesh
//
// Visit gamemath.com for the latest version of this file.
//
// For more details, see EditTriMesh.cpp
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __EDITTRIMESH_H_INCLUDED__
#define __EDITTRIMESH_H_INCLUDED__

#ifndef __VECTOR3_H_INCLUDED__
	#include "Vector3.h"
#endif

class Matrix4x3;
class AABB3;

/// \brief Store an indexed triangle mesh in a very flexible format that makes
/// editing and mesh manipulations easy.
/// \note This class supports texture mapping coordinates and vertex normals
/// \note This class is NOT optimized for rendering, collision detection,
/// or anything else.
class EditTriMesh
{
public:

// Local types

	/// \brief Represents the information we keep track of for one vertex
  /// \note Although we store the UV coordinates and normals here,these may
  /// be invalid at various times.  The "real" UVs are in the triangles.  
  /// For rendering, we often need UV's at the vertex level.  But for 
  /// many other optimizations, we may need to weld vertices for faces
  /// with different UV's.
	class Vertex
  {
	public:
    /// Basic constructor
		Vertex() { setDefaults(); }

    void	setDefaults(); ///< Sets the member variables to default values
		
		Vector3	p; ///< 3D vertex position
		Vector3	normal; ///< Vertex-level surface normal

		float	u; ///< Vertex-level texture mapping coordinate
    float v; ///< Vertex-level texture mapping coordinate

		int	mark; ///< Utility "mark" variable, often handy
	};
  //-------------------------------------------------------------------------

  //-------------------------------------------------------------------------
	/// \brief Represents the information we keep track of for one triangle
	class Tri
  {
	public:
    /// Basic constructor
		Tri() { setDefaults(); }

    void	setDefaults(); ///< Sets the member variables to default values

		/// \brief Face vertices
		struct Vert
    {
			int	index; ///< Index into the vertex list
      float	u; ///< Mapping coordinate
      float v; ///< Mapping coordinate
    };

    Vert	v[3]; ///< Array of vertex indices that form the triangle

    Vector3	normal; ///< Surface normal

    int	part; ///< Index of the part to which this triangle belongs
    int	material; ///< Index into the material list

    int	mark; ///< Utility "mark" variable, often handy

		bool	isDegenerate() const; ///< Tests whether the triangle is degenerate.
		int	findVertex(int vertexIndex) const;     ///< Finds the index (triangle based) of the vertex specified
	};
  //-------------------------------------------------------------------------

  //-------------------------------------------------------------------------
	/// \brief Represents the information we store for a material
	/// \remark We're only going to store a simple
	/// diffuse texture map.  However, more complex properties
	/// are often associated with materials.
	class Material
  {
	public:
    /// Basic constructor
		Material() { setDefaults(); }

		void	setDefaults(); ///< Sets the member variables to default values

		char	diffuseTextureName[256]; ///< Name of the texture used with this material

		int	mark; ///< Utility "mark" variable, often handy
	};
  //-------------------------------------------------------------------------

  //-------------------------------------------------------------------------
	/// \brief Represents the information we store for a part
	// We don't store much except the part name,
	// and a mark variable
	class Part
  {
	public:
    /// Basic constructor
		Part() { setDefaults(); }

    void	setDefaults(); ///< Sets the member variables to default values

		char	name[256]; ///< Name of the part

		int	mark; ///< Utility "mark" variable, often handy
	};
  //-------------------------------------------------------------------------

  //-------------------------------------------------------------------------
	/// \brief Contains options used to control optimization
	class OptimizationParameters
  {
	public:
    /// Basic constructor
		OptimizationParameters() { setDefaults(); }

    /// Sets the member variables to default values
		void	setDefaults();

		float	coincidentVertexTolerance; ///< A tolerance value which is used to
                                     ///< determine if two vertices are
                                     ///< coincident.

		float	cosOfEdgeAngleTolerance; ///< Triangle angle tolerance.
                                   ///< Vertices are not welded if they are on
                                   ///< an edge and the angle between the
                                   ///< normals of the triangles on this edge
                                   ///< are too far apart. We store the cosine
                                   ///< of this value since that's what's
                                   ///<actually used. Use the functions to set it

		void	setEdgeAngleToleranceInDegrees(float degrees); ///<Sets the edge angle tolerance
	};
  //-------------------------------------------------------------------------

  // Standard class object maintenance
	EditTriMesh(); ///< Basic constructor
	EditTriMesh(const EditTriMesh &x); ///< Copy constructor
	~EditTriMesh(); ///< Destructor

  //-------------------------------------------------------------------------
  /// \name Operators
  //@{
	EditTriMesh &operator=(const EditTriMesh &src); ///< Makes a copy of the mesh
  //@}
  //-------------------------------------------------------------------------

  //-------------------------------------------------------------------------
  /// \name Data access
  //@{
  /// \brief Returns the number of vertices in the mesh
  /// \return The number of vertices in the mesh
	int	vertexCount() const { return vCount; }

  /// \brief Returns the number of triangles in the mesh
  /// \return The number of triangles in the mesh
	int	triCount() const { return tCount; }

  /// \brief Returns the number of materials used in the mesh
  /// \return The number of materials used in the mesh
	int	materialCount() const { return mCount; }

  /// \brief Returns the number of parts in the mesh
  /// \return The number of parts in the mesh
  int	partCount() const { return pCount; }

  Vertex &vertex(int vertexIndex);   ///< Returns the specified vertex data
  const Vertex &vertex(int vertexIndex) const;   ///< Returns the specified vertex data
  Tri &tri(int triIndex);  ///< Returns the specified triangle data
  const Tri &tri(int triIndex) const;  ///< Returns the specified triangle data
  Material &material(int materialIndex);  ///< Returns the specified material data
  const Material &material(int materialIndex) const;  ///< Returns the specified material data
  Part &part(int partIndex);  ///< Returns the specified part data
  const Part &part(int partIndex) const;  ///< Returns the specified part data
  //@}
  //-------------------------------------------------------------------------

  //-------------------------------------------------------------------------
  /// \name Basic mesh operations
  /// Add or remove vertices, triangles, materials, or parts
  //@{
  void empty(); ///< Reset the mesh to empty state

  
  void setVertexCount(int newCount); ///< Sets the size of the vertex list
  void setTriCount(int newCount);  ///< Sets the size of the triangle list
  void setMaterialCount(int newCount); ///< Sets the size of the material list
  void setPartCount(int newCount); ///< Sets the size of the part list

  int addTri();   ///< Adds a triangle to the mesh
  int addTri(const Tri &t);   ///< Adds a triangle to the mesh

  int addVertex();   ///< Adds a vertex to the mesh
  int addVertex(const Vertex &v); ///< Adds a vertex to the mesh
  int dupVertex(int srcVertexIndex); ///< Adds a vertex to the mesh, duplicating an existing vertex
  int addMaterial(const Material &m);   ///< Adds a material to the mesh
  int addPart(const Part &p);   ///< Adds a part to the mesh
  void deleteVertex(int vertexIndex);  ///< Deletes a vertex from the mesh
  void deleteTri(int triIndex);  ///< Deletes a triangle from the mesh
  void deleteMarkedTris(int mark);  ///< Deletes all triangles from the mesh whose mark value matches the specified value
  void deleteDegenerateTris();  ///< Deletes all degenerated triangles from the mesh
  void deleteMaterial(int materialIndex);  ///< Deletes a material from the mesh
  void deleteUnusedMaterials();   ///< Deletes all materials in the mesh that are not being used
  void deletePart(int partIndex);  ///< Deletes a part from the mesh
  void deleteEmptyParts();   ///< Deletes all empty parts from the mesh
  //@}
  //-------------------------------------------------------------------------

  //-------------------------------------------------------------------------
  /// \name Marking
  /// Functions used to reset mark values
  //@{
  void markAllVertices(int mark);   ///< Reset all vertex mark values at once
  void markAllTris(int mark);   ///<Reset all triangle mark values at once
  void markAllMaterials(int mark);   ///< Reset all material mark values at once
  void markAllParts(int mark);   ///< Reset all part mark values at once
  //@}
  //-------------------------------------------------------------------------

  //-------------------------------------------------------------------------
  /// \name Extract data
  //@{
  void extractParts(EditTriMesh *meshes);   ///< Extract each part into a separate mesh.
  void extractOnePartOneMaterial(int partIndex, int materialIndex, EditTriMesh *result);   ///< Extract all triangles using a specific part and material

  //@}
  //-------------------------------------------------------------------------

  //-------------------------------------------------------------------------
  /// \name Misc
  //@{
  //
  void detachAllFaces();   ///< Detach all the faces from one another
  void transformVertices(const Matrix4x3 &m);   ///< Transform all the vertices
  //@}
  //-------------------------------------------------------------------------

  //-------------------------------------------------------------------------
  /// \name Computations
  //@{
  void computeOneTriNormal(int triIndex);   ///< Compute triangle-level surface normal
  void computeOneTriNormal(Tri &t);   ///< Compute triangle-level surface normal
  void computeTriNormals();   ///< Compute all triangle-level surface normals
  void computeVertexNormals();   ///< Compute vertex level surface normals.
  AABB3 computeBounds() const;   ///< Compute the size of the mesh
  //@}
  //-------------------------------------------------------------------------

  //-------------------------------------------------------------------------
  /// \name Optimization
  //@{
  void optimizeVertexOrder(bool removeUnusedVertices = true);   ///< Order the vertex list in the order that they are used by the faces
  void	sortTrisByMaterial();   ///< Sort triangles by material
  void	weldVertices(const OptimizationParameters &opt);   ///< Weld coincident vertices
  void copyUvsIntoVertices();   ///< Ensure that the vertex UVs are correct, possibly duplicating vertices if necessary
  void optimizeForRendering();   ///< Do all of the optimizations
  //@}
  //-------------------------------------------------------------------------

  //-------------------------------------------------------------------------
  /// \name Import/Export S3D format
  //@{
  bool importS3d(const char *filename, char *returnErrMsg,size_t errMsgSize, bool defaultDirectory = true);   ///< Import a mesh from an .s3d file
  void exportS3d(const char *filename);   ///< Export a mesh to an .s3d file
  //@}
  //-------------------------------------------------------------------------

  //-------------------------------------------------------------------------
  /// \name Debugging
  //@{
  void validityCheck();   ///< Check the validity of the mesh

  bool validityCheck(char *returnErrMsg);   ///< Check the validity of the mesh
  //@}
  //-------------------------------------------------------------------------

// Private representation

private:

	// The mesh lists

  int vAlloc; ///< Number of vertices allocated
  int vCount; ///< Number of vertices being used
  Vertex		*vList; ///< Pointer to the vertex list

  int tAlloc; ///< Number of triangles allocated
  int tCount; ///< Number of triangles being used
  Tri *tList; ///< Pointer to the triangle list

  int mCount; ///< Number of materials being used
  Material *mList; ///< Pointer to the material list

  int pCount; ///< Number of parts in the mesh
  Part *pList; ///< Pointer to the part list

// Implementation details:

  void construct();   ///< Does the real work of the constructor
};
//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __EDITTRIMESH_H_INCLUDED__

