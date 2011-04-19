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

/// \file Terrain.cpp
#include <math.h>
#include "terrain.h"
#include "terrainsubmesh.h"
#include "common/commonstuff.h"
#include "common/random.h"
#include "tinyxml/tinyxml.h"
#include "directorymanager/directorymanager.h"
#include <list>

extern CRandom Random; // random number generator

bool Terrain::terrainTextureDistortion = true;
int Terrain::LOD = -1;
bool Terrain::crackRepair = true;

using namespace std;

/// Creates a terrain representation from an XML.  The height values are
/// loaded in from an image file specified in the XML file.
/// \param submeshPerSide Number of submeshes each row/column will contain.
/// If this is 8, the terrain will be broken into 8 rows and 8 columns yeilding
/// 64 submeshes.
/// \param xmlFileName Name of the XML file that contains terrain information.
/// The XML is loaded from the default XML directory.
Terrain::Terrain(int submeshPerSide,const char *xmlFileName): 
m_nCurrentLOD(0),
m_bDistanceLOD(true),
m_bCrackRepair(true),
m_texturesSupported(8),
m_TextureDistorted(false),
m_terrainTextureIndex(new int[m_texturesSupported]),
m_textureNames(new std::string[m_texturesSupported]),
m_blendHeightsHigh(new float[m_texturesSupported]),
m_blendHeightsLow(new float[m_texturesSupported]),
m_textureStretch(new float[m_texturesSupported])
{
  parseXML(xmlFileName);
 
  // precompute variables.
  m_nVPS = m_pHeightMap->m_nSide;
  m_nSide = m_pHeightMap->m_nSide - 1; // verts in whole terrain
	m_nSubmeshSide = m_nVPS / submeshPerSide; // verts in a submesh
	m_nSubmeshRatio = m_nSide/m_nSubmeshSide; // number or submeshes in a row/col
	m_nNumVertices = m_nVPS*m_nVPS;
	m_nNumQuads = m_nSide*m_nSide;
	m_nNumTriangles = 2*m_nNumQuads;
  m_fOriginOffset = (float)(m_nSide-2)*m_fDelta/2.0f;

  // it is computationally expensive for the video card to set up the pipeline
  // to do a render.  Because of this, rendering small meshes can be just as
  // costly as rendering large ones.  The max level of Detail m_nMaxLOD is
  // calculated based on the minimum triangles that is acceptable for a render.
  // In short, there is no point in rendering a 16 triangle mesh when a 128 
  // triangle mesh is just as expensive
  float minTrianglesPerRender = 128;
  float triPerSubmesh = (float)m_nSubmeshSide * m_nSubmeshSide * 2;  
  float result = log(triPerSubmesh/minTrianglesPerRender) / log(4.0f);
  m_nMaxLOD = (int)(result + 0.5f) + 1;
  
  m_vertices = new RenderVertexTerrain[m_nNumVertices]; //vertices
  m_triangles = new RenderTri[m_nNumTriangles]; //triangles  
  m_triangleNormals = new Vector3[m_nNumTriangles]; //triangle normals  
  initMeshVertices(); //lay out mesh vertices
  initMeshTriangles(); //lay out mesh triangles
  
  for (int a = 0; a < m_texturesSupported; a++)
    m_terrainTextureIndex[a] = 0;
 
  // cache all the textures
  for (int a= 0; a < m_nNumberTextures; a++)
    m_terrainTextureIndex[a] = gRenderer.cacheTexture(m_textureNames[a].c_str());	  
  
  setTerrainFromHeightMap(); //set terrain heights
  
  //etc.
  initNormals(); //initialize vertex normals from heights
  
  if (terrainTextureDistortion)
    setTextureDistortion(); //distort textures to hide patterns

  //create submeshes
  int nNumSubmeshes = m_nSubmeshRatio*m_nSubmeshRatio;
  m_pSubmesh = new TerrainSubmesh**[m_nMaxLOD];
  for(int i=0; i<m_nMaxLOD; i++)
  { 
    m_pSubmesh[i] = new TerrainSubmesh*[nNumSubmeshes];  
    for(int j=0; j < nNumSubmeshes; j++)
      m_pSubmesh[i][j] = new TerrainSubmesh(m_nSubmeshSide,m_nSide,i);
  }

  //create submesh LOD level array
  m_pSubmeshLODLevel = new int*[m_nSubmeshRatio];
  for(int i=0; i<m_nSubmeshRatio; i++)
    m_pSubmeshLODLevel[i] = new int[m_nSubmeshRatio];

  //init submesh structures
  setSubMeshes(); //init submeshes - do this last

  
  // load effect file from the engine resources directory
  gDirectoryManager.setDirectory(eDirectoryEngine);
  m_effect = new Effect("terrain.fx",true,false);

}

