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

/// \file HeightMap.cpp
/// \brief Code for the HeightMap class.

#include <math.h>
#include "heightmap.h"
#include "common/random.h"
#include "directorymanager/directorymanager.h"
#include "common/bitmap.h"
#include "common/commonstuff.h"

extern CRandom Random; // Extern the random number generator

/// Creates a height map of size side*side.  All height are initially zero.
/// \param side Size of one side of the 2 dimensional height map.  The height
/// map size will be side * side.
HeightMap::HeightMap(int side):m_nSide(side)
{
  // Create 2D array
  m_fHeight = new float*[m_nSide];
  for(int i=0; i<m_nSide; i++)
    m_fHeight[i] = new float [m_nSide];

  // Set the height to zero
  Clear();
}

/// Creates a height map from an image file.  Each pixel in the image
/// represents a height.  Black represents 0.0 and white represents 1.0.  The
/// value is then scaled by the maxHeight parameter which creates a heights of
/// range 0.0 to maxHeight.
/// \param fileName Name of the image file.
/// \param maxHeight Value that is used to scale all the heights.
/// \param defaultDirectory Whether to load the image from the default
/// texture directory.
HeightMap::HeightMap(const char* fileName, float maxHeight, bool defaultDirectory) //constructor
{
	Bitmap bitmap;
	char	text[256];
	
  if (defaultDirectory)
	  gDirectoryManager.setDirectory(eDirectoryTextures);
		
	// Load up the image
  //SECURITY-UPDATE:2/3/07
  //if (!bitmap.load(fileName, text)){
  if (!bitmap.load(fileName, text,sizeof(text))){
		ABORT("Can't load texture %s.  %s.", fileName, text);
  }
	// It must be 32-bit
	if (bitmap.format() != Bitmap::eFormat_8888) {
		ABORT("Can't load texture %s.  Only 32-bit textures supported.", fileName);
	}

	m_nSide = bitmap.xSize() + 1;

	m_fHeight = new float*[m_nSide];
	for(int i=0; i<m_nSide; i++)
	 m_fHeight[i] = new float [m_nSide];	 
	
	for (int y = 0 ;y < m_nSide - 1; y++)
		for (int x = 0 ;x < m_nSide - 1; x++)
		{
			m_fHeight[y][x] = ((float)(0x000000FF & bitmap.getPix(x,y)) / 256.0f) * maxHeight;
		}

  // Add skirt
  for (int x = 0; x < m_nSide; x++)
  {
    m_fHeight[m_nSide - 1][x] = m_fHeight[m_nSide - 2][x];
    m_fHeight[x][m_nSide - 1] = m_fHeight[x][m_nSide - 2];
  }
	
} // End of function

// Deallocates heightmap array
HeightMap::~HeightMap()
{
  // delete allocated array
  for(int i=0; i<m_nSide; i++)
    delete [] m_fHeight[i];
  delete m_fHeight;  
  m_fHeight = NULL;
}

// resets all heights to zero
void HeightMap::Clear()
{ 
  for (int i = 0 ; i < m_nSide ; ++i)
	  for (int j = 0 ; j < m_nSide ; ++j) 
		  m_fHeight[i][j] = 0.0f;
}