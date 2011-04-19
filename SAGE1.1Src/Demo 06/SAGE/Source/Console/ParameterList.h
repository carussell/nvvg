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

/// \file ParameterList.h
/// \brief Interface for the ParameterList class.

#ifndef __H_PARAMETERLIST_INCLUDED__
#define __H_PARAMETERLIST_INCLUDED__

#include "common/Vector3.h"

/// \brief Maximum amount of parameter allowed in a single command
#define MAX_PARAMETERS 20

/// \struct ParameterList
/// \brief Holds parameters in an easy to use format
//
/*! 
When the user types a line in the console, the line
is broken up into a command and parameters.  The parameters
are stored in this structure.  Once the structure is filled
in, the custom function is called passing a pointer to this
structure.  The custom function can then access each 
parameter easily.
*/
struct ParameterList
{
	std::string Strings[MAX_PARAMETERS]; ///< holds string parameters
	int Ints[MAX_PARAMETERS]; ///< holds integer parameters
	float Floats[MAX_PARAMETERS]; ///< holds float parameters
	bool Bools[MAX_PARAMETERS]; ///< holds boolean parameters
	Vector3 Vector3s[MAX_PARAMETERS]; ///< holds Vector3 parameters

	int numStrings; ///< number of string parameters available
	int numInts; ///< number of integer parameters available
	int numFloats; ///< number of float parameters available
	int numBools; ///< number of boolean parameters available
	int numVector3s; ///< number of vector3 parameters available
	
	int numParameters; ///< number of parameters available


	void Clear()
	{
		numStrings = 
		numInts =
		numFloats =
		numBools =
		numVector3s =
		numParameters = 0;

	}
};


#endif