Terrain::~Terrain()
{  
  delete [] m_terrainTextureIndex;  m_terrainTextureIndex = NULL;
  delete [] m_textureNames; m_textureNames = NULL;
  delete [] m_blendHeightsHigh; m_blendHeightsHigh = NULL;
  delete [] m_blendHeightsLow; m_blendHeightsLow = NULL;
  delete [] m_textureStretch; m_textureStretch = NULL;
  
  delete m_effect; m_effect = NULL;
  delete [] m_vertices; m_vertices = NULL;
  delete [] m_triangles; m_triangles = NULL;
  delete [] m_triangleNormals; m_triangleNormals = NULL;
  for(int i=0; i<m_nMaxLOD; i++)
  {
    for(int j=0; j<m_nSubmeshRatio*m_nSubmeshRatio; j++)
      delete m_pSubmesh[i][j];
    delete [] m_pSubmesh[i];
  }
  delete [] m_pSubmesh;
  delete m_pHeightMap;
  for(int i=0; i<m_nSubmeshRatio; i++)
    delete [] m_pSubmeshLODLevel[i];
  delete [] m_pSubmeshLODLevel;
}

/// Textures (including height map) specified in this XML are loaded from the
/// default texture directoy.
/// \param xmlFileName Name of XML file.  This will be loaded from the default
/// XML directory.
void Terrain::parseXML(const char* xmlFileName)
{
  // the XML document object
	TiXmlDocument file;
  TiXmlElement *item, *main, *textures;
  double dtemp; // used to convert from doubles to floats
  string heightMapFileName;

  // set default directory
  gDirectoryManager.setDirectory(eDirectoryXML);

  // load the terrain xml
  bool result = file.LoadFile(xmlFileName);	
	assert(result);
  
  // get the terrain element list
  main = file.FirstChildElement("terrain");

  assert(main);

  // get heightmap filename
  item = main->FirstChildElement("heightmap");
  if (item)
    heightMapFileName = item->Attribute("value");

  // get distance between vertices
  item = main->FirstChildElement("stretch");
  if (item)
  {     
    item->Attribute("value",&dtemp);
    m_fDelta = (float)dtemp;
  }

  // get maximum height of terrain
  item = main->FirstChildElement("maxheight");
  if (item)
  {      
    item->Attribute("value",&dtemp);
    m_maxHeight = (float)dtemp;      
  }

  // get maximum height of terrain
  item = main->FirstChildElement("fade");
  if (item)
  {      
    item->Attribute("bottom",&dtemp);
    m_fadeBottom = (float)dtemp;      
    item->Attribute("top",&dtemp);
    m_fadeTop = (float)dtemp;      
  }
  
  // get the textures element     
  textures = main->FirstChildElement("textures");
  if (textures)
  {
    
    item = textures->FirstChildElement();
    m_nNumberTextures = 0;
    while (item && m_nNumberTextures != m_texturesSupported)
    {        
      
      m_textureNames[m_nNumberTextures] = item->Attribute("filename");

      item->Attribute("stretch",&dtemp);
      m_textureStretch[m_nNumberTextures] = (float)dtemp;         
      
      item->Attribute("minheight",&dtemp);
      m_blendHeightsLow[m_nNumberTextures] = (float)dtemp;
      
      item->Attribute("maxheight",&dtemp);
      m_blendHeightsHigh[m_nNumberTextures] = (float)dtemp;              

      m_nNumberTextures++;
      item = item->NextSiblingElement();
    }
    
  }
  
  //height map
  m_pHeightMap = new HeightMap(heightMapFileName.c_str(), m_maxHeight);
  
}

