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

/// \file Effect.cpp
#include "Effect.h"
#include <fstream>
#include <string>
#include "directorymanager/directorymanager.h"
#include "common/Renderer.h"

// get the directX device
extern LPDIRECT3DDEVICE9 pD3DDevice;

/// Loads an effect file into memory
/// \param fileName Name of effect file
/// \param isDynamic True iff effect resides on the video card
/// \param defaultDirectory If true is specified, the file will be loaded from
/// the default directory.  False specifies to load the file from the current
/// directory.
Effect::Effect(std::string fileName, bool isDynamic,bool defaultDirectory)
: ResourceBase(isDynamic),
m_clipPlaneChanged(false)
{
  
	ID3DXBuffer* cErrors; // holds errors returned if effect file was invalid
  DWORD shaderFlags;

  // set the shaderFlags accordingly for 
  if (gRenderer.getShaderDebug())
    shaderFlags = D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION;   
  else
    shaderFlags = D3DXSHADER_NO_PRESHADER;

  // set directory to the default shader directory if requested
  if (defaultDirectory)
    gDirectoryManager.setDirectory(eDirectoryShaders);
  // load the D3DX effect file
  if(
    D3DXCreateEffectFromFile
    ( 
      pD3DDevice, // the device
      fileName.c_str(), // file name
      NULL, // some stuff we don't need
      NULL, // more stuff
      shaderFlags, // flags calculated earlier in this function
		  NULL, // more
      &m_pEffect, // pointer to the effect point
      &cErrors  // errors will be returned here
      )
		!=D3D_OK)
	{
    m_pEffect = NULL;    
	}
}

/// Releases the effect file
Effect::~Effect()
{
  // release it if it's there
  if (m_pEffect) 
    m_pEffect->Release();
    m_pEffect = NULL;
}


/// \param matrixName Name of matrix you want to set in effect file.
void Effect::setWorldViewProjMatrixFromDevice(const std::string &matrixName)
{
  if (!m_pEffect) return;

  D3DXMATRIX World,View, Projection,WorldViewProj;

  // get all matrices from device
  pD3DDevice->GetTransform(D3DTS_WORLD, & World);
  pD3DDevice->GetTransform(D3DTS_VIEW, & View);
  pD3DDevice->GetTransform(D3DTS_PROJECTION, & Projection);

  // multiply them
  WorldViewProj = World * View * Projection;

  // pass them to effect file
  m_pEffect->SetMatrix(matrixName.c_str(), &WorldViewProj);
}

/// \param textureName Name of texture you want to set in the effect file.
/// \param index Texture index to set.  This is pulled from the DirectX device.
void Effect::setTextureFromDevice(const std::string &textureName, int index)
{
  if (!m_pEffect) return;

  IDirect3DBaseTexture9* pTexture;

  // get texture from device
  pD3DDevice->GetTexture(index,&pTexture);

  // pass texture to effect file
  m_pEffect->SetTexture(textureName.c_str(), pTexture);

  pTexture->Release();

  return;
}

/// \param floatName Name of float you want to set in the effect file.
/// \param value Value of float.
void Effect::setFloat(const std::string &floatName, float value)
{
  if (!m_pEffect) return;
  
  m_pEffect->SetFloat(floatName.c_str(), value);
  
  return;
}

/// Converts an unsigned type (color) to 4 floats.  This is done because then
/// it is easier to use in the effect file.
/// \param colorName Name of float4 you want to set in the effect file.
/// \param color Color to set.
void Effect::setColor(const std::string &colorName, unsigned color)
{
  if (!m_pEffect) return;

  D3DXVECTOR4 vec;

  // calculate the a 4D vector from the color
  BYTE* pColor = (BYTE*)&color;
  vec.x = ((float)pColor[2]) / 256.0f;
  vec.y = ((float)pColor[1]) / 256.0f;
  vec.z = ((float)pColor[0]) / 256.0f;
  vec.w = ((float)pColor[3]) / 256.0f;
 

  m_pEffect->SetVector(colorName.c_str(),&vec);


}

// Sets the value of a global bool value in the effect file
void Effect::setBoolean(const std::string &vectorName, bool value)
{
  if (!m_pEffect) return;
  m_pEffect->SetBool(vectorName.c_str(),value);
}

/// \param vectorName Name of vector you want to set in the effect file.
/// \param vec Vector to set.
void Effect::setVector(const std::string &vectorName, Vector3 vec)
{
  if (!m_pEffect) return;
  D3DXVECTOR4 d3dVec;

  // convert Vector3 to D3DXVECTOR4
  d3dVec.x = vec.x;
  d3dVec.y = vec.y;
  d3dVec.z = vec.z;
  d3dVec.w = 1.0f;

  m_pEffect->SetVector(vectorName.c_str(), &d3dVec);
}

