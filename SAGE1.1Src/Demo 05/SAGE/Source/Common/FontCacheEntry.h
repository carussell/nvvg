/// \file FontCacheEntry.h
#ifndef __FONTCACHEENTRY_H_INCLUDED__
#define __FONTCACHEENTRY_H_INCLUDED__

#include "Resource/ResourceBase.h"
#include <d3d9.h>
#include <d3dx9core.h>

/// \brief Encapsulates the Direct3D Font class. Since this is derived
/// from ResourceBase, managing this font is automated.
class FontCacheEntry : ResourceBase
{
public:
    FontCacheEntry(); ///< Constructs a FontCacheEntry object.
    ~FontCacheEntry(); ///< Destructs a FontCacheEntry object.

    LPD3DXFONT d3dFont; ///< Pointer to the Direct3D font interface.

    void release(); ///< Releases the font resource.
    void restore(); ///< Restors the font resource.
};



#endif