/// This disables distance based LOD
/// \param n Level of detail to be set.  This can range from 0 to m_nMaxLOD - 1
void Terrain::setCurrentLOD(int n)
{
  if( n >= 0 && n < m_nMaxLOD)
  {
    m_nCurrentLOD = n; m_bDistanceLOD = false;    
  }
  else 
    m_bDistanceLOD = true;
}

// renders terrain
void Terrain::render()
{
  
  // if the global terrainTextureDistortion flag was changed
  if (m_TextureDistorted != terrainTextureDistortion)
  {
    m_TextureDistorted = terrainTextureDistortion;

    if (m_TextureDistorted == true)
      setTextureDistortion();
    else
      this->clearTextureDistortion();

    setSubMeshes();
  }

  // update crack setting
  m_bCrackRepair = crackRepair;

  // if the global terrain LOD flag was changed
  setCurrentLOD(LOD);

  for (int a = 0; a < m_nNumberTextures; a++)
    gRenderer.selectTexture(m_terrainTextureIndex[a],a); // Select the texture

  m_effect->setWorldViewProjMatrixFromDevice("WorldViewProj");
  
  m_effect->setTextureFromDevice("texture1",0);
  m_effect->setTextureFromDevice("texture2",1);
  m_effect->setTextureFromDevice("texture3",2);
  m_effect->setTextureFromDevice("texture4",3);
  m_effect->setTextureFromDevice("texture5",4);
  m_effect->setTextureFromDevice("texture6",5);

  m_effect->setFloat("textureStretch1",m_textureStretch[0]);
  m_effect->setFloat("textureStretch2",m_textureStretch[1]);
  m_effect->setFloat("textureStretch3",m_textureStretch[2]);
  m_effect->setFloat("textureStretch4",m_textureStretch[3]);
  m_effect->setFloat("textureStretch5",m_textureStretch[4]);
  m_effect->setFloat("textureStretch6",m_textureStretch[5]);

  float fogConstant = gRenderer.getFogFar() - gRenderer.getFogNear();
  m_effect->setFloat("FogEnd", gRenderer.getFogFar());
  m_effect->setFloat("FogConstant", 1.0f / fogConstant);

  m_effect->setVector("CameraPosition", gRenderer.getCameraPos());

  m_effect->setWorldMatrix("World");

  m_effect->setTechnique("Terrain");
  
  m_effect->setColor("LightDirectionColor",
    gRenderer.getDirectionalLightColor());
  
  m_effect->setVector("NegativeLightDirection", 
    -gRenderer.getDirectionalLightVector());

  m_effect->setColor("AmbientLight", 
    gRenderer.getAmbientLightColor());

  m_effect->startEffect();
  
  //render subgrids
  for(int i=0; i < m_nSubmeshRatio; i++)
    for(int j=0; j < m_nSubmeshRatio; j++)
    {
      unsigned int lodflag = m_pSubmeshLODLevel[i][j]; //precomputed lod flag
      //decode lodflag into lod
      int lod = (lodflag & LOD_DRAW) - 1; //lod is in last 2 bits
      
      if(m_bDistanceLOD) //if doing distance based lod       
      {
        if(lodflag & LOD_DRAW) //if LOD says to render at all
        { 
          if(m_bCrackRepair)          
            m_pSubmesh[lod][i*m_nSubmeshRatio + j]->render(lodflag); //render at precomputed lod          
          else
            m_pSubmesh[lod][i*m_nSubmeshRatio + j]->render(0); //render at precomputed lod with no repair
        }      
      }
      else 
        m_pSubmesh[m_nCurrentLOD][i*m_nSubmeshRatio + j]->render(); //render at default lod
    }
    m_effect->endEffect();
  
}

