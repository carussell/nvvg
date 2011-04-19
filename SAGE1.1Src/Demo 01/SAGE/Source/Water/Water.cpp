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

/// \file Water.cpp
#include "Water.h"
#include "directorymanager/directorymanager.h"
#include "tinyxml/tinyxml.h"
#include <d3d9.h>

Water::Water(int vertsPerSide):
numVertsPerSide(vertsPerSide),
numVerts(vertsPerSide * vertsPerSide),
numQuads((vertsPerSide - 1) * (vertsPerSide - 1))
{  
  // create vertex and index buffers
  // note that the vertex buffer is created as a dynamic texture because
  // it is frequently changed
  m_vertexBuffer = new LitVertexBuffer(numVerts, true);
  m_indexBuffer = new IndexBuffer(2 * numQuads);
    
  // Fill in index buffer
  // This only has to be done one time so it is done in the constructor.
  // The vertex buffer isn't filled here because it must be filled on a per
  // frame basis.
	if (m_indexBuffer->lock()) 
  {
    //set triangles 
    for (int i = 0 ; i < numVertsPerSide-1 ; i++) {
	    for (int j = 0 ; j < numVertsPerSide-1 ; j++) {
        (*m_indexBuffer)[i*(numVertsPerSide-1) + j].index[0] = i*numVertsPerSide+j;
        (*m_indexBuffer)[i*(numVertsPerSide-1) + j].index[1] = (i+1)*numVertsPerSide + j + 1;
        (*m_indexBuffer)[i*(numVertsPerSide-1) + j].index[2] = (i+1)*numVertsPerSide + j;
        (*m_indexBuffer)[numQuads+i*(numVertsPerSide-1)+j].index[0] = i*numVertsPerSide + j; 
        (*m_indexBuffer)[numQuads+i*(numVertsPerSide-1)+j].index[1] = i*numVertsPerSide + j + 1;
        (*m_indexBuffer)[numQuads+i*(numVertsPerSide-1)+j].index[2] = (i+1)*numVertsPerSide + j + 1;
      }
    }    	
    m_indexBuffer->unlock(); // unlock
  }

}

Water::~Water()
{  
  // free resources
  delete m_vertexBuffer; m_vertexBuffer = NULL;
  delete m_indexBuffer; m_indexBuffer = NULL;
}

// Render the water mesh
void Water::render()
{	
   
  // save previous states so they can be restored
  EBackfaceMode prevBackFaceMode = gRenderer.getBackfaceMode();    
  unsigned int prevAmbient = gRenderer.getAmbientLightColor();
  bool prevBlend = gRenderer.getBlendEnable();
   
  // now set the render states we want
  gRenderer.setBackfaceMode(eBackfaceModeDisable);
  gRenderer.setAmbientLightColor(0X00FFFFFF);
  gRenderer.setBlendEnable(true);  
  gRenderer.setSourceBlendMode(eSourceBlendModeSrcAlpha);
  gRenderer.setDestBlendMode(eDestBlendModeInvSrcAlpha);  
  


  gRenderer.selectTexture(textureHandle);

  setAlphaBlendingMode();
  
  // push the location of the water onto the world transformation stack
  gRenderer.instance(centerLocation, EulerAngles::kEulerAnglesIdentity);    
  
  // render the mesh
  gRenderer.render(m_vertexBuffer, m_indexBuffer);  
    
  gRenderer.instancePop();

  restoreAlphaBlendingMode();
  
  gRenderer.setBackfaceMode(prevBackFaceMode);
  gRenderer.setAmbientLightColor(prevAmbient);
  gRenderer.setBlendEnable(prevBlend);

}

// allows water to process
/// This should be called once per frame.
/// \param elapsed The elapsed time in seconds since the last time process
/// was called.
void Water::process(float elapsed)
{
  // increment texture coordinates
  m_texturePos += elapsed * textureVelocity;
  
  // wrap the texture coordinates around 0;
  m_texturePos.x = fmod(m_texturePos.x, 1.0f);
  m_texturePos.y = fmod(m_texturePos.y, 1.0f);

  // recreate a new mesh with the correct texture coordinates
  setupMesh();

  return;
}

static DWORD prevAlphaOp, prevAlphaArg1;

/// \brief Sets up how the alpha blending is done
void Water::setAlphaBlendingMode()
{  
  pD3DDevice->GetTextureStageState(0,D3DTSS_ALPHAOP,&prevAlphaOp);    
  pD3DDevice->GetTextureStageState(0,D3DTSS_ALPHAARG1,&prevAlphaArg1);

  // gRenderer doesn't have support for pulling the alpha value off the  
  // diffuse color of the vertex so we have to go straight to the DirectX
  // device
  pD3DDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);    
  pD3DDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);

}

/// \brief Restores alpha blending information back to how it was before
/// setAlphaBlendingMode was called
void Water::restoreAlphaBlendingMode()
{
  // restore previous states
  pD3DDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,prevAlphaOp);    
  pD3DDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,prevAlphaArg1);
}

/// This must be called everyframe so that the texture coordinates can be
/// updated.
void Water::setupMesh()
{
  /////// Fill the vertex buffer
  if (!m_vertexBuffer->lock()) 
    return;

  float locX, locZ;
  float spacing = dimensions/ (numVertsPerSide - 1);
  float startX = - dimensions / 2.0f;
  float startZ = - dimensions / 2.0f;

  unsigned int color = MAKE_ARGB(alpha * 255, 255,255,255);

  for (int z = 0; z < numVertsPerSide;z++)
    for (int x = 0; x< numVertsPerSide; x++)
    {
      locX = startX + x * spacing;
      locZ = startZ + z * spacing;
      (*m_vertexBuffer)[x + z * numVertsPerSide].p = Vector3(locX, 0.0f, locZ);
      (*m_vertexBuffer)[x + z * numVertsPerSide].u = m_texturePos.x + x * textureScale;
      (*m_vertexBuffer)[x + z * numVertsPerSide].v = m_texturePos.y  + z * textureScale;
      (*m_vertexBuffer)[x + z * numVertsPerSide].argb = color;
    }
    
	m_vertexBuffer->unlock();
}