/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// AnimatedModel.h - Model used for rendering stillframe animations
//
// Visit gamemath.com for the latest version of this file.
//
/////////////////////////////////////////////////////////////////////////////

/// \file AnimatedModel.h
/// \brief Interface for the AnimatedModel class.

#pragma once

#include <list>
#include "graphics/VertexTypes.h"
#include "common/model.h"
#include "common/vector3.h"

class EditTriMesh;
class TriMesh;

/// \brief An encapsulation of a model with multiple frames.
///
/// This class consists of a model that can contain multiple frames of animation.
/// It assumes that each frame is read in as a separate model, with the same
/// vertex count, triangle count, and texture for each frame.

class AnimatedModel : public Model {

public:
  static bool m_bModelLerp; ///< Global flag; specifies whether models should be linearly interpolated.
  
  /// \brief Constructs a model with the given number of submodels.
  AnimatedModel(int framecount, int animationcount=0);

  /// \brief Frees resources and destroys the model.
  ~AnimatedModel();

  /// \brief Imports a model from several S3D files (.S3D).
	void importS3d(const char *s3dFilenamePrefix, bool defaultDirectory = true);

  /// \brief Imports a model from several S3D files.	
	void importS3d(const std::list<const char *> &s3dFilenames, bool defaultDirectory = true);

  /// \brief Render animation.
  void render(VertexBufferBase *vb) const;
  
  /// \brief Stores a sequence of frame numbers for an animation.
  void setAnimationSequence(int seqno, int length, int* sequence);
  
  /// \brief Stores a sequence of frame numbers for an animation.
  void setAnimationSequence(int seqno, const std::list<int> &sequence);

  /// \brief Select the current animation sequence and frame, updating a supplied vertex buffer.
  void selectAnimationFrame(float frame, int animation, StandardVertexBuffer &vb);
  
  /// \brief Select the current animation sequence and frame, updating a supplied vertex buffer and bounding box.
  void selectAnimationFrame(float frame, int animation, StandardVertexBuffer &vb, AABB3 &boundingBox, const Matrix4x3 &world);

  /// \brief Queries the model for the number of frames in the current animation.
  int numFramesInAnimation() const;

  /// \brief Creates a dynamic vertex buffer of the proper size.
  StandardVertexBuffer *getNewVertexBuffer();

private:
  /// \brief Utility function.
  void selectAnimationFrame(float frame, int animation, StandardVertexBuffer &vb, AABB3 *boundingBox, const Matrix4x3 *world);

  Model** m_pModelArray; ///< Array of pointers to models for frames.
  int m_nNumAnimations; ///< Number of animations.
  int** m_nAnimationFrame; ///< Animation frames for each behaviour.
  int* m_nAnimationFrameCount; ///< Number of frames in each animation.
  int m_animation;  ///< Specifies currently selected animation sequence.
  float m_frame; ///< Specifies current animation frame.

};