// sets all normals to up
void Terrain::clearNormals()
{
  for (int i = 0 ; i < m_nNumVertices ; ++i) //for each vertex
    m_vertices[i].n = Vector3(0.0,1.0,0.0); //normal points straight up
}

// calculates the normals of all triangles and vertices
void Terrain::initNormals()
{
  initTriangleNormals(); //compute triangle normals for rendering and collision detection
  initVertexNormals(); //compute vertex normals from triangle normals for rendering
}

// get y coordinate of terrain at (x,z)
/// \param x X coordinate of on the terrain in world space
/// \param z Z coordinate of on the terrain in world space
/// \return Y coordinate on the terrain at specified X and Z positions
float Terrain::getHeight(float x, float z)
{ 

  // bail if point is outside grid
  if(!isPointWithinBounds(x, z))
    return 0.0f;

  // normalize origin to top right corner
  x += m_fOriginOffset; z += m_fOriginOffset;

  //normalize to grid units
  x /= m_fDelta; z /= m_fDelta;
  
  // Compute the grid square in
  int row = (int)x;
  int col = (int)z;
  int square = row * (m_nVPS - 1) + col;
  
  // Compute the offset within the square
  float xoffset = x - row;
  float zoffset = z - col;

  int index = xoffset > zoffset ? square: square + m_nNumQuads;

  float h0,h1,h2; //heights of vertices in triangle
  
  RenderTri* t = &m_triangles[index];

  h0 = m_vertices[t->index[0]].p.y;
  h1 = m_vertices[t->index[1]].p.y;
  h2 = m_vertices[t->index[2]].p.y; 

  if (xoffset > zoffset)  // In bottom left triangle
    return h0 + xoffset*(h2-h0) + zoffset*(h1-h2);
  else // In top right triangle
    return h2 + (1.0f-xoffset)*(h1-h2) + (1.0f-zoffset)*(h0-h1);
}

//get normal of terrain at (x,z)
/// \param x X coordinate of on the terrain in world space
/// \param z Z coordinate of on the terrain in world space
/// \return Normal at terrain location (x,z)
Vector3 Terrain::getNormal(float x, float z)
{ 
  int index=getTriangleIndex(x,z);
  
  if(index>=0 && index<m_nNumTriangles)
    return m_triangleNormals[index];
  else 
    return Vector3(0,1,0);
}

/// This slightly offsets the U V coordinates randomly.  This helps solve the
/// repeating look on the textures.
void Terrain::setTextureDistortion()
{
  for (int i = 0 ; i < m_nVPS ; ++i)   
	  for (int j = 0 ; j < m_nVPS ; ++j) 
    {
		  m_vertices[i*m_nVPS+j].u = (float)i + Random.getFloat(-0.20f,0.20f);
		  m_vertices[i*m_nVPS+j].v = (float)j + Random.getFloat(-0.20f,0.20f);
    }
    m_TextureDistorted = true;
}

/// Changes the texture coordinates so that there is no distortion.
void Terrain::clearTextureDistortion()
{
  for (int i = 0 ; i < m_nVPS ; ++i)
	  for (int j = 0 ; j < m_nVPS ; ++j)
    {
		  m_vertices[i*m_nVPS+j].u = (float)i;
		  m_vertices[i*m_nVPS+j].v = (float)j;
    }  
    m_TextureDistorted = false;
}

// load every submesh with terrain information
void Terrain::setSubMeshes()
{
  for(int k=0; k<m_nMaxLOD; k++)
    for(int i=0; i<m_nSubmeshRatio; i++)
      for(int j=0; j<m_nSubmeshRatio; j++)
        m_pSubmesh[k][i*m_nSubmeshRatio+j]->setMesh(i,j,k,m_vertices);
  
}

