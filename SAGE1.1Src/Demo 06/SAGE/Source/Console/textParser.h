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

/// \file TextParser.h
/// \brief Interface for the TextParser class.


#ifndef __H_TEXTPARSER_INCLUDED__
#define __H_TEXTPARSER_INCLUDED__

#include <string>
#include "common/vector3.h"
#include "ConsoleDefines.h"
#include "ParameterList.h"

//-------------------------------------------------------------------------
/// Specifies all the types that the console can understand
enum EDataType
{
	eDataTypeString = 's', ///< string data type
	eDataTypeInt = 'i', ///< integer data type
	eDataTypeFloat = 'f', ///< float data type
	eDataTypeBool = 'b', ///< boolean data type
	eDataTypeVector3 = 'v'	///< vector3 data type
};
//-------------------------------------------------------------------------


//-------------------------------------------------------------------------
/// this structure is capable of holding all types of data.
//
/// The MultiVariable structure is an easy way to hold data
/// of unknown types.
struct MultiVariable
{
	EDataType type; ///< type of data that the multivariable is holding
	std::string typeString; ///< if type is a string then this is filled in
	Vector3 typeVector3; ///< if type is a vector3 then this is filled in
	int typeInt; ///< if type is a integer then this is filled in
	float typeFloat; ///< if type is a float then this is filled in
	bool typeBool; ///< if type is a bool then this is filled in
};
//-------------------------------------------------------------------------



//-------------------------------------------------------------------------

/// \class TextParser
/// \brief Used to parse a string into a list of data types
//
/*!
To use the text parser:\n
1. Create an instance of the TextParser object.\n
2. Call the parse member function passing a string you want to parse.\n

Once the text is parsed, you can view the the results through public member 
functions.

*/
class TextParser
{
public:
	//-------------------------------------------------------------------------
	TextParser(); ///<Basic Constructor. Initiates variables to defaults
	~TextParser(); ///<Basic Destructor. Deletes the tokens array


	/// \brief Parses specified text into a list of types and values		
	bool parse(std::string text); ///< parses the text and stores the results

	/// \brief If parse fails (returns 0) an error message will be in
	/// this string message
	std::string errorMessage; 

	/// \brief Returns a string in which each character represents
	/// a data type.  i.e. "sif" means string, int, float.
	std::string getTypes() {return m_typeList;}
	
	
	/// \brief Fills in the ParameterList structure with the data
	/// information obtained from the parse.	
	void feedParameterList(ParameterList* pList); 

	
private:
	//-------------------------------------------------------------------------
	
	/// \brief Used to evaluate parenthesis
	bool giveTokenFromTextParser(TextParser* tp, MultiVariable* token);

	/// \brief Returns a float for a specified string, returns 0 on failure
	bool giveFloat(float* result, std::string);
	
	/// \brief Returns an integer for a specified string, returns 0 on failure
	bool giveInt(int* result, std::string); 

	/// \brief Returns a boolean for a specified string, returns 0 on failure
	bool giveBool(bool* result, std::string); 

	MultiVariable* m_tokens; ///< Holds all the tokens after a parse
	int m_numTokens; /// Size of tokens array
	
	/// \brief Holds a string in which each character represents a data type.
	std::string m_typeList; 
};



#endif