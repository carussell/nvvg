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

/// \file DirectoryManager.h
/// Manages the organization of resources in subfolders.
/// Last updated July 14th, 2005

#ifndef __DIRECTORYMANAGER_H_INCLUDED__
#define __DIRECTORYMANAGER_H_INCLUDED__

#include <string>


/// All the types of directories.  You can add your own directories
/// and nest directories.
enum EDirectory
{
	eDirectoryTextures, ///< Any texture for the game
	eDirectoryModels, ///< Any model for the game
	eDirectoryXML, ///< Any XML for the game
	eDirectoryMax // make sure this is last
};

/// Manages the organization of resources in subfolders.  The Directory Manager
/// is told what resources are stored where at initiation by an XML file.
/// SetDirectory allows the user to set the current directory by specifying a
/// resource from the enumeration EDirectory.

class DirectoryManager
{
public:
	DirectoryManager(); ///< Constructor	

	/// Opens up the xml file specified and loads all the directories	
	bool initiate(std::string rootPath, std::string xmlFileName); 

  /// Sets the current directory to the root directory
  void setRoot();

	/// Sets the current directory to a specified resource type.
	/// example: the following call would set the current directory to the 
	/// sound directory specified in the XML file passed in at initate	
	void setDirectory(EDirectory resource);

	/// \brief Returns the current working directory
	std::string getDirectory();


private:

	/// \brief Converts the string to a Directory type		
	EDirectory getResourceIndex(std::string resourceName);

	bool m_activated; ///< true if the xml file has been loaded

	std::string m_directories[eDirectoryMax]; ///< holds the pathes of all the resources

	/// \brief the path to the root directory where all the resources are.	
	std::string m_rootPath; 

};

extern DirectoryManager gDirectoryManager;




#endif