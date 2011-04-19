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

/// \file ResourceBase.h
/// \brief Interface for the ResourceBase class.

#ifndef __RESOURCEBASE_H_INCLUDED__
#define __RESOURCEBASE_H_INCLUDED__

#include "ResourceManager.h"

//-----------------------------------------------------------------------------
/// \class ResourceBase
/// \brief Base class for any resource that needs to be managed.
///
/// A managed resource will benefit from being released and restored
/// automatically in the case of a lost device. A class derived from
/// ResourceBase will not need to worry about when the device is lost, only
/// what to do at that time. Any derived class will have to implement
/// ResourceBase::release() and ResourceBase::restore() to handle releasing
/// and restoring their resources.
class ResourceBase
{
  friend class ResourceManager;

public:
  /// \brief Registers with the resource manager, if necessary
  ResourceBase(bool isRegistered);

  /// \brief Unregisters the resource with the manager, if necessary.
  ~ResourceBase();

  /// \brief Returns whether the resource is registered with the ResourceManager
  /// \return Whether the resource is registered with the ResourceManager
  bool isRegistered() { return m_isRegistered; }

private:
  bool m_isRegistered; ///< Whether the resource is registered

protected:
  /// \brief Virtual release function
  ///
  /// This function is called by the resource manager (if the resource is
  /// managed)
  virtual void release() = 0;

  /// \brief Virtual restore function
  ///
  /// This function is called by the resource manager (if the resource is
  /// managed)
  virtual void restore() = 0;
};

#endif