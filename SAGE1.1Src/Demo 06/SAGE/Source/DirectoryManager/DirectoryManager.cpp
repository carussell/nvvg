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

/// \file DirectoryManager.cpp
/// \brief Code for the DirectoryManager class.


#include "DirectoryManager.h"
#include "tinyxml/tinyxml.h"
#include "windows.h"

/// The global instance of DirectoryManager.  This is initated in the windows wrapper.
/// It can be used anywhere.
DirectoryManager gDirectoryManager; 

DirectoryManager::DirectoryManager()
{	
	m_activated = false;	
}


// loads in xml specifying location of directories
/// \param rootPath root directory where all the resources are
/// \param xmlFileName name of the xml file that contains the directory information
/// \return true if the xml file was found.  false if it wasn't.
bool DirectoryManager::initiate(std::string rootPath, std::string xmlFileName)
{
	int index;
	
	// save the root directory	
	m_rootPath = rootPath;
    
	m_activated = false;

	// the XML document object
	TiXmlDocument file;

	// load the directories xml
	if (!file.LoadFile(xmlFileName.c_str())) 
		return 0;

	// get the directories list
	TiXmlElement *dir = file.FirstChildElement("directories");
	
	// loop through every item and add the directory to the list
	TiXmlElement *list = dir->FirstChildElement();	
	while(list != 0)
	{
		// get the index from the string
		index = getResourceIndex(list->Value());				

		// save the directory into the directory array
		if (index != -1) 
			m_directories[index] = rootPath + list->Attribute("path");						
		
		list = list->NextSiblingElement();
	}

	m_activated = true;

	return true;
}

// sets current directory to root directory
void DirectoryManager::setRoot()
{
  SetCurrentDirectory(m_rootPath.c_str());

}

/// \param resource the type resource that you want to load e.g.(sounds, textures....)
void DirectoryManager::setDirectory(EDirectory resource)
{	
	
	// if the manager wasn't activated successfully then leave now 
	if (m_activated == false) return;

	// if the index is out of range then exit
	if (resource >= eDirectoryMax) return;
	if (resource < 0) return;
	
	SetCurrentDirectory(m_directories[resource].c_str());

	return;
}

/// \return Current working directory
std::string DirectoryManager::getDirectory()
{
	char buffer[1024];

	GetCurrentDirectory(1024, buffer);

	return buffer;
}

// convert string into an directory type
/// \param resourceName the value of the xml item from the xml file.
EDirectory DirectoryManager::getResourceIndex(std::string resourceName)
{	
	int index = -1;

	if (resourceName == "sounds")
		index = eDirectorySounds;
	else if (resourceName == "textures")
		index = eDirectoryTextures;
	else if (resourceName == "models")
		index = eDirectoryModels;
	else if (resourceName == "xmls")
		index = eDirectoryXML;
	else if (resourceName == "engine")
		index = eDirectoryEngine;
	else if (resourceName == "shaders")
		index = eDirectoryShaders;
	
		

	return (EDirectory)index;
}
