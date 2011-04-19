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

/// \file NameGenerator.h
/// \brief Interface for the NameGenerator class.

#ifndef __NAMEGENERATOR_H_INCLUDED__
#define __NAMEGENERATOR_H_INCLUDED__

#include <hash_map>
#include <hash_set>
#include <string>

/// \brief Generates and tracks unique names (strings). These names can be
/// requested and released according to the needs of the application.
class NameGenerator
{
  public:
    // Nested types
    
    bool requestName(const std::string &name);  ///< Requests a specific name from the generator.
    const std::string &generateName(const std::string &baseName);  ///< Generates a name given a base name.
    void releaseName(const std::string &name);  ///< Releases a name, making it available for future requests.
    void clear();  ///< Clears the set of allocated names.

    // Constructers/destructor

  private:
    typedef stdext::hash_set<std::string> NameSet;  ///< Represents a set of names.
    typedef NameSet::iterator NameSetIter;  ///< Set iterator.
    typedef stdext::hash_map<std::string,unsigned int> CounterMap;  ///< Represents a mapping of base names to ID counters.
    typedef CounterMap::iterator CounterMapIter;  ///< Map iterator.
    
    NameSet names;        ///< Holds the set of all allocated names.
    CounterMap counters;  ///< Holds the mapping of all base names to their ID counters.
};

#endif

