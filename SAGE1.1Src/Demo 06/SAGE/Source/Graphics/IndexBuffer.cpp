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

/// \file IndexBuffer.cpp
/// \brief Code for the IndexBuffer class.

#include "IndexBuffer.h"

extern LPDIRECT3DDEVICE9 pD3DDevice;


/// \param triCount Number of triangles the index buffer should hold.
/// \param isDynamic Whether the buffer should be dynamic, default is false
/// \remark triCount is the number of triangles, not the number of vertex
/// indices, the index buffer should hold. For example, to store a quad
/// (two triangles, four vertices), triCount should be two, not six.
IndexBuffer::IndexBuffer(int triCount, bool isDynamic)
: ResourceBase(isDynamic),
  m_count(triCount),
  m_bufferLocked(false),
  m_isDynamic(isDynamic)
{
  restore();
}

IndexBuffer::~IndexBuffer()
{
  release();
}

/// Locks the entire buffer for writing using the common parameters for
/// dynamic buffers.
/// \code
/// IndexBuffer ib(2);
/// 
/// if( !ib.lock() ) 
/// {
///   // failed to lock the buffer
/// }
/// 
/// ib[0].index[0] = 0;
/// ib[0].index[1] = 1;
/// ib[0].index[2] = 2;
/// ...
///
/// ib.unlock();
/// \endcode
/// \return True if the lock was successful, false otherwise
bool IndexBuffer::lock()
{
  if(m_dxBuffer == NULL || m_bufferLocked)
  {
    return false;
  }

  if( FAILED( m_dxBuffer->Lock(
    0, 0, (void**)(&m_data), m_isDynamic ? D3DLOCK_DISCARD : 0) ) )
  {
    // you may want to abort here
    return false;
  }

  m_bufferLocked = true;
  m_dataEmpty = false;
  return true;
}

bool IndexBuffer::unlock()
{
  if(m_dxBuffer == NULL || !m_bufferLocked)
  {
    return false;
  }

  if( FAILED( m_dxBuffer->Unlock() ) )
  {
    return false;
  }

  m_bufferLocked = false;
  return true;
}

void IndexBuffer::release()
{
  if(m_dxBuffer != NULL)
  {
    m_dxBuffer->Release();
    m_dxBuffer = NULL;
  }
}

void IndexBuffer::restore()
{
  // if dynamic is requested or if we are in reference mode for debugging shaders
  if(m_isDynamic || gRenderer.getDeviceReference())
  {
    if( FAILED( pD3DDevice->CreateIndexBuffer(
      m_count * sizeof(RenderTri),
      D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
      D3DFMT_INDEX16,
      D3DPOOL_DEFAULT,
      &m_dxBuffer,
      NULL) ) )
    {
      m_dxBuffer = NULL;
    }
  }
  else
  {
    if( FAILED( pD3DDevice->CreateIndexBuffer(
      m_count * sizeof(RenderTri),
      0,
      D3DFMT_INDEX16,
      D3DPOOL_MANAGED,
      &m_dxBuffer,
      NULL) ) )
    {
      m_dxBuffer = NULL;
    }
  }

  m_bufferLocked = false;
  m_dataEmpty = true;
}