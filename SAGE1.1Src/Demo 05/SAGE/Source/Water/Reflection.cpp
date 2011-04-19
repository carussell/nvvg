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

/// \file Reflection.cpp
#include "reflection.h"
#include "common/renderer.h"

/// Creates a render target texture of the specified size
/// \param width Width of the render target texture
/// \param height Height of the render target texture
/// \warning gRenderer must initiate before creating a Reflection object
Reflection::Reflection(int width, int height):
textureHandle(gRenderer.allocTexture("reflect",width,height,true,true))
{}

Reflection::~Reflection()
{  
  gRenderer.freeTexture(textureHandle);
}

/// beginReflectedScene flips the camera position and orientation in Renderer
/// over the plane specified.  It also sets the current render target to the
/// texture created in the constructor.  beginReflection scene also calls
/// gRenderer.beginScene() for you.  Simply call this function and then begin
/// rendering.
/// \param plane Plane to reflect the scene over
/// \remark Before calling beingReflectedScene, set the camera to the desired
/// position and orientation.  Changing the orientation afterwards will cause
/// the scene to not be reflected.
void Reflection::beginReflectedScene(const Plane &plane)
{       
  // save current camera position and orientation
  m_oldPosition = gRenderer.getCameraPos();
  m_oldOrientation = gRenderer.getCameraOrient();
  
  // reflect the orienation and the position over the reflection plane
  EulerAngles newOrientation = plane.reflectOrientation(m_oldOrientation);
  Vector3 newPosition = plane.reflectPoint(m_oldPosition);
  
  // set the new position and orientation
  gRenderer.setCamera(newPosition, newOrientation);

  // set the render target to the texture created in the constructor
  gRenderer.setRenderTarget(textureHandle);
  
  // setup the clipping plane
  Plane clipPlane = plane;      
  gRenderer.setClipPlane(clipPlane);
    
  // clear screen for rendering
  gRenderer.clear(kClearFrameBuffer | kClearDepthBuffer | kClearToFogColor);
  gRenderer.beginScene();

  return;
}

/// Calls gRenderer.endScene(), resets the camera position and orientation, and
/// resets the render target.
void Reflection::endReflectedScene()
{

  gRenderer.endScene();
    
  // reset the old camera position and orientation
  gRenderer.setCamera(m_oldPosition, m_oldOrientation);
  
  // set rendertarget back to main render target  
  gRenderer.setRenderTarget();
    
  gRenderer.disableClipPlane();
 
 return;
}
