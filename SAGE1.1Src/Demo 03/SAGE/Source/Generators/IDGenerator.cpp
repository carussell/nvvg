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


#include "IDGenerator.h"

IDGenerator::IDGenerator()
    : m_idCounter(0), m_hasCounterWrapped(false)
{}

/// \return A new ID
/// \remark The new ID is guaranteed to not have been generated without
///    a subsequent, corresponding release; furthermore, it is guaranteed
///    to not be generated again until released.  When the caller is finished
///    with an ID, the caller should release it.
/// \warning Theoretically, this function will lock up if all possible IDs
///    are allocated at once without a release.  This is unlikely, as the
///    number of IDs that can be generated in a 32-bit environment is over
///    four billion.  Also, theoretically, generation no longer happens in
///    constant time after every ID has been generated once.
unsigned int IDGenerator::generateID()
{
  unsigned int oldID = m_idCounter++;

  // Check if this wraps the ID counter around the range of an unsigned int  
  if(m_idCounter < oldID)
  {
    m_idCounter = 1;
    m_hasCounterWrapped = true;
  }
  if(!m_hasCounterWrapped)
  {
    m_ids.insert(m_idCounter);
    return m_idCounter;
  }

  // Counter has wrapped; must check for duplicate IDs
  // Only happens in the greatest of games....
  // ASSUME:  No game has every single possible object ID at once

  while(m_ids.find(m_idCounter) != m_ids.end())
    ++m_idCounter;
  m_ids.insert(m_idCounter);
  return m_idCounter;
}

/// \param id Specifies the ID to be released.
/// \remark If id hasn't been generated, does nothing.
void IDGenerator::releaseID(unsigned int id)
{
  m_ids.erase(id);
}

void IDGenerator::clear()
{
  m_ids.clear();
  m_idCounter = 0;
}
