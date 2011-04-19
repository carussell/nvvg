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

/// \file NameGenerator.cpp
/// \brief Code for the NameGenerator class.

#include <stdio.h>
#include "NameGenerator.h"

/// Requests a specific name from the name generator.  If the name is not
/// already allocated, the name will be considered generated.  When the
/// caller is finished with the name, the caller should release it.
/// \param name Specifies the requested name.
/// \return True iff the name request was granted.
bool NameGenerator::requestName(const std::string &name)
{
  if(names.find(name) != names.end())
    return false;
  names.insert(name);
  return true;
}

/// Generates a name given a base name.  The generated name is a concatenation
/// of the base name and a number.  For example, calling
/// <tt>generateName("Foo")</tt> three times will cause the generator to
/// generate "Foo1", "Foo2", and "Foo3" (assuming that none of these names
/// is already allocated).  When the caller is finished with the name,
/// the caller should release it.
/// \param baseName Specifies the base name.
/// \return A reference to the generated name.  If you want to keep the name
///     after clearing or discarding the set, be sure to store it in a string
/// rather than a reference-to-string.
const std::string &NameGenerator::generateName(const std::string &baseName)
{
  unsigned int counter = 0;

  // Get existing counter if this class has one
  CounterMapIter it = counters.find(baseName);
  if(it != counters.end())
    counter = it->second;
  
  // Generate names of the form (baseName + (string)counter)
  // until a unique one is found
  char buffer[80]; // more than enough
  std::string generatedName;
  do
  {
    //SECURITY-UPDATE:2/3/07
    //sprintf(buffer,"%u",++counter);
	sprintf_s(buffer,sizeof(buffer),"%u",++counter);
    generatedName = baseName + buffer;
  } while (names.find(generatedName) != names.end());
  
  // Update counter and add name
  counters[baseName] = counter;
  std::pair<NameSetIter,bool> rv = names.insert(generatedName);
  return *(rv.first);
}

/// \param name Specifies the name to be released.
/// \note If the name wasn't allocated, this function has no effect.
void NameGenerator::releaseName(const std::string &name)
{
  names.erase(name);
}

void NameGenerator::clear()
{
  names.clear();
  counters.clear();
}