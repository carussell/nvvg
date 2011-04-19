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

/// \file ModelManager.h
/// Defines the object management class
/// Last updated July 12, 2005

#ifndef __MODELMANAGER_H_INCLUDED__
#define __MODELMANAGER_H_INCLUDED__

#include <hash_map>
#include <string>
#include "TinyXML/tinyxml.h"
#include "generators/IDGenerator.h"

class EulerAngles;
class Model;
class Vector3;

/// \brief Manages a group of objects for a game.
class ModelManager
{
public:
  ModelManager();  ///< Constructs a new manager.
  ~ModelManager();  ///< Frees all model resources and destroys the manager.
  
  void clear();  ///< Frees all model resources and clears the manager.
  
  bool importXml(const std::string &fileName, bool defaultDirecotry = true);  ///< Imports models from an XML file.

  unsigned int getModelID(const std::string &name);  ///< Queries the manager for a model's ID.
  Model *getModelPointer(unsigned int id);  ///< Queries the manager for a model's pointer.
  Model *getModelPointer(const std::string &name);  ///< Queries the manager for a model's pointer.
  
private:

  typedef stdext::hash_map<std::string, unsigned int> NameToIDMap;  ///< Maps model names to IDs.
  typedef NameToIDMap::iterator NameToIDMapIter;  ///< Map iterator.
  typedef stdext::hash_map<unsigned int, Model *> IDToModelMap;  ///< Maps model IDs to models.
  typedef IDToModelMap::iterator IDToModelMapIter;  ///< Map iterator.
  
  static void getXmlVector3(TiXmlElement &elem, Vector3 &v);  ///< Reads a vector as "x", "y", and "z" values of an XML element.
  
  NameToIDMap m_nameToID;  ///< Maps the model names to their IDs.
  IDToModelMap m_idToModel;  ///< Maps the model IDs to the models.
  IDGenerator m_ids;  ///< Generates the model IDs.
};

extern ModelManager gModelManager;

#endif

