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

#pragma warning(disable : 4996)
/// \file textParser.cpp
#include "TextParser.h"
#include <queue>
#include "ParameterList.h"
using namespace std;



// constructor
TextParser::TextParser()
{
	m_tokens = NULL;
	m_numTokens = 0;
}

// destructor
TextParser::~TextParser()
{
	// deallocate token array
	if (m_tokens)
		delete []m_tokens;
	m_tokens = NULL;
	m_numTokens = 0;
}

/// TextParser parse.
/// parse() should be called before you start you try to do anything
/// else with the TextParser.
/// \param text Line of text that you want to parse
/// \return true on success, if false is returned, a errorMessage 
/// can is stored in public member variable errorMessage.
bool TextParser::parse(std::string text)
{
	// delete previous m_tokens
	if (m_tokens) 
	{
		delete []m_tokens;
		m_tokens = NULL;
	}

	// create a queue so we can add m_tokens as we go
	queue<MultiVariable> qTokens;

	MultiVariable token; // used to push m_tokens on the queue

	int strLength = (int)text.length();

	const char* s = text.c_str();

	if (text == "")
	{
		m_tokens = NULL;
		m_numTokens = 0;
		m_typeList = "";
		return 1;
	}
	

	// parse the entire
	bool done = false;
	int index = 0;  //index into the string
	while(!done)
	{		

		// if a quote is found
		if (s[index] == '"')
		{			
			// increment the index passed the quote
			index++;

			// remember where we started
			int startIndex = index;
			
			// search for a a corresponding end quote or the end of the string
			while (!(s[index] == '"' || index == strLength))
			 index++; 
			

			// if the end of file was found before and ending quote then
			// return an error
			if (index == strLength)
			{
				errorMessage = "Open quote without ending quote";
				return 0;
			}

			// now there is a string waiting between startIndex and index
			// put it in a string
			token.typeString = text.substr(startIndex,index - startIndex);
			token.type = eDataTypeString;
			
			// push it on the stack
			qTokens.push(token);

			//increment passed ending quote
			index++;
		}
		// if a closing parenthesis was found then there is a problem
		else if (s[index] == ')')
		{
			errorMessage = "Close parenthesis before open parenthesis";
			return 0;
		}

		// if a parenthesis was found
		else if (s[index] == '(')
		{
			// parse till the end parenthesis is found and then 
			// parse the inside of it
			
			index++;
			int startIndex = index;

			
			// search for a breaking character or the end of the string
			while (!(s[index] == ')' || index == strLength))
			 index++; 

			if (index == strLength)
			{
				errorMessage = "Open quote without ending quote";
				return 0;
			}

            // now there is a string waiting between startIndex and index
			// put it in a string
			string sToken = text.substr(startIndex,index - startIndex);

			// parse the inside
			TextParser tp;
			tp.parse(sToken);

			// get the token from the string
			if (!giveTokenFromTextParser(&tp, &token))
			{
				errorMessage = "Could not evaluate (" + sToken + ") to a type";
				return 0;
			}
			
			// push it on the stack
			qTokens.push(token);

			// skip ending parenthesis
			index++;

		}
		// if we found a breaking character just skip it
		else if (s[index] == ' ' || s[index] == ',')
		{
			index++;

		}
		else // a character
		{
			int startIndex = index;
			
			// search for a breaking character or the end of the string
			while (!(s[index] == ' ' || s[index] == ',' || index == strLength))
			 index++; 
			
            // now there is a string waiting between startIndex and index
			// put it in a string
			string sToken = text.substr(startIndex,index - startIndex);

		    // figure out what data type it is
			if (giveInt(&token.typeInt,sToken))
			{
				token.type = eDataTypeInt;
			}
			else if (giveFloat(&token.typeFloat,sToken))
			{
				token.type = eDataTypeFloat;
			}
			else if (giveBool(&token.typeBool,sToken))
			{
				token.type = eDataTypeBool;
			}
			else // take it as a string
			{
				token.type = eDataTypeString;
				token.typeString = sToken;
				
			}
			
			// push it on the stack
			qTokens.push(token);		
		}


		// leave at end of string
		if (index >= strLength) done = true;
	}

	// delete previous token array
	if (m_tokens) 
		delete []m_tokens;

	// get number of m_tokens in queue
	m_numTokens = (int)qTokens.size();
	
	if (m_numTokens <=0)
	{
		m_tokens = NULL;
		m_typeList = "";
		m_numTokens = 0;
	}



	// create a new array to hold m_tokens from the queue
	m_tokens = new MultiVariable[m_numTokens];
	
	// reset the parameter array
	m_typeList = "";

	// loop through all the m_tokens
	for (int a = 0; a < m_numTokens; a++)
	{
		// save the token from the front of the queue
		m_tokens[a] = qTokens.front();

		// pop off the item we just saved
		qTokens.pop();
		
		m_typeList += m_tokens[a].type;


	}

	

	// everything worked return true
	return true;
}

