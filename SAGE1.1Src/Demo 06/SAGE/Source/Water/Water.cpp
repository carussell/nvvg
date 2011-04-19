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
/// \brief Code for the Water class.

#include "Water.h"
#include "directorymanager/directorymanager.h"
#include "tinyxml/tinyxml.h"

bool Water::m_bReflection = true;

/// \param fieldOfViewInRadians Field of view that will be used when rendering
/// the water.  This can be changed by calling reloadMesh().  This angle must
/// be specified in radians.
/// \param farClippingPlane Far clipping plane that will be used when rendering
/// the water.  This can be changed by calling reloadMesh()
/// \param xmlFileName Name of the XML file to load attributes from.
/// \param defaultXMLDirectory True specifies that the current directory should
/// be set to the default XML directory.
Water::Water(float fieldOfViewInRadians, float farClippingPlane, 
             const char* xmlFileName, bool defaultXMLDirectory):    
  m_reflection(512,512)
{  
  m_vertexBuffer = NULL; 
  m_indexBuffer = NULL;
  m_farClippingPlane = 0.0f;
  m_fieldOfView = 0.0f;
    
  parseXML(xmlFileName, defaultXMLDirectory);

  // create the effect file
  gDirectoryManager.setDirectory(eDirectoryEngine);
  m_effect = new Effect("water.fx",true,false);

  // If the graphics card will support per-pixel reflection mapping then use
  // that technique.  If not select per-vertex mapping.
  if (m_effect->validTechnique("PerPixelMapping"))
  	  m_techniqueName ="PerPixelMapping";
  else if (m_effect->validTechnique("PerVertexMapping"))
	  m_techniqueName = "PerVertexMapping";

  reloadMesh(fieldOfViewInRadians, farClippingPlane);  
  
}


Water::~Water()
{
  delete m_effect; m_effect = NULL;
  delete m_vertexBuffer; m_vertexBuffer = NULL;
  delete m_indexBuffer; m_indexBuffer = NULL;
}

// Render the water mesh
/// The camera location and heading are needed so that the water mesh can be
/// rotated and translated to the position directly infront of the camera.
/// \param CamLoc Location of the camera in world space
/// \param CamHeading Heading of the camera (Rotation around the Y-Axis)
void Water::render(Vector3 CamLoc, float CamHeading)
{	
  
  Vector3 WaterLoc = CamLoc;

  WaterLoc.y = m_waterHeight; // we don't care about the y component
  
  // push the world transformation on the stack
  gRenderer.instance(WaterLoc, EulerAngles(CamHeading, 0.0f,0.0f));

  // pass information to the effect file
  m_effect->setColor("lightDirectionColor",
    gRenderer.getDirectionalLightColor());  
  m_effect->setVector("negativeLightDirection", 
    -gRenderer.getDirectionalLightVector());
  m_effect->setColor("ambientLight", 
    gRenderer.getAmbientLightColor());  
  m_effect->setWorldViewProjMatrixFromDevice("worldViewProj");
  m_effect->setMatrix4x3("world", gRenderer.getModelToWorldMatrix());
  m_effect->setColor("diffuseColor", m_color);
  m_effect->setVector("textureTrans", m_texturePos);
  m_effect->setBoolean("reflectionOn", Water::m_bReflection);
  m_effect->setFloat("textureScaleInverse", 1.0f/m_textureScale);
  
  m_effect->setVector("cameraPosition", gRenderer.getCameraPos());
  
  // set fog variables
  float fogConstant = gRenderer.getFogFar() - gRenderer.getFogNear();
  m_effect->setFloat("FogEnd", gRenderer.getFogFar());
  m_effect->setFloat("FogConstant", 1.0f / fogConstant);
  m_effect->setColor("FogColor", gRenderer.getFogColor());
  
  m_effect->setFloat("screenWidth", (float)gRenderer.getScreenX());
  m_effect->setFloat("screenHeight", (float)gRenderer.getScreenY());

  gRenderer.selectTexture(m_textureHandle,0);
  gRenderer.selectTexture(m_reflection.textureHandle,1);
  gRenderer.selectTexture(m_textureHandleBumpMap,2);
  m_effect->setTextureFromDevice("textureWater",0);
  m_effect->setTextureFromDevice("textureReflection",1);
  m_effect->setTextureFromDevice("textureDistortionMap",2);

  m_effect->setTechnique(m_techniqueName.c_str());
  
  Matrix4x3 Mr;  
  Mr.m11 = 0.5f;  Mr.m12 = 0.0f;  Mr.m13 = 0.0f;   
  Mr.m21 = 0.0f;  Mr.m22 = 0.5f;  Mr.m23 = 0.0f;  
  Mr.m31 = 0.0f;  Mr.m32 = 0.0f;  Mr.m33 = 0.5f; 
  Mr.tx  = 0.5f;  Mr.ty  = 0.5f;  Mr.tz  = 0.5f; 
  m_effect->setMatrix4x3("posToReflectTextureMatrix", Mr);

  // render the water
  m_effect->startEffect();
  gRenderer.render(m_vertexBuffer,m_indexBuffer);
  m_effect->endEffect();

  // pop the world tranformation of the stack
  gRenderer.instancePop();

}

