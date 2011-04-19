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

/// \file AnimatedModel.h
/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// AnimatedModel.h - Model used for rendering stillframe animations
//
// Visit gamemath.com for the latest version of this file.
//
/////////////////////////////////////////////////////////////////////////////

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