/// Allows the levels of detail for each submesh to be computed based on
/// the camera location.
/// \param p Location of the camera.
void Terrain::setCameraPos(const Vector3& p)
{
  m_v3CameraPos = p; 
  getSubmeshIndex(p.x,p.z,m_nCameraSubmeshRow,m_nCameraSubmeshCol);
  
  unsigned int lod = m_nMaxLOD;
  //precompute lod levels for each subgrid
  for(int i=0; i < m_nSubmeshRatio; i++)
    for(int j=0; j < m_nSubmeshRatio; j++)
    {
      lod = m_nMaxLOD;
      // calculate what lod the subgrid should be
      for (int a = 1; a < m_nMaxLOD + 1; a++)
        if (abs(m_nCameraSubmeshRow - i) <= a && abs(m_nCameraSubmeshCol - j) <= a)
        {
          lod = a;
          break;
        }             
      m_pSubmeshLODLevel[i][j] = lod; //default is nothing
    } // end for
    
  //precompute crack information for each subgrid
  for(int i=0; i < m_nSubmeshRatio; i++)
    for(int j=0; j < m_nSubmeshRatio; j++)
      if(m_bDistanceLOD)
      { 
        int lod = (m_pSubmeshLODLevel[i][j] & LOD_DRAW)-1; //lod is in last 2 bits
        int lodneighbor; //lod of neighboring subgrid
        if(i > 0) //neighbor to top
        { 
          lodneighbor = (m_pSubmeshLODLevel[i-1][j] & LOD_DRAW)-1;
          if(lod>=0 && lodneighbor > lod) m_pSubmeshLODLevel[i][j] |= LODCRACK_TOP;
        }
        if(lod>=0 && j < m_nSubmeshRatio-1) //neighbor to right
        {
          lodneighbor = (m_pSubmeshLODLevel[i][j+1] & LOD_DRAW)-1;
          if(lodneighbor > lod) m_pSubmeshLODLevel[i][j] |= LODCRACK_RIGHT;
        }
        if(lod>=0 && i < m_nSubmeshRatio-1) //neighbor to bottom
        { 
          lodneighbor = (m_pSubmeshLODLevel[i+1][j] & LOD_DRAW)-1;
          if(lodneighbor > lod) m_pSubmeshLODLevel[i][j] |= LODCRACK_BOTTOM;
        }
        if(lod>=0 && j > 0) //neighbor to left
        { 
          lodneighbor = (m_pSubmeshLODLevel[i][j-1] & LOD_DRAW)-1;
          if(lodneighbor > lod) m_pSubmeshLODLevel[i][j] |= LODCRACK_LEFT;
        }
      }
}

/// \param pos Position of the starting point of the ray.
/// \param dir Direction and magnitude of the ray.
/// \param outPos Position in world space on the terrain that the ray
/// intersects the terrain.
/// \return True if the ray intersects the terrain.
bool Terrain::rayIntersect(Vector3 pos, Vector3 dir, Vector3& outPos)
{
  std::list<RenderTri*> markedTris;
  float mag = dir.magnitude();
  int numSteps = 300;
  float delta = mag / numSteps; //m_fDelta;
  dir.normalize();
  dir *= delta;

  
  bool foundFirstInBounds = false;
  int lastIndexAdded = -1;
  bool doesIntersect = false;
  Vector3 curPos = pos;

  for(int i=0; i<numSteps; i++)
  {
    curPos += dir;
    if(isPointWithinBounds(curPos.x, curPos.x))
    {
      foundFirstInBounds = true;
      int index = getTriangleIndex(curPos.x, curPos.z);

      if(index == lastIndexAdded) // don't do same triangle twice
        continue;

      markedTris.push_back(&m_triangles[index]);

      if(getHeight(curPos.x, curPos.z) > curPos.y)
      {
        doesIntersect = true;
        outPos.x = curPos.x;
        outPos.z = curPos.z;
        outPos.y = getHeight(curPos.x, curPos.z);
        break;
      }
      
      if(index % 2 == 1) // index is odd, add first half
      {
        markedTris.push_back(&m_triangles[index-1]);
        lastIndexAdded = index - 1;
      }
      else // index is even, add second half
      {
        markedTris.push_back(&m_triangles[index+1]);
        lastIndexAdded = index + 1;
      }
    }
    else
    {
      if(foundFirstInBounds) // we were in the terrain, and now we're not
        break;
    }
  }

  return doesIntersect;
}

