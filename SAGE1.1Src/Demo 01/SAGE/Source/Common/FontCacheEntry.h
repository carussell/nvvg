/// \file FontCacheEntry.h
#ifndef __FONTCACHEENTRY_H_INCLUDED__
#define __FONTCACHEENTRY_H_INCLUDED__

#include "Resource/ResourceBase.h"
#include <d3d9.h>
#include <d3dx9core.h>

class FontCacheEntry : ResourceBase
{
public:
    FontCacheEntry();
    ~FontCacheEntry();

    LPD3DXFONT d3dFont;

    void release();
    void restore();
};



#endif