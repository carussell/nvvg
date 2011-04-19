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

/// \file ModelManager.cpp
/// \brief Code for the ModelManager class.

#include <list>
#include "DerivedModels/AnimatedModel.h"
#include "DerivedModels/ArticulatedModel.h"
#include "directorymanager/DirectoryManager.h"
#include "Common/EulerAngles.h"
#include "ModelManager.h"

ModelManager gModelManager;;

ModelManager::ModelManager()
{
}

ModelManager::~ModelManager()
{
  clear();
}

void ModelManager::clear()
{
  for(IDToModelMapIter it = m_idToModel.begin(); it != m_idToModel.end(); ++it)
    delete it->second;
  m_idToModel.clear();
  m_nameToID.clear();
  m_ids.clear();
};

/// \param fileName Specifies the name of the file containing the models.
/// \param defaultDirectory If true, specifies that \p fileName is relative
///     to the default XML directory.  If false, specifies that \p is relative
///     to the current directory.
/// \return Iff the import was successful, true.
bool ModelManager::importXml(const std::string &fileName, bool defaultDirectory)
{
  using namespace std;
  
  TiXmlDocument doc;
  if(defaultDirectory)
    gDirectoryManager.setDirectory(eDirectoryXML);
  if(!doc.LoadFile(fileName.c_str()))
    return false;
  TiXmlElement *root = doc.FirstChildElement("models");
  
  gDirectoryManager.setDirectory(eDirectoryModels);

  // Utility variables
  
  Vector3 v;
//  int i;
//  double d;
  Model *m = NULL;
  const char *cs = NULL;
  
  // Read models
  
  for(TiXmlElement *model = root->FirstChildElement("model"); model != NULL; model = model->NextSiblingElement("model"))
  {
    // Get main model attributes
    cs = model->Attribute("name");
    if(cs == NULL) continue; // Broken model entry; must have name
    string name = cs;
    if(m_nameToID.find(cs) != m_nameToID.end())
      continue; // Broken model entry; has same name as existing model
    cs = model->Attribute("type");
    string type = (cs == NULL) ? "normal" : cs; // type defaults to Model
    
    // Load model by type
    
    if(type == "normal")
    {
      // Load only frame
      TiXmlElement *frame = model->FirstChildElement("frame");
      if(frame == NULL) continue; // Broken model entry; must have a frame
      cs = frame->Attribute("fileName");
      if(cs == NULL) continue; // Broken model entry; frame must specify filename
      
      // Create the model
      m = new Model();
      m->importS3d(cs);
    }
    else if(type == "articulated")
    {
      // Count submodels
      int numSubmodels = 0;
      TiXmlElement *submodel;
      for(submodel = model->FirstChildElement("submodel"); submodel != NULL; submodel = submodel->NextSiblingElement("submodel"))
        ++numSubmodels;
      if(numSubmodels < 1) continue; // Broken model entry; must have at least one submodel
      
      // Create the model
      ArticulatedModel *am = new ArticulatedModel(numSubmodels);
      m = am;
      
      // Load only frame
      TiXmlElement *frame = model->FirstChildElement("frame");
      if(frame == NULL)
      {
        delete m;  // Broken model entry; must have a frame
        continue;
      }
      cs = frame->Attribute("fileName");
      if(cs == NULL)
      {
        delete m;  // Broken model entry; must have a filename
        continue;
      }
      m->importS3d(cs);
      
      // Load submodel data
      int submodelIndex = 0;
      for(submodel = model->FirstChildElement("submodel"); submodel != NULL; submodel = submodel->NextSiblingElement("submodel"))
      {
        // Count parts
        int numParts = 0;
        TiXmlElement *part;
        for(part = submodel->FirstChildElement("part"); part != NULL; part = part->NextSiblingElement("part"))
        {
          int firstIndex, lastIndex;
          cs = part->Attribute("first", &firstIndex);
          if(cs == NULL || firstIndex < 0) continue; // Broken part entry; must have first index
          cs = part->Attribute("last", &lastIndex);
          if(cs == NULL || lastIndex < firstIndex)
            lastIndex = firstIndex;
          numParts += lastIndex - firstIndex + 1;
        }
        am->setSubmodelPartCount(submodelIndex, numParts);
        // Add parts to submodel
        for(part = submodel->FirstChildElement("part"); part != NULL; part = part->NextSiblingElement("part"))
        {
          int firstIndex, lastIndex;
          cs = part->Attribute("first", &firstIndex);
          cs = part->Attribute("last", &lastIndex);
          if(cs == NULL || lastIndex < firstIndex)
            lastIndex = firstIndex;
          am->addPartToSubmodel(submodelIndex, firstIndex, lastIndex);
        }
        
        // Check for submodel offset
        
        TiXmlElement *offset = submodel->FirstChildElement("offset");
        if(offset != NULL)
        {
          getXmlVector3(*offset, v);
          am->moveSubmodel(submodelIndex, v);
        }
        ++submodelIndex;
      }
    }
    else if(type == "animated")
    {
      // Get frames
      std::list<const char *> frames;
      for(TiXmlElement *frame = model->FirstChildElement("frame"); frame != NULL; frame = frame->NextSiblingElement("frame"))
      {
        cs = frame->Attribute("fileName");
        if(cs != NULL)
          frames.push_back(cs);
      }

      // Count animations
      int numAnims = 0;
      TiXmlElement *anim;
      for(anim = model->FirstChildElement("anim"); anim != NULL; anim = anim->NextSiblingElement("anim"))
        ++numAnims;
        
      // Create the model
      AnimatedModel *am = new AnimatedModel((int)frames.size(), numAnims);
      m = am;
      am->importS3d(frames);

      // Get animations
      
      int animIndex = 0;
      for(anim = model->FirstChildElement("anim"); anim != NULL; anim = anim->NextSiblingElement("anim"))
      {
        std::list<int> anims;
        for(TiXmlElement *frameref = anim->FirstChildElement("frameref"); frameref != NULL; frameref = frameref->NextSiblingElement("frameref"))
        {
          int id;
          cs = frameref->Attribute("frame", &id);
          if(cs != NULL)
            anims.push_back(id);
        }
        am->setAnimationSequence(animIndex, anims);
        ++animIndex;
      }
    }
    else continue; // Broken model entry; type is invalid
    
    // Model created; add to the map
    unsigned int id = m_ids.generateID();
    m_nameToID[name] = id;
    m_idToModel[id] = m;
    m->cache();
  }

  return true;
}