/// \param x X coordinate in world space
/// \param z Z coordinate in world space
/// \return True if (x, z) is within the terrain's bounds
bool Terrain::isPointWithinBounds(float x, float z)
{
  // bail if point is outside grid
  if( x < -m_fOriginOffset || x > m_fOriginOffset ||
      z < -m_fOriginOffset || z > m_fOriginOffset )
    return false;

  return true;
}


// Calculates the blending weights of textures based on a height
/// One blending weight is given to each vertex for each texture.  The weight
/// that each texture is rendered at is calculated based on the height of the 
/// vertex.  The Height at which each texture begins and ends is specified in
/// the XML file.
/// \param height Height of vertex in world space.
/// \param outWeight1 Weights 1-4 are returned through this variable.  Each
/// weight is 1 byte ranging from 0 to 255.
/// \param outWeight2 Weights 5-8 are returned through this variable.  Each
/// weight is 1 byte ranging from 0 to 255.
void Terrain::calculateWeightsAtPoint(float height, DWORD& outWeight1, DWORD& outWeight2)
{
	float* textures = new float[m_texturesSupported];
  int* bTextures = new int[m_texturesSupported];
	
	// clear blending array
	for (int a =0; a < m_texturesSupported; a++)
		textures[a] = 0.0f;

	// compute texture weights
	for (int a= 0; a < m_texturesSupported; a++)
	{	
		// above last texture
		if ( a == m_texturesSupported - 1)
		{
			textures[m_texturesSupported - 1] = 1.0f;
			break;
		}

		// below first texture
		if (height < m_blendHeightsLow[0])
		{
			textures[0] = 1.0f; 
			break;
		}

		// all one type of texture
		if (m_blendHeightsHigh[a] >= height
			&& m_blendHeightsLow[a] <= height)
		{
			textures[a] = 1.0f;
			break;
		}
		// blend btw textures
		
		if (m_blendHeightsHigh[a - 1] <= height
			&& m_blendHeightsLow[a] >= height)
		{			
			
			float diff = m_blendHeightsLow[a] - m_blendHeightsHigh[a - 1];

			textures[a - 1] = 1.0f - ((height - m_blendHeightsHigh[a - 1]) / diff);
			textures[a] = ((height - m_blendHeightsHigh[a - 1]) / diff);
			break;
		}
	}

  // stick the alhpa value in weight 7 (index 6)
  if (height >= m_fadeTop) 
    textures[6] = 1.0f;
  else
    if (height <= m_fadeBottom)
      textures[6] = 0.0f;
    else
      textures[6] = (height - m_fadeBottom)/(m_fadeTop - m_fadeBottom);

  // calculate dwords from floats

	for (int a = 0;a < m_texturesSupported; a++)
		bTextures[a] = (int)(textures[a] * 255.0f);

	outWeight1 = ((((bTextures[0])&0xff)<<24)|(((bTextures[1])&0xff)<<16)|(((bTextures[2])&0xff)<<8)|((bTextures[3])&0xff));
	outWeight2 = ((((bTextures[4])&0xff)<<24)|(((bTextures[5])&0xff)<<16)|(((bTextures[6])&0xff)<<8)|((bTextures[7])&0xff));
	
  delete bTextures;
  delete textures;

}

// Returns the row and column of the location (x, z)
/// \param x X location in world Space
/// \param z Z location in world Space
/// \param row Row number of the submesh located at (x,z)
/// \param col Column number of the submesh located at (x,z)
void Terrain::getSubmeshIndex(float x, float z, int& row, int& col)
{
  int index=0; //return result
  // normalize origin to top right corner
  x += m_fOriginOffset; z += m_fOriginOffset;

  //normalize to grid units
  float factor = m_fDelta*(float)m_nSubmeshSide;
  x /= factor; z /= factor;
  
  // Compute the submesh grid location
  row = (int)x; col = (int)z;
}

