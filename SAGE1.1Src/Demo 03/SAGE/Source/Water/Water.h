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

#include "graphics/vertexbuffer.h"
#include "graphics/vertextypes.h"
#include "graphics/indexbuffer.h"
#include "graphics/effect.h"
#include "water/reflection.h"
#include "common/vector2.h"

/// \brief Custom vertex for rendering the water.
//
/// No texture coordinates are needed because they are calculated in the
/// vertex shader
struct RenderVertexWater
{
	Vector3 p; // vertex position
  static const DWORD FVF = D3DFVF_XYZ;
};

/// Specifies what shape the water mesh will be.  A wedge is good for
/// the follow camera and the rectangle water mesh is good for any other 
/// camera.  A new shape of mesh might be required depending on the situation.
enum EWaterMesh
{
  eWaterMeshWedge, ///< Specifies a wedge shaped mesh
  eWaterMeshRectangle ///< Specifies a rectangle shaped mesh
};

//-----------------------------------------------------------------------------
/// \class Water
/// \brief Water capable of reflections and specular lighting
class Water
{	
public:
  /// \brief Global flag; specifies whether reflections should be rendered on
  /// the water
  static bool m_bReflection; 

  /// \brief Constructor
  Water(float fieldOfViewInRadians, float farClippingPlane, 
    const char* xmlFileName, bool defaultXMLDirectory = true);
  /// \brief Destructor
  ~Water();
  	 	
  /// \brief Render the water to the screen
  void render(Vector3 CamLoc, float CamHeading);
  /// \brief Allows water to process movements such as texture translations.
  void process(float elapsed);

  /// \brief Refills the vertex buffer with the water mesh if field of view or
  /// radians have changed.
  void reloadMesh(float fieldOfViewInRadians, float farClippingPlane);

  /// \brief Returns height of water in world space
  /// \return Height of water in world space
  float getWaterHeight() {return m_waterHeight;}  
  /// \brief Reflection object.  The reflection for the water's surface should
  /// be rendered using this object.
  /// \remark Before calling render, it is assumed that you have already used
  /// this reflection object to render the reflected scene.
  Reflection m_reflection;
 
private:
	/// \brief Creates a water mesh.
  void setupMesh(float fieldOfViewInRadians, float farClippingPlane);

  /// \brief Creates a rectanglular water mesh
  void setupMeshRectangle(int sizeX, int sizeZ,int centerX, int centerZ, float spacing);
  /// \brief Creates a wedge shaped water mesh
  void setupMeshWedge(float fov, float farClipPlane);

  /// \brief Fills in water attributes by parsing an XML file
  void parseXML(const char* xmlFileName, bool defaultXMLDirectory);
  
  /// \name Mesh Data
  //{@
  /// \brief Stores all the vertices in the water grid
  VertexBuffer<RenderVertexWater>* m_vertexBuffer;
  /// \brief Indicies of vertices specifing how to render the water grid
  IndexBuffer* m_indexBuffer;
  EWaterMesh m_meshType; ///< Shape of the mesh
  float m_fieldOfView; ///< Field of view recorded in radians
  float m_farClippingPlane; ///< Records the far clipping plane
  bool m_perPixelMapping; ///< If we are doing per pixel reflections mapping
  //@}

  /// \name Texture Data
  //{@
  int m_textureHandle; ///< Handle to the water texture
  /// \brief Handle to teh bump map used to distort the reflection textures
  /// simulating ripples
  int m_textureHandleBumpMap;   
  /// \brief Inverse of how many times the texture repeats per quad
  float m_textureScale; 
  /// \brief Velocity that a texture travels in quads per second
  Vector2 m_textureVelocity;
  /// \brief Translation of the texture.  This is incremented in process
  Vector2 m_texturePos; 
  //@}
  
  std::string m_techniqueName; ///< Name of technique we use to render
  unsigned int m_color; ///< Color of water (0XAARRGGBB)  
  /// \brief Effect used to render the water.  This is necessary because the
  /// water must be rendered with a custom vertex and pixel shader.
  Effect* m_effect;
  float m_waterHeight; ///< Height of water.
};

#endif
