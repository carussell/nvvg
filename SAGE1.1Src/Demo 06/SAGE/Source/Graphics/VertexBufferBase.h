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

/// \file VertexBufferBase.h
/// \brief Interface for the VertexBufferBase class.

#ifndef __VERTEXBUFFERBASE_H_INCLUDED__
#define __VERTEXBUFFERBASE_H_INCLUDED__

#include "resource/ResourceBase.h"
#include <d3d9.h>

//-----------------------------------------------------------------------------
/// \class VertexBufferBase
/// \brief Base class for any vertex buffer
///
/// This is the base class for any vertex buffer. It handles the DirectX calls
/// for creating, releasing, locking, and unlocking the buffer. The benefits of
/// using this class are simplified usuage and strong-typed buffers (once you
/// derive)
class VertexBufferBase : public ResourceBase
{
  friend class Renderer;

public:
  VertexBufferBase(int count, bool isDynamic, DWORD fvf, int vertexStride);
  ~VertexBufferBase();

  bool lock();
  bool unlock();

  int getCount() { return m_count; }
  bool isLocked() { return m_bufferLocked; }
  bool isEmpty() { return m_dataEmpty; }

protected:
  int m_count;
  const DWORD m_FVF;
  const int m_vertexStride;
  BYTE *m_data;
  bool m_bufferLocked;
  bool m_dataEmpty; ///< Whether the buffer has been filled (locked) since the last restore()
  bool m_isDynamic;
  LPDIRECT3DVERTEXBUFFER9 m_dxBuffer;

  void release();
  void restore();
};
//-----------------------------------------------------------------------------

#endif