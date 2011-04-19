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

/// \file ConsoleDefines.h
/// Some global functions
#ifndef __H_CONSOLEDEFINES_INCLUDED__
#define __H_CONSOLEDEFINES_INCLUDED__

#include <string>


#include "ParameterList.h"

/// \brief Transforms a string the spells out a type to a single char
//
/// Examples: "string" -> "s", "float" ->f 
/// \param type Type as a string
/// \return Character that represents the type
char getCharFromType(std::string type);

/// \brief Transforms a char representing a type to a string that spells out
/// that type
//
/// Examples: 's' -> "string", 'f' -> "float" 
/// \param c Character representing a type
/// \return Spelled out type that c represented
std::string getTypeFromChar(char c);

/// \brief Transforms a string of characters representing
/// types to a list of types seperated by commas
/// \param s Each character represents a type
/// \return List of types spelled out seperated by commas
std::string getParameterListFromString(std::string s);



#endif