/// returns a bool for a specified string. returns 0 on failure
bool TextParser::giveBool(bool* result, std::string toConvert)
 {
  
  // no pointer for return value?
  if (!result) 
	  return 0;

	const char* s = toConvert.c_str();
	
   if (stricmp( s, "off" ) == 0) { (*result) = 0; return 1; }    
   if (stricmp( s, "on" ) == 0) { (*result) = 1; return 1; }
   if (stricmp( s, "false" ) == 0) { (*result) = 0; return 1; }    
   if (stricmp( s, "true" ) == 0) { (*result) = 1; return 1; }

	 return 0;
 }



/// returns an int for a specified string. returns 0 on failure
bool TextParser::giveInt(int* result, std::string toConvert )
 {

  // no pointer for return value check
  if (!result) 
	  return 0;

  const char* s = toConvert.c_str();

  // make sure all the chars are numbers
  for (int i = 0; s[i] != NULL; i++)
  {
   if (i == 0 && s[0] == '-') continue; // the first char can be minus
   if ( s[i] > 57 || s[i] < 48) return 0 ;
  }  

  (*result) = atoi(s);

  return 1;
 }

///< returns a float for a specified string. returns 0 on failure
bool TextParser::giveFloat(float* result, std::string toConvert)
{


  // no pointer for return value?
  if (!result) 
	  return 0;

  const char* s = toConvert.c_str();
  
  int deccount = 0; // used to count decimals
  // make sure all the chars are numbers or decimal(only one )
  for (int i = 0; s[i] != NULL; i++)
  {
	// the negative is acceptable at the start
   if (i == 0 && s[0] == '-') continue; 

   // only one decimal
   if (s[i] == '.') {deccount++; continue;}

   // accept numbers
   if ((s[i] > 57 || s[i] < 48)) return 0;
  }

  // there must be at least one decimal to signify it is a float
  if (deccount != 1) return 0;

  // convert the string to a float
  (*result) = float(atof(s));

  // return success
  return true;
}


// Fills in the parameterList structure with the correct parameter information
/// You must call the parse function before calling feedParameterList
/// \param pList a pointer to the structure that the data info will be placed
void TextParser::feedParameterList(ParameterList* pList)
{
	// if the user didn't pass in an address to put information then leave
	if (!pList) 
		return;

	// clear the parameter list
	pList->Clear();

	// go through all the m_tokens
	for (int a = 0; a < m_numTokens; a++)
	{
		switch (m_tokens[a].type) // switch on type of token
		{
		case eDataTypeString:
			{
				pList->Strings[pList->numStrings] = m_tokens[a].typeString;
				pList->numStrings++;				
			}break;
		case eDataTypeFloat:
			{
				pList->Floats[pList->numFloats] = m_tokens[a].typeFloat;
				pList->numFloats++;				
			}break;
		case eDataTypeInt:
			{
				pList->Ints[pList->numInts]  = m_tokens[a].typeInt;
				pList->numInts++;				
			}break;
		case eDataTypeBool:
			{
				pList->Bools[pList->numBools]  = m_tokens[a].typeBool;
				pList->numBools++;				
			}break;
		case eDataTypeVector3:
			{
				pList->Vector3s[pList->numVector3s]  = m_tokens[a].typeVector3;
				pList->numVector3s++;				
			}break;
		}
	}

	// save the number of parameters
	pList->numParameters = m_numTokens;

	return;
}



// this is called when a parenthesis must be evaluated
bool TextParser::giveTokenFromTextParser(TextParser* tp, MultiVariable* token) // 
{
	if (token == NULL || tp == NULL) 
		return 0;

	// if there are 3 m_tokens
	if (tp->m_numTokens == 3)
	{

		// if there are 3 floats then it is a vector
		if (tp->getTypes() == "fff")
		{
			// there are 3 floats
			token->type = eDataTypeVector3;
			token->typeVector3.x = tp->m_tokens[0].typeFloat;
			token->typeVector3.y = tp->m_tokens[1].typeFloat;
			token->typeVector3.z = tp->m_tokens[2].typeFloat;
			return true;		
		}	
	}
	
	return false;
}





