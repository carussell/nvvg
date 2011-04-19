/// \file TextureCacheEntry.cpp
/// \brief Code for the TextureCacheEntry class.

#include "TextureCacheEntry.h"
#include "CommonStuff.h"
extern LPDIRECT3DDEVICE9 pD3DDevice;

TextureCacheEntry::TextureCacheEntry(bool isManaged)
:ResourceBase(!isManaged)
{
  d3dTexture = NULL;
  d3dLockedSurface = NULL;
  renderTarget = NULL;
  d3dDepthBuffer = NULL;
  depthStencil = 0;

}
TextureCacheEntry::~TextureCacheEntry()
{
  release();
}

void TextureCacheEntry::release()
{
  if (d3dTexture)
    d3dTexture->Release();

  if (d3dLockedSurface)
    d3dLockedSurface->Release();

  if (d3dDepthBuffer)
    d3dDepthBuffer->Release();

  depthStencil = 0;
  d3dDepthBuffer = NULL;
  d3dTexture = NULL;
  d3dLockedSurface = NULL;


}

void TextureCacheEntry::restore()
{

	// compute usage
	DWORD usage = D3DUSAGE_AUTOGENMIPMAP;
	D3DPOOL pool = D3DPOOL_MANAGED;
  UINT levels = 0;

	if (renderTarget) 
	{
		usage = usage | D3DUSAGE_RENDERTARGET;
		pool = D3DPOOL_DEFAULT;
    levels = 1;   
	}

	// Allocate D3D texture
	HRESULT result = pD3DDevice->CreateTexture(
		xSize,			// width
		ySize,			// height
		levels,			// number of miplevels - zero for automatic mip-map generation
		usage,	// usage - autogen mipmaps and maybe rendertarget
		D3DFMT_A8R8G8B8,	// pixel format - we will only use 32-bit
		pool,	// memory pool - we'll let D3D manage the textures so we don't have to "restore."
		&d3dTexture,		// return pointer to texture interface object here
		NULL // this parameter must be NULL
	);

	// Check for failure.  We won't handle errors gracefully.

	if (FAILED(result)) {
		ABORT("Can't allocate %dx%d 32-bit texture", xSize, ySize);
	}



  // Create a depth stencil if requested
  if (depthStencil)
  {
    // acceptable formats in order of preference
    D3DFORMAT formats[] = {D3DFMT_D32, D3DFMT_D16};

    for (int a = 0; a < 2; a++)
    {  
      result = 
      pD3DDevice->CreateDepthStencilSurface(
        xSize,
        ySize,
        formats[1],
        D3DMULTISAMPLE_NONE,
        NULL,
        true,
        &d3dDepthBuffer,
        NULL
        );

      // leave if a buffer was successfully created
      if (SUCCEEDED(result)) break;
      
    }
    if (FAILED(result)) // if a depth buffer could not be created
    {
      ABORT("Can't create a depth buffer", xSize, ySize);
    }
  }



}