/// \param name Specifies the name of the model.
/// \return The ID of the model.
unsigned int ModelManager::getModelID(const std::string &name)
{
  NameToIDMapIter it = m_nameToID.find(name);
  if(it == m_nameToID.end())
    return 0;
  else
    return it->second;
}

/// \param id Specifies the ID of the model.
/// \return A pointer to the model.
Model *ModelManager::getModelPointer(unsigned int id)
{
  IDToModelMapIter it = m_idToModel.find(id);
  if(it == m_idToModel.end())
    return NULL;
  else
    return it->second;
}

/// \param name Specifies the name of the model.
/// \return A pointer to the model.
Model *ModelManager::getModelPointer(const std::string &name)
{
  return getModelPointer(getModelID(name));
}

/// \param elem Specifies the XML element to be queried.
/// \param v References the vector to be filled.  Any missing
///     or invalid coordinates will be zeroed.
/// \note This function expects an XML element with attributes
///     named <tt>x</tt>, <tt>y</tt>, and <tt>x</tt>.
///     For example:
///     \code <offset x="1.0f" z="-12.5" /> \endcode
///     will set the vector to [1.0f, 0.0f, -12.5].
void ModelManager::getXmlVector3(TiXmlElement &elem, Vector3 &v)
{
  double d;
  if(elem.Attribute("x",&d))
    v.x = (float)d;
  else
    v.x = 0.0f;
  if(elem.Attribute("y",&d))
    v.y = (float)d;
  else
    v.y = 0.0f;
  if(elem.Attribute("z",&d))
    v.z = (float)d;
  else
    v.z = 0.0f;
}