// allows water to process
/// This should be called once per frame.
/// \param elapsed The elapsed time in seconds since the last time process
/// was called.
void Water::process(float elapsed)
{
  // increment texture coordinates
  m_texturePos += elapsed * m_textureVelocity;
  
  // wrap the texture coordinates around 0;
  m_texturePos.x = fmod(m_texturePos.x, 1.0f);
  m_texturePos.y = fmod(m_texturePos.y, 1.0f);

  return;
}

// Sets up a new water mesh
/// \param fieldOfViewInRadians Field of view in radians that the water will
/// be rendered at.
/// \param farClippingPlane Far clipping plane that the water will be rendered
/// at
void Water::reloadMesh(float fieldOfViewInRadians, float farClippingPlane)
{

  // if a new field of view or far clipping plane is requested
  if (fieldOfViewInRadians != m_fieldOfView ||
    farClippingPlane != m_farClippingPlane)
  {
    setupMesh(fieldOfViewInRadians, farClippingPlane);
    
    // save specifications
    m_fieldOfView = fieldOfViewInRadians;
    m_farClippingPlane = farClippingPlane;
  }
}


void Water::setupMesh(float fieldOfViewInRadians, float farClippingPlane)
{
  switch (m_meshType)
  {
    case eWaterMeshWedge:
    {setupMeshWedge(fieldOfViewInRadians,farClippingPlane);}break;
    case eWaterMeshRectangle:
    {setupMeshRectangle(100,200,50,50,20.0f);}break;
  }
}

