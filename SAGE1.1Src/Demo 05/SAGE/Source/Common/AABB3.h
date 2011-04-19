/// \file AABB3.h
/////////////////////////////////////////////////////////////////////////////
///
/// 3D Math Primer for Games and Graphics Development
///
/// AABB3.h - Declarations for class AABB3
///
/// Visit gamemath.com for the latest version of this file.
///
/// For more details, see AABB3.cpp
///
/////////////////////////////////////////////////////////////////////////////

#ifndef __AABB3_H_INCLUDED__
#define __AABB3_H_INCLUDED__

#ifndef __VECTOR3_H_INCLUDED__
	#include "Vector3.h"
#endif

class Matrix4x3;

/// \brief Implements a 3D axially-aligned bounding box.
class AABB3 {
public:

// Public data

	Vector3	min; ///< Specifies the minimum corner of the box.
	Vector3	max; ///< Specifies the maximum corner of the box.

	/// \brief Queries the box for its size along each dimension.
	/// \return A vector containing the size of the box.
	Vector3	size() const { return max - min; }

	/// \brief Queries the box for its width (its size in the x-dimension).
	/// \return The width of the box.
	float	xSize() { return max.x - min.x; }

	/// \brief Queries the box for its height (its size in the y-dimension).
	/// \return The height of the box.
	float	ySize() { return max.y - min.y; }

  /// \brief Queries the box for its depth (its size in the z-dimension).
  /// \return The depth of the box.
	float	zSize() { return max.z - min.z; }

	/// \brief Queries the box for its center point
	/// \return A vector indicating the center of the box.
	Vector3	center() const { return (min + max) * .5f; }

  /// \brief Queries the box for one of its corner points.
	Vector3	corner(int i) const;

  // Box operations

  /// \brief Sets the box to empty.
	void	empty();

  /// \brief Adds a point to the box, expanding the box if necessary.
	void	add(const Vector3 &p);

  /// \brief Adds an AABB to the box, expanding the box if necessary.
	void	add(const AABB3 &box);

  /// \brief Transforms the box and computes a new AABB.
	void	setToTransformedBox(const AABB3 &box, const Matrix4x3 &m);

  // Containment/intersection tests

  /// \brief Queries the box to see if it is empty.
	bool	isEmpty() const;

  /// \brief Performs an AABB-point intersect test on this box.
	bool	contains(const Vector3 &p) const;

  /// \brief Performs a closest-point test on this box.
	Vector3	closestPointTo(const Vector3 &p) const;

  /// \brief Performs an AABB-sphere intersect test on this box.
	bool  intersectsSphere(const Vector3 &center, float radius) const;

	// Parametric intersection with a ray.  Returns >1 if no intresection

  /// \brief Performs a parametric AABB-ray intersection test on this box.
	float	rayIntersect(const Vector3 &rayOrg, const Vector3 &rayDelta,
		Vector3 *returnNormal = 0) const;

	/// \brief Classifies a box as being on one side or the other of a plane
	int	classifyPlane(const Vector3 &n, float d) const;

  /// \brief Performs a dynamic AABB-plane intersection test.
	float	intersectPlane(const Vector3 &n, float planeD,
		const Vector3 &dir) const;

  /// \brief Performs an AABB-AABB intersection test.
  static bool intersect(const AABB3 &box1, const AABB3 &box2,
	  AABB3 *boxIntersect = 0);

  /// \brief Performs a dynamic AABB-AABB intersection test with one moving box.
  static float	intersectMoving(
	  const AABB3 &stationaryBox,
	  const AABB3 &movingBox,
	  const Vector3 &d);
	  
	/// \brief Performs a dynamic AABB-AABB intersection test with two moving boxes.
	static float intersectMoving(
	  const AABB3 &box1,
	  const AABB3 &box2,
	  const Vector3 &d1,
	  const Vector3 &d2
	);
};

/////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __AABB3_H_INCLUDED__
