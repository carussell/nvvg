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

/// \file ConsoleFunctionEntry.h
#ifndef __H_CONSOLEFUNCTIONENTRY_INCLUDED__
#define __H_CONSOLEFUNCTIONENTRY_INCLUDED__

#include <iostream>
#include "ConsoleDefines.h"
#include "ConsoleCommentEntry.h"

/// Type definition for function pointer used to call custom commands
typedef bool (*consoleFunctionPointer) (ParameterList* params,std::string* errorMessage);

/// \class ConsoleFunctionEntry
/// \brief Holds all information about a command instance
//
/*!
When a user creates a command using the Console::addFunction method,
they indirectly create an instance of ConsoleFunctionEntry.
*/
class ConsoleFunctionEntry
{
public:
	ConsoleFunctionEntry(); ///< Initiates variables to defaults

	/// \brief Prints detailed information about the command to the console
	void printDetailedInformation();
	
	/// \brief Prints simple information about the command to the console
	void printSimpleInformation();

	/// \brief Prints problems found with commands			
	void printProblems(bool criticalOnly);

	/// \brief Returns a string in which each character represents a 
	/// parameter type
	/// \return String in which each character represents a parameter type
	std::string getParameters() {return m_parameters;}

	/// \brief Calls the function pointer that was defined in the 
	/// DefineFunction method		
	bool callFunction(ParameterList* pList, std::string* errorMessage);


	/// \brief Sets the name of the command, parameters required and a pointer
	/// to the function to invoke	
	void DefineFunction(std::string funcName, std::string parameters, consoleFunctionPointer functionPointer);
	
	/// \brief Sets the comment entrees	
	void DefineComments(const ConsoleCommentEntry* pComments);

	/// \brief Returns true if the function has been declared in code
	/// \return True if the function has been declared in code
	bool functionDefined() { return m_loadedFunction;}

	/// \brief Returns true if the comments for this command have been decalred
	/// in an XML
	/// \return Rrue if the comments have been declared
	bool commentDefined() { return m_loadedComments;}

	/// \brief Returns the name of the command
	/// \return Name of the command
	std:: string getName() {return m_functionName;}

private:
	
	bool m_loadedComments; ///< True if comments have been loaded from and XML
	bool m_loadedFunction; ///< True if function has been declared in code

	ConsoleCommentEntry m_comments; ///< Holds comments loaded from XML

	std::string m_functionName; ///< Command name
	
	/// \brief Parameters the function takes (one char for each parameter)
	std::string m_parameters;
	
	consoleFunctionPointer m_funcPtr; ///< Pointer to the function
};



#endif