/// Creates a wedge mesh made up of trapezoids.  Each trapezoid
/// has a different size and a different number of triangles depending on
/// an exponential functions.
/// \param fov Field of view that will be used when rendering the water
/// \param farClipPlane Far clipping plane
void Water::setupMeshWedge(float fov, float farClipPlane)
{
  // make the fov a little larger so you can't see the edges of the water
  fov *= 1.5f;

  int numTrapezoids = 25; // number of trapezoids in mesh
  int minRes = 15; // Minimum number of vertices on small side of trapezoid
  int maxRes = 25; // Maximum number of vertices on small side of trapezoid
  
  float funcScale = 0;
  int levelRes = 0; // verts on smaller end of trapezoid
  float levelstart = 0.0f;
  float levelend = 0.0f;
  int vertsPerTrapezoid;
  int trapStartIndex;   
  float dx;
  float startX, endX;
  double fovD2 = (double)(fov / 2.0f);

  int vertsNeeded = 0;
  int trisNeeded = 0;  
  int vertexCount = 0;
  int indCount = 0;

  int *resolutions = new int[numTrapezoids];
  float *trapEnd = new float[numTrapezoids];
  
  funcScale = farClipPlane / (float)((numTrapezoids) * (numTrapezoids));
  for (int i =0; i < numTrapezoids; i++)
    trapEnd[i] = funcScale * (float)((i + 1) *(i + 1));

  funcScale = (float)(minRes - maxRes);//(float)((numTrapezoids) * (numTrapezoids));
  for (int i =0; i < numTrapezoids; i++)
    resolutions[i] = (int)(funcScale * (float) (pow(trapEnd[i]/ farClipPlane, 2.0f))) + maxRes;

  // calculate total number of vertices and triangles needed
  for (int i = 0; i < numTrapezoids; i++)
  {
    vertsNeeded += resolutions[i] * 2 + 2;
    trisNeeded += resolutions[i] * 2;
  }

  // delete previous vertex and index buffers
  delete m_vertexBuffer; m_vertexBuffer = NULL;
  delete m_indexBuffer; m_indexBuffer = NULL;


  // create vertex and index buffers
  m_vertexBuffer = new VertexBuffer<RenderVertexWater>(vertsNeeded);
  m_indexBuffer = new IndexBuffer(trisNeeded);
  
  /////// Fill the vertex buffer
  if (!m_vertexBuffer->lock()) 
    return;
	  
  /////// fill in index buffer
	if (!m_indexBuffer->lock()) 
		return;
  


  for (int a = 0; a < numTrapezoids; a++)
  {    
    levelstart = levelend;
    levelend = trapEnd[a];
    
 
    levelRes = resolutions[a];
    vertsPerTrapezoid = levelRes * 2 + 2;
    trapStartIndex = vertexCount;
    
    
    startX = -(levelend * (float)tan(fovD2));
    endX = -startX;
    
    // fill in top vertices of trapezoid
    for (int b = 0; b < levelRes + 2; b++)    
    {
      dx = (float)b / (float)(levelRes + 2-1);
      (*m_vertexBuffer)[vertexCount++].p = Vector3(endX * dx + startX * (1.0f - dx), 0.0f, levelend);
    }

    startX = -(levelstart * (float)tan(fovD2));
    endX = -startX;
    // fill in bottom vertices of trapezoid
    for (int b = 0; b < levelRes; b++)    
    {
      dx = (float)b / (float)(levelRes - 1);
      (*m_vertexBuffer)[vertexCount++].p  = Vector3(endX * dx + startX * (1.0f - dx), 0.0f, levelstart);
    }

    
    // fill single left triangle
    (*m_indexBuffer)[indCount].index[0] = trapStartIndex;
    (*m_indexBuffer)[indCount].index[1] = trapStartIndex + 1;
    (*m_indexBuffer)[indCount++].index[2] = trapStartIndex + levelRes + 2;
    
    // fill single right triangle
    (*m_indexBuffer)[indCount].index[0] = trapStartIndex + levelRes;
    (*m_indexBuffer)[indCount].index[1] = trapStartIndex + levelRes + 1;
    (*m_indexBuffer)[indCount++].index[2] = trapStartIndex + vertsPerTrapezoid - 1;
    
    // fill in top triangles in trapezoid
    for (int b =0; b <  (levelRes - 1); b++)
    {
      (*m_indexBuffer)[indCount].index[0] = trapStartIndex + 1 + b;
      (*m_indexBuffer)[indCount].index[1] = trapStartIndex + 2 + b;
      (*m_indexBuffer)[indCount++].index[2] = trapStartIndex + levelRes + 2 + 1 + b;
    }

    // fill in top triangles in trapezoid
    for (int b =0; b <  (levelRes - 1); b++)
    {
      (*m_indexBuffer)[indCount].index[0] = trapStartIndex + 1 + b;
      (*m_indexBuffer)[indCount].index[1] = trapStartIndex + levelRes + 2 + 1 + b;
      (*m_indexBuffer)[indCount++].index[2] = trapStartIndex + levelRes + 2 + b;
    }        
  
  } // done filling in trapezoid
    
  
  m_indexBuffer->unlock(); // unlock
  m_vertexBuffer->unlock();

  delete []resolutions;
  delete []trapEnd;
  
}
// Creates a rectangular water mesh
/// \param sizeX the length of the water mesh
/// \param sizeZ the depth of the water mesh
/// \param centerX Vertex that is the center vertex
/// \param centerZ Vertex that is the center vertex
/// \param spacing Spacing between each vertex.
void Water::setupMeshRectangle(int sizeX, int sizeZ,int centerX, int centerZ, float spacing)
{
  // delete previous vertex and index buffers if there are any
  if (m_vertexBuffer)
    delete m_vertexBuffer;
  m_vertexBuffer = NULL;

  if (m_indexBuffer)
    delete m_indexBuffer;
  m_indexBuffer = NULL;

  int numQuads = (sizeX - 1) * (sizeZ -1);
  
  // create vertex and index buffers
  m_vertexBuffer = new VertexBuffer<RenderVertexWater>(sizeX * sizeZ);
  m_indexBuffer = new IndexBuffer(2 * numQuads);
  


  /////// Fill the vertex buffer
  if (!m_vertexBuffer->lock()) 
    return;

  float locX;
  float locZ;
  float cX = centerX * spacing;
  float cZ = centerZ * spacing;

  for (int z = 0; z < sizeZ;z++)
    for (int x = 0; x< sizeX; x++)
    {
      locX = x * spacing - cX;
      locZ = z * spacing - cZ;
      (*m_vertexBuffer)[x + z * sizeX].p = Vector3(locX, 0.0f, locZ);
    }
    
	m_vertexBuffer->unlock();



/////// fill in index buffer
	if (!m_indexBuffer->lock()) 
		return;

  
  //set triangles 
  for (int i = 0 ; i < sizeZ-1 ; i++) {
	  for (int j = 0 ; j < sizeX-1 ; j++) {
      (*m_indexBuffer)[i*(sizeX-1) + j].index[0] = i*sizeX+j;
      (*m_indexBuffer)[i*(sizeX-1) + j].index[1] = (i+1)*sizeX + j + 1;
      (*m_indexBuffer)[i*(sizeX-1) + j].index[2] = (i+1)*sizeX + j;
      (*m_indexBuffer)[numQuads+i*(sizeX-1)+j].index[0] = i*sizeX + j; 
      (*m_indexBuffer)[numQuads+i*(sizeX-1)+j].index[1] = i*sizeX + j + 1;
      (*m_indexBuffer)[numQuads+i*(sizeX-1)+j].index[2] = (i+1)*sizeX + j + 1;
    }
  }
	
  m_indexBuffer->unlock(); // unlock

}