// Sets mesh vertices to a grid 
void Terrain::initMeshVertices()
{
  for (int i = 0 ; i < m_nVPS ; ++i) 
	  for (int j = 0 ; j < m_nVPS ; ++j) 
    {
		  m_vertices[i*m_nVPS + j].p.x = i*m_fDelta - m_fOriginOffset;
		  m_vertices[i*m_nVPS + j].p.y = 0.0f;
		  m_vertices[i*m_nVPS + j].p.z = j*m_fDelta - m_fOriginOffset;
		  m_vertices[i*m_nVPS + j].n = Vector3(0.0f,1.0f,0.0);
		  m_vertices[i*m_nVPS + j].u = (float)i;
		  m_vertices[i*m_nVPS + j].v = (float)j;
	  }  
}

// set up triangle list / index buffer
void Terrain::initMeshTriangles()
{
  for (int i = 0 ; i < m_nVPS-1 ; ++i)
	  for (int j = 0 ; j < m_nVPS-1 ; ++j) {
      m_triangles[i*(m_nVPS-1) + j].index[0] = i*m_nVPS+j;
      m_triangles[i*(m_nVPS-1) + j].index[1] = (i+1)*m_nVPS + j + 1;
      m_triangles[i*(m_nVPS-1) + j].index[2] = (i+1)*m_nVPS + j;
      m_triangles[m_nNumQuads+i*(m_nVPS-1)+j].index[0] = i*m_nVPS + j; 
      m_triangles[m_nNumQuads+i*(m_nVPS-1)+j].index[1] = i*m_nVPS + j + 1;
      m_triangles[m_nNumQuads+i*(m_nVPS-1)+j].index[2] = (i+1)*m_nVPS + j + 1;
    }
  
}

// Calculates the normals of every triangle in the triangle list m_triangles
void Terrain::initTriangleNormals()
{
  for (int i = 0 ; i < m_nNumTriangles ; i++)
  { //for each triangle
    m_triangleNormals[i] = Vector3::crossProduct( //take the cross product of
      m_vertices[m_triangles[i].index[0]].p - m_vertices[m_triangles[i].index[1]].p, //one edge
      m_vertices[m_triangles[i].index[1]].p - m_vertices[m_triangles[i].index[2]].p //with another edge
      );
    m_triangleNormals[i].normalize();
  }
}

// Calculates vertex normals based on triangle normals
void Terrain::initVertexNormals()
{
  //add triangle normals at vertices
  for (int i = 0 ; i < m_nNumTriangles ; ++i) // for each triangle
    for(int k=0; k<=2; k++) // for each of 3 vertices in triangle
      m_vertices[m_triangles[i].index[k]].n += m_triangleNormals[i]; //add tri normal to vertex normal
  
  //normalize summed vertex normals
  for (int i = 0 ; i < m_nNumVertices ; ++i) //for each vertex   
    m_vertices[i].n.normalize(); //normalize its normal
  
}

// Calculates the index into the triangle list of the triangle that 
// is located at (x,z) in world space  
/// \param x X coordinate in world space
/// \param z Z coordinate in world space
/// \return Index of the triangle located at (x,z)
int Terrain::getTriangleIndex(float x, float z)
{	
  int index=0; //return result
  // normalize origin to top right corner
  x += m_fOriginOffset; z += m_fOriginOffset;

  //normalize to grid units
  x /= m_fDelta; z /= m_fDelta;
  
  // Compute the grid square in
  int row = (int)x;
  int col = (int)z;
  int square = row * (m_nVPS - 1) + col;
  
  // Compute the offset within the square
  float xoffset = x - row;
  float zoffset = z - col;

  index = xoffset > zoffset ? square: square + m_nNumQuads;
  return index;
}

// sets heights of vertices on the terrain based on the height map
void Terrain::setTerrainFromHeightMap()
{  
	RenderVertexTerrain* v;
	for (int i = 0 ; i < m_nVPS; ++i)
	  for (int j = 0 ; j < m_nVPS ; ++j) 
	  {
		  v = &m_vertices[i*m_nVPS+j];
		  v->p.y = m_pHeightMap->m_fHeight[i][j];		  
		  calculateWeightsAtPoint(v->p.y,v->Weights1,v->Weights2);		                
	  }
}
