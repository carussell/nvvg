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

/// \file Reflection.h
#ifndef H_REFLECTION_H
#define H_REFLECTION_H

#include "common/eulerangles.h"
#include "common/vector3.h"
#include "common/plane.h"

//-----------------------------------------------------------------------------
/// \brief Controls rendering planar reflections.
//
/// This class is used to render planar reflections.  The water class uses this
/// class for reflections.
class Reflection
{
public:    
  Reflection(int width, int height); ///< Basic constructor
  ~Reflection(); ///< Basic destructor

  //------------------------------------------------------------
  /// \brief Rendering
  //
  /// Use these functions to render on the reflection texture.  Call
  /// beginReflectedScene before rendering to the texture and endReflectedScene
  /// when finished rendering.
  //{@
  /// \brief Must be called before rendering the reflection
  void beginReflectedScene(const Plane &plane);
  /// \brief Must be called immediately after rendering the reflection.
  void endReflectedScene();
  //@}

  const int textureHandle; ///< Handle to the reflection texture

private:

  EulerAngles m_oldOrientation; ///< Saves camera orientation
  Vector3 m_oldPosition; ///< Saves camera position

};

#endif