// fills in water attributes by parsing an XML file
/// \brief xmlFileName The name of the xml file to parse
/// \brief defaultXMLDirectory True specifies that the xml file is in the
/// default XML directory.
void Water::parseXML(const char* xmlFileName, bool defaultXMLDirectory)
{
  // the XML document object
	TiXmlDocument file;
  TiXmlElement *item, *waterTag, *textures;
  std::string type;
  double dtemp; // used to convert from doubles to floats
  
  // load from default directory if specified
  if (defaultXMLDirectory)
    gDirectoryManager.setDirectory(eDirectoryXML);

	// load the terrain xml
	if (!file.LoadFile(xmlFileName)) return;

  // get the terrain element list
  waterTag = file.FirstChildElement("water");
  
  if (waterTag == NULL) return;

  // get water height
  waterTag->Attribute("height", &dtemp); m_waterHeight = (float)dtemp;
  
  // get mesh type
  type = waterTag->Attribute("meshType");
  if (type == "wedge")
	  m_meshType = eWaterMeshWedge; 
  else
    m_meshType = eWaterMeshRectangle; 

  
  // get textures element
  textures = waterTag->FirstChildElement("textures");
  if (textures == NULL) return;
  
  // scale
  textures->Attribute("stretch", &dtemp);
  m_textureScale = (float)dtemp;

  // speed
  textures->Attribute("speedX", &dtemp); m_textureVelocity.x = (float)dtemp;
  textures->Attribute("speedY", &dtemp); m_textureVelocity.y = (float)dtemp;

  item = textures->FirstChildElement("bumpmap");
  if (item)    
    m_textureHandleBumpMap = gRenderer.cacheTextureDX(item->Attribute("filename"));      
  
  item = textures->FirstChildElement("texture");
  if (item)
    m_textureHandle = gRenderer.cacheTextureDX(item->Attribute("filename"));

  // get the water color
  item = waterTag->FirstChildElement("color");
  if (item)
  {
    int a, r, g, b;
    item->Attribute("a", &a);
    item->Attribute("r", &r);
    item->Attribute("g", &g);
    item->Attribute("b", &b);
    m_color = MAKE_ARGB(a,r,g,b);
  }  
}
