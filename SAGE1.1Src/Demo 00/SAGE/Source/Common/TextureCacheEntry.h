/// \file TextureCacheEntry.h
#ifndef __TEXTURECACHEENTRY_H_INCLUDED__
#define __TEXTURECACHEENTRY_H_INCLUDED__

#include <d3d9.h>
#include "resource/ResourceBase.h"

/// Texture cache variables.  See the notes above the Renderer::resetTextureCache()
/// for more details.
class TextureCacheEntry : ResourceBase
{
public:
  TextureCacheEntry(bool isManaged);
  ~TextureCacheEntry();

	// Symbolic name of the texture (usually a filename)

  std::string name;

	// Size

	int	xSize, ySize;

	// Direct3D interface object.  We're going to let D3D manage
	// the memory

	LPDIRECT3DTEXTURE9	d3dTexture; 

  /// When using a texture as a render target, you must aquire a surface object from
  /// the texture object.  You can then set the render target to that surface.
  /// After rendering, you must release the surface object.  d3dLockedSurface
  /// holds the surface object.  If it is NULL, the surface is not locked.
  LPDIRECT3DSURFACE9 d3dLockedSurface;

  /// If the texture is a rendertarget, a depth stencil may accompany it.  This
  /// surface will act as a Z-Buffer for the rendertarget.
  LPDIRECT3DSURFACE9 d3dDepthBuffer;


  void release();
  void restore();

  bool depthStencil; ///< True if a depth stencil is attached
  bool renderTarget; ///< True if this texture is a render target

};


#endif