/// \param vectorName Name of vector you want to set in the effect file.
/// \param vec Vector to set.
void Effect::setVector(const std::string &vectorName, Vector2 vec)
{
  if (!m_pEffect) return;
  D3DXVECTOR4 d3dVec;

  // convert Vector3 to D3DXVECTOR4
  d3dVec.x = vec.x;
  d3dVec.y = vec.y;
  d3dVec.z = 1.0f;
  d3dVec.w = 0.0f;

  m_pEffect->SetVector(vectorName.c_str(), &d3dVec);
}


// Sets a technique from the effect file to render with
/// \param technique Name of technique to set
void Effect::setTechnique(const std::string &technique)
{
  if (!m_pEffect) return;

  m_pEffect->SetTechnique((char*)technique.c_str());

  return;
}

// Checks to see if a technique is valid
/// \param technique Name of the technique to check
/// \return True if the technique is valid
bool Effect::validTechnique(const std::string &technique)
{
	if (!m_pEffect) return false;
	return SUCCEEDED(m_pEffect->ValidateTechnique(technique.c_str()));
}

/// \param matrixName Name of matrix you want to set in the effect file.
void Effect::setWorldMatrix(const std::string &matrixName)
{ 
  if (!m_pEffect) return;

  D3DXMATRIX world;

  // get all matrices from device
  pD3DDevice->GetTransform(D3DTS_WORLD, &world);

  m_pEffect->SetMatrix(matrixName.c_str(), &world);
  
}


/// \param matrixName Name of matrix you want to set in the effect file.
/// \param matrix 4x3 Matrix to set.
void Effect::setMatrix4x3(const std::string &matrixName, Matrix4x3 matrix)
{
  if (!m_pEffect) return;
	

  // convert Matrix4x3 to D3DXMATRIX
  D3DXMATRIX	m;
	m._11 = matrix.m11;
	m._12 = matrix.m12;
	m._13 = matrix.m13;
	m._14 = 0.0f;

	m._21 = matrix.m21;
	m._22 = matrix.m22;
	m._23 = matrix.m23;
	m._24 = 0.0f;

	m._31 = matrix.m31;
	m._32 = matrix.m32;
	m._33 = matrix.m33;
	m._34 = 0.0f;

	m._41 = matrix.tx;
	m._42 = matrix.ty;
	m._43 = matrix.tz;
	m._44 = 1.0f;

  // pass it
  m_pEffect->SetMatrix(matrixName.c_str(),&m);

}

// called before rendering
void Effect::startEffect()
{
  if (!m_pEffect) return;

  m_clipPlaneChanged = false;

  

  // if a clipping plane is specified, it must be converted to projections space
  // the plane is currently assumed to be in world space.
  if (gRenderer.getClipPlaneEnable())
  {
    D3DXMATRIX View, Projection,ViewProj;
    
    
    D3DXPLANE projPlane, worldPlane;

	  //D3DXVECTOR4 projClipPlane;

    gRenderer.getClipPlane(&m_oldClipPlane);
    worldPlane.a = m_oldClipPlane.a;
    worldPlane.b = m_oldClipPlane.b;
    worldPlane.c = m_oldClipPlane.c;
    worldPlane.d = m_oldClipPlane.d;
        
        
    // get all matrices from device    
    pD3DDevice->GetTransform(D3DTS_VIEW, & View);
    pD3DDevice->GetTransform(D3DTS_PROJECTION, & Projection);

    // multiply them
    ViewProj = View * Projection;

    //worldPlane.y *= -1;

    D3DXPlaneNormalize(&worldPlane, &worldPlane);

    D3DXMatrixInverse(&ViewProj, NULL ,&ViewProj);
    D3DXMatrixTranspose(&ViewProj ,&ViewProj);

    D3DXPlaneTransform(&projPlane, &worldPlane, &ViewProj);

    D3DXPlaneNormalize(&projPlane, &projPlane);

    Plane passIn(projPlane.a, projPlane.b, projPlane.c, projPlane.d);

    gRenderer.setClipPlane(passIn);
    
    m_clipPlaneChanged = true;
  }
  
	m_pEffect->Begin(0,0);
	m_pEffect->BeginPass(0);
  m_pEffect->CommitChanges();
}

// called after rendering
void Effect::endEffect()
{
  if (!m_pEffect) return;
  // change the clipping plane back
  if (m_clipPlaneChanged)
  {
    gRenderer.setClipPlane(m_oldClipPlane);       
  }
  m_pEffect->EndPass();
  m_pEffect->End();
}

// called when device is released
void Effect::release()
{  
  m_pEffect->OnLostDevice();
}

// called when device is restored
void Effect::restore()
{
  m_pEffect->OnResetDevice();
}