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

/// \file ConsoleFunctionEntry.cpp
/// \brief Code for the ConsoleFunctionEntry class.

#include "ConsoleFunctionEntry.h"
#include "console.h"

using namespace std;

ConsoleFunctionEntry::ConsoleFunctionEntry()
{
	m_loadedComments = false;
	m_loadedFunction = false;
	m_funcPtr = NULL;
}

// invoke the function
/// Calls the function set passing through the list of parameters and
/// the pointer to the errorMessages string
/// \param pList Values and info about the parameters
/// \param errorMessage Holds log of messages to print to console
/// \return false if errorMessage needs to be printed to console
bool ConsoleFunctionEntry::callFunction(ParameterList* pList, std::string* errorMessage)
{
	if (m_funcPtr == NULL) 
	{
		(*errorMessage) = "Function '";
		(*errorMessage) += m_functionName;
		(*errorMessage) += "' is NULL";
		return 0;
	}

	return m_funcPtr(pList, errorMessage);
}

// sets the comments for the function
/// \param pComments Address of a comment object to copy
void ConsoleFunctionEntry::DefineComments(const ConsoleCommentEntry* pComments)
{
	
	if (pComments == NULL) 
		return;

	// save information
	m_comments = *pComments;	
	m_functionName = m_comments.functionName;
	m_parameters = m_comments.parameters;

	// flag that comments are loaded
	m_loadedComments = true;
    
	return;
}
/// \param funcName Name of command
/// \param parameters Parameters required.  Each character represents a 
/// data type
/// \param functionPointer Pointer to a function to invoke
void ConsoleFunctionEntry::DefineFunction(std::string funcName, std::string parameters, consoleFunctionPointer functionPointer)
{	 	
	m_functionName = funcName;
	m_parameters = parameters;
	m_funcPtr = functionPointer;
	m_loadedFunction = true;
}


void ConsoleFunctionEntry::printDetailedInformation()
{
	string output;

	// if there are not comments just print out simple information
	if (!m_loadedComments)
	{
		printSimpleInformation();
		output = m_functionName;
		output += " - Detailed information unavailable.  Update XML file";		
		gConsole.printLine(output);				
	}	
	// if there are comments
	else  
	{		
		
		m_comments.printInformation();	
	}
	

	return;
}

void ConsoleFunctionEntry::printSimpleInformation()
{
	// used to log output to console
	string output; 

	// if the functino isn't loaded then return
	if (!m_loadedFunction)		
		return;
	
	output = "Command: " + m_functionName + " ";	
	output += getParameterListFromString(m_parameters);

	gConsole.printLine(output);	

	return;
}


/*!
This function helps users find problems with their console functions.

Critical errors:
A function is commented in an XML but is not declared in code

Non-critical errors:
A function is declared in code but not commented	
*/
/// \param criticalOnly true specfies that only critical errors should
/// be printed
void ConsoleFunctionEntry::printProblems(bool criticalOnly)
{
	string output;
		
	// if the comments are loaded but the function is not
	// this is a critical problem
	if (m_loadedComments && !m_loadedFunction)
	{
		output = "Command: '" + m_functionName + " ";		
		output += getParameterListFromString(m_parameters);
		output += "' is commented in an XML but does not exist in code.";
		gConsole.printLine(output);
	}
	if (m_loadedFunction && !m_loadedComments && !criticalOnly)
	{
		output = "Command: '" + m_functionName + " ";		
		output += getParameterListFromString(m_parameters);
		output += "' is not commented in an XML file";
		gConsole.printLine(output);
	}

	return;
}
