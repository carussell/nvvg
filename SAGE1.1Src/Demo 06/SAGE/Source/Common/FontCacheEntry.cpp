/// \file FontCacheEntry.cpp
/// \brief Code for the FontCacheEntry class.

#include "FontCacheEntry.h"

FontCacheEntry::FontCacheEntry()
:ResourceBase(true) // register with the resource manager
{
  d3dFont = NULL;

}

FontCacheEntry::~FontCacheEntry()
{
  if (d3dFont) 
    d3dFont->Release();

  d3dFont = NULL;
}

void FontCacheEntry::release()
{
  if (d3dFont)
    d3dFont->OnLostDevice();

}

void FontCacheEntry::restore()
{
  if (d3dFont)
    d3dFont->OnResetDevice();

}