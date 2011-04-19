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

/// \file Water.h
#ifndef __WATER_H_INCLUDED__
#define __WATER_H_INCLUDED__

#include "graphics/VertexTypes.h"
#include "graphics/vertexbuffer.h"
#include "graphics/vertextypes.h"
#include "graphics/indexbuffer.h"
#include "common/vector2.h"

//-----------------------------------------------------------------------------
/// \class Water
class Water
{	
public:

  /// \brief Constructor
  Water(int vertsPerSide = 129);

  /// \brief Destructor
  ~Water(); 
  	 	
  /// \brief Render the water to the screen
  void render();
  /// \brief Allows water to process movements such as texture translations.
  void process(float elapsed);
  
  float alpha; ///< Alpha value of water (transparency)
  float dimensions; ///< Width and Length of the water plane segment  
  int textureHandle; ///< Handle to the water texture
  float textureScale; ///< Value all texture coordinates are scaled by.
  /// \brief Velocity that a texture travels in quads per second
  Vector2 textureVelocity;
  /// \brief Location of the center of the water plane segment
  Vector3 centerLocation; 
 
private:

  // constants used to create mesh
  const int numQuads;
  const int numVerts;
  const int numVertsPerSide;

  /// \name Alpha Blending Setup
  /// The Renderer object does not support advanced blending features.  
  /// Since the water needs to support alpha blending based on the the alpha
  /// value in the vertex, we need these function to contact the DirectX device
  /// directly.  This rarely has to be done.  You could add advanced blending
  /// functionality to the Renderer class as an exercise.  
  //{@
  /// \brief Sets up how the alpha blending is done
  void setAlphaBlendingMode();  
  /// \brief Restores alpha blending information back to how it was before
  /// setAlphaBlendingMode was called
  void restoreAlphaBlendingMode();
  
  //@}

	/// \brief Creates a water mesh.
  void setupMesh(); 
  
  /// \name Mesh Data
  //{@
  /// \brief Stores all the vertices in the water grid
  LitVertexBuffer* m_vertexBuffer;
  /// \brief Indicies of vertices specifing how to render the water grid
  IndexBuffer* m_indexBuffer;   
  //@}
  
  /// \brief Translation of the texture.  This is incremented in process
  Vector2 m_texturePos;   
   
};

#endif
