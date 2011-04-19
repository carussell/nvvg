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

/// \file ResourceManager.h

#ifndef __RESOURCEMANAGER_H_INCLUDED__
#define __RESOURCEMANAGER_H_INCLUDED__

#include <set>

// Forward declaration of the managed class
class ResourceBase;

//-----------------------------------------------------------------------------
/// \class ResourceManager
/// \brief Manager of resources (vertex buffers, index buffers, textures, etc.)
///
/// This class will release and restore resources when necessary. It tracks a
/// resource when that resource registers itself with the manager. The
/// ResourceBase class handles registering and unregistering in the constructor
/// and destructor.
///
/// \remark Although this class isn't a true singleton, it should be treated as
/// such. Don't create an instance of this class yourself; use the global
/// instance already created, gResourceManager
class ResourceManager
{
public:
  /// \brief Basic constuctor
  ResourceManager();

  /// \brief Basic destructor
  ~ResourceManager();

  /// \brief Register a resource to be managed
  void registerResource(ResourceBase *resource);
  
  /// \brief Unregister a resource
  void unregister(ResourceBase *resource);

  /// \brief Call release() on all managed objects
  void releaseAll();

  /// \brief Call restore() on all managed objects
  void restoreAll();

private:
  std::set<ResourceBase*> m_resources; ///< Set of all resources to manage

};
//-----------------------------------------------------------------------------

/// \brief Global instance of ResourceManager
extern ResourceManager gResourceManager;

#endif