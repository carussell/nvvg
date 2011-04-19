/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// ArticulatedModel.h - Model used for rendering by part
//
// Visit gamemath.com for the latest version of this file.
//
/////////////////////////////////////////////////////////////////////////////

/// \file ArticulatedModel.h
/// \brief Interface for the ArticulatedModel class.

#pragma once

#include "common/model.h"
#include "common/vector3.h"

class EditTriMesh;
class TriMesh;

/// \brief An encapsulation of a model with articulated submodels.
///
/// This class consists of a model that can contain submodels.  The submodels
/// get their parts from the main model, have their own origins, and can be
/// moved independently.

class ArticulatedModel : public Model {
private:
  int **m_nSubmodelPart;        ///< Contains the part data for each submodel
  int m_nSubmodelCount;         ///< Specifies the number of submodels
  int *m_nSubmodelPartCount;    ///< Specifies the number of parts in each
                                ///<     submodel
  int *m_nNextSubmodelPart;     ///< Specifies the next unused part in each
                                ///<     submodel
public:
  /// \brief Constructs a model with the given number of submodels.
  ArticulatedModel(int count);
  
  /// \brief Frees resources and destroys the model.
  ~ArticulatedModel();
  
  /// \brief Sets the number of parts in a given submodel.
  void setSubmodelPartCount(int nSubmodel,int count);
  
  /// \brief Adds a part to the submodel.
  void addPartToSubmodel(int nSubmodel,int nPart);
  
  /// \brief Adds a range of parts to the submodel.
  void addPartToSubmodel(int nSubmodel,int lower,int upper);
  
  /// \brief Renders a submodel.
  void renderSubmodel(int nSubmodel);
  
  /// \brief Moves a submodel by a given displacement.
  void moveSubmodel(int nSubmodel,const Vector3 &v);

  AABB3 getSubmodelBoundingBox(int submodel) const;  ///< Return the bounding box of a submodel.
  AABB3 getSubmodelBoundingBox(int submodel, const Matrix4x3 &m) const;  ///< Return the bounding box of a submodel given a world transformation.
};
