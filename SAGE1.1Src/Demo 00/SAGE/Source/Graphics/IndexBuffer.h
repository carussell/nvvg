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

/// \file IndexBuffer.h

#ifndef __INDEXBUFFER_H_INCLUDED__
#define __INDEXBUFFER_H_INCLUDED__

#include "resource/ResourceBase.h"
#include "common/Renderer.h"
#include <d3d9.h>

//-----------------------------------------------------------------------------
/// \class IndexBuffer
/// \brief Wrapper over the DirectX index buffer.
///
/// Wraps the DirectX index buffer for common usuage.
///
/// \remarks The buffer is dynamic, which will allow you to change the data if
/// needed.
class IndexBuffer : public ResourceBase
{
  friend class Renderer;

public:
  IndexBuffer(int triCount, bool isDynamic = false); ///< Basic constructor
  ~IndexBuffer(); ///< Basic destructor

  /// \brief Locks the index buffer, allowing you to write to it
  bool lock();

  /// \brief Unlocks the index buffer
  /// \return True if the unlock was successful, false otherwise
  /// \see lock
  bool unlock();

  /// \brief Index into the data buffer
  /// \param i Index of RenderTri to write to
  /// \return RenderTri at index i
  /// \warning To keep this operation fast, no bounds checking was included.
  /// Make sure the value of i is between 0 and the number of triangles
  /// stored by this buffer. \n \n
  /// Also, the operation does not check to see if the buffer is currently
  /// locked. Make sure the buffer is locked while writing to it.
  /// \see lock
  RenderTri &operator[] (int i) { return ((RenderTri*)m_data)[i]; }

  /// \brief Index into the data buffer
  /// \param i Index of RenderTri to write to
  /// \return RenderTri at index i
  /// \warning To keep this operation fast, no bounds checking was included.
  /// Make sure the value of i is between 0 and the number of triangles
  /// stored by this buffer. \n \n
  /// Also, the operation does not check to see if the buffer is currently
  /// locked. Make sure the buffer is locked while writing to it.
  /// \see lock
  const RenderTri &operator[] (int i) const { return ((RenderTri*)m_data)[i]; }

  /// \brief Get the number of triangles the buffer holds
  /// \return The number of triangles the buffer holds
  int getCount() { return m_count; }

  /// \brief Get whether the buffer is currently locked
  /// \return True if the buffer is currently locked, false otherwise
  bool isLocked() { return m_bufferLocked; }

private:
  int m_count; ///< Number of triangles stored
  BYTE *m_data; ///< Pointer to the buffer while locked
  bool m_bufferLocked; ///< Whether the buffer is locked
  bool m_dataEmpty; ///< Whether the buffer has been filled (locked) since the last restore()
  bool m_isDynamic;
  LPDIRECT3DINDEXBUFFER9 m_dxBuffer; ///< Pointer to the DX index buffer interface

  void release();
  void restore();
};
//-----------------------------------------------------------------------------

#endif