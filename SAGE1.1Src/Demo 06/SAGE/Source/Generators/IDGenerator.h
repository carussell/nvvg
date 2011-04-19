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

/// \file IDGenerator.h
/// \brief Interface for the IDGenerator class.

#ifndef __IDGENERATOR_H_INCLUDED__
#define __IDGENERATOR_H_INCLUDED__

#include <hash_set>

/// \brief Generates unique ids in the form of unsigned ints.  Useful for resource factories/managers.
class IDGenerator
{
public:
  // Nested types
  
  // Constructers/destructor
  
  IDGenerator();  ///< Constructs a fresh generator.

  // Member functions
  
  unsigned int generateID(); ///< Generates a new ID.
  void releaseID(unsigned int id);  ///< Releases an ID.
  void clear(); /// Clears the set of allocated IDs.
  
  const static unsigned int NULLID = 0; ///< Represents an invalid or nonexistent ID.
private:
  
  typedef stdext::hash_set<unsigned int> IDSet; ///< Represenets a set of IDs.
  
  unsigned int m_idCounter;  ///< Holds last allocated ID. 
  bool m_hasCounterWrapped;  ///< Holds true iff ID counter has wrapped around the range of IDs (not likely).
  IDSet m_ids;               ///< Holds the set of allocated IDs.
};

#endif

