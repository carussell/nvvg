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

/// \file Effect.h

#ifndef __EFFECT_H_INCLUDED__
#define __EFFECT_H_INCLUDED__

#include "resource/ResourceBase.h"
#include <d3dx9effect.h>
#include <string>
#include "common/Matrix4x3.h"
#include "common/vector3.h"
#include "common/vector2.h"
#include "common/plane.h"


//-----------------------------------------------------------------------------
/// \class Effect
/// \brief Holds effect files allowing for pixel and vertex shaders
class Effect : public ResourceBase
{
public:
  Effect(std::string fileName, bool isDynamic, bool defaultDirectory = true); ///< Constructor
  ~Effect(); ///< Destructor


  /// \name Variable Functions
  /// \brief Used to pass values to the effect file.
  //@{

  void setWorldMatrix(const std::string &matrixName);

  /// \brief Sets the value of a global matrix in the effect file
  void setMatrix4x3(const std::string &matrixName, Matrix4x3 matrix);

  /// \brief Sets the value of a global matrix in the effect file to the concatenation
  /// of the world, view, and projection matrix.
  void setWorldViewProjMatrixFromDevice(const std::string &matrixName);

  /// \brief Sets a global texture in the effect file to the texture from the device
  void setTextureFromDevice(const std::string &textureName, int index);
  
  /// \brief Sets the value of a global float value in the effect file
  void setFloat(const std::string &floatName, float value);

  /// \brief Sets the value of a global color in the effect file
  void setColor(const std::string &colorName, unsigned color);

  /// \brief Sets the value of a global bool value in the effect file
  void setBoolean(const std::string &vectorName, bool value);

  /// \brief Sets the value of a global Vector value in the effect file
  void setVector(const std::string &vectorName, Vector3 vec);

  /// \brief Sets the value of a global Vector value in the effect file
  void setVector(const std::string &vectorName, Vector2 vec);

  /// \brief Sets a technique from the effect file to render with
  void setTechnique(const std::string &technique);

  /// \brief checks to see if a technique is valid
  bool validTechnique(const std::string &technique);

  //@}


  /// \brief Must be called before rendering with the effect
  void startEffect();

  /// \brief Must be called after rendering with the effect
  void endEffect();

  
protected:
 
  void release(); ///< Called when the device is lost.
  void restore(); ///< Called when the device is restored.

private:


  Plane m_oldClipPlane; ///< Holds the world clipping plane
  bool m_clipPlaneChanged; /// Remembers if the clipping plane has changed

  ID3DXEffect* m_pEffect; ///< DirectX Effect interface object


};


#endif