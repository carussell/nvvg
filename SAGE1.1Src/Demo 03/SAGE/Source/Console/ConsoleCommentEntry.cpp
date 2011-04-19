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


#include "ConsoleCommentEntry.h"
#include "TinyXML/tinyxml.h"
#include <iostream>
#include "console.h"
using namespace std;


// loads comments from a xml element
/// \param command Element that the command's comment information is in
/// \return false if a data type in the XML is not recognize	
bool ConsoleCommentEntry::loadComments(TiXmlElement* command)
{
	TiXmlElement* parameter = NULL;

	// save the name of the command and comment
	functionName = command->Value();
	description = command->Attribute("comment");

	// find the number of parameters
	parameter = command->FirstChildElement();
	
	// start off with no parameters
	parameters = ""; 
	numParameters = 0;
	
	// loop through every parameter of the command
	parameter = command->FirstChildElement();	
		
	// add every parameter comment
	while(parameter != 0)
	{		
		char toAdd = getCharFromType(parameter->Value());
		if (toAdd == ' ') 
			return 0;
		
		// add a letter for each parameter
		parameters += toAdd;

		parameterDescriptions[numParameters] = parameter->Attribute("comment");
		parameter = parameter->NextSiblingElement();		
		numParameters++;
	}
	
	
	return 1;
}


void ConsoleCommentEntry::printInformation()
{
	
	// print command to console
	gConsole.printLine( "Command: " + functionName + + " " + getParameterListFromString(parameters));

	// print definition	
	gConsole.printLine("Definition: " + description);

	// go through every parameter
	for (int i = 0 ; i < numParameters; i++)
	{	
		gConsole.printLine(getTypeFromChar(parameters[i]) + " : " + parameterDescriptions[i]);		
	}	


	return;
}


