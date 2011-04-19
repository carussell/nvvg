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

#include "ResourceManager.h"
#include "ResourceBase.h"
#include <d3d9.h>
#include <assert.h>

ResourceManager gResourceManager;

ResourceManager::ResourceManager()
{
  m_resources.clear();
}

ResourceManager::~ResourceManager()
{
  // if we have any resources left over, something went wrong
  assert(m_resources.empty());
}

/// \remark It is so named because 'register' is a C++ keyword, and we could
/// not come up with another more suitable name
/// \param resource Pointer to a resource object
void ResourceManager::registerResource(ResourceBase *resource)
{
  m_resources.insert(resource);
}

/// \param resource Pointer to the resource object to unregister
void ResourceManager::unregister(ResourceBase *resource)
{
  if(m_resources.find(resource) != m_resources.end())
  {
    m_resources.erase(resource);
  }
}

void ResourceManager::releaseAll()
{
  std::set<ResourceBase*>::iterator iter;

  for(iter = m_resources.begin(); iter != m_resources.end(); iter++)
  {
    (*iter)->release();
  }
}

void ResourceManager::restoreAll()
{
  std::set<ResourceBase*>::iterator iter;

  for(iter = m_resources.begin(); iter != m_resources.end(); iter++)
  {
    (*iter)->restore();
  }
}