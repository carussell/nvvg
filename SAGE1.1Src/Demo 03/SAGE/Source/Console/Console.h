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

/// \file Console.h
/// Contains the decleration of class Console.
/// externs a global instance of the Console object called gConsole

#ifndef __CONSOLE_H_INCLUDED__
#define __CONSOLE_H_INCLUDED__

#include "common/vector3.h"
#include "common/eulerangles.h"
#include "common/rectangle.h"
#include "ConsoleFunctionEntry.h"
#include <string>
#include <iostream>
#include <hash_map>


#include "ParameterList.h"



// this template is kind of messy to declare so a typedef is
// in order
typedef stdext::hash_map<const std::string,int> HashTableInt;

//-------------------------------------------------------------------------
/// \class Console
/// \brief Provides a console allowing the user to type commands at 
/// run time.
//
/*! 

A global Console object is created in console.cpp and is externed in 
console.h (this file). \n


To create a custom function:

1. Create a global function with the following prototype:

bool myFunction (ParameterList* ,string*);

2. Call the addFunction method to add the function.  
Your custom function will now be called when it is requested by the user

\n \n


When the custom function is called, the ParameterList* contains information
about the parameters.  If the custom function returns false, the string 
passed in will be printed to the console.  You are free to change this 
string to create your own custom error messages.

Example:

The following example creates a command, mytest, which takes 1 integer as
a parameter and is attached to function myFunction.  When a user types 
mytest followed by an integer myFunction is called  If the integer value
is greater than 10, an error message is printed to the console.  If it
is less than 10, a process is done.

 \code
bool myFunction (ParameterList* params,string* errorMessage)
{
	if (params[0]->Ints[0] > 10)
	{
		// dereference the point to the string and set it to a message
		(*errorMessage) = "Number is greater than 10 error!";
		
		// return error so that the errorMessage is printed to the console
		return false; 
	}
	
	// Do Something

	return true;
}

int AddMyFunction()
{
	
	// the "i" means that the command takes one integer as a parameter
	gConsole.addFunction("mytest","i",myFunction);

}

\endcode
*/
/// \warning Do not create multiple instances of this object!

class Console
{
public:
	
	Console(); ///< Basic Constructor.  Initiates varaibles to default values.
	~Console(); ///< Basic Destructor.  Invokes the shutdown() method.

	//-------------------------------------------------------------------------
	/// \name Maintenance	
	//@{
	
	/// \brief Initializes the engine. Must be called once at program startup
	void initiate(); 
	
	void process(); ///< Moves console and processes input
	void render(); ///< Renders the console to the screen
	void shutdown(); ///< Release resources

	//@}
	//-------------------------------------------------------------------------


	//-------------------------------------------------------------------------
	/// \name Interface
	/// \brief Methods that the user can call to interface with the console
	//@{

	/// \brief Process the string passed as a command	
	void processLine(std::string line, bool printToConsole = false); 
	
	
	/// \brief Inserts a line into the console	
	void printLine(std::string line); 
	
	bool isActive(); ///< True if the console is down and taking input

	/// \brief Adds custom command.				
	bool addFunction(std::string functionName, std::string parameters, consoleFunctionPointer functionPtr);
	
	///\brief Loads command comments from an xml file.			
	void loadCommentsFromXml(std::string fileName, bool defaultDirectory = true); 

	//@}
	//-------------------------------------------------------------------------
	
	
	/// \brief Tells the console that a key has been pressed	
	void pressedChar(char input); 

	//-------------------------------------------------------------------------
	/// \name Console Commands
	/// \brief These functions are called through console commands
	/// via global functions
	//@{	
	
	/// \brief Prints command problems to the console
	void problemsCommand(bool criticalOnly);

	/// \brief Allows user to see comments for commands		
	void helpCommand(std::string command);
	//@}
	
private:

	//-------------------------------------------------------------------------
	/// \name Graphics Members
	/// \brief These members are used to render the console	
	//@{

	void moveConsole(); ///< Animates the console if necessary
	void renderToTexture(); ///< Renders the text and background to texture
	
	/// \brief Renders the background texture to the renderTarget texture
	void renderBackgroundTexture();

	void renderText(); ///< Renders the text to the renderTarget texture
	
	/// \brief Calculates the number of rows base on console dimensions
	void calculateRows();
		
	std::string m_textureName; ///< Image file name for background
	int m_textureHandle; ///< Stores the texture handle
	
	/// \brief Stores the handle to the render target created in initate().
	int m_renderTargetHandle; 

	/// \brief Stores the width of the texture that m_renderTargetHandle 
	/// has a handle to.  This has to be a power of 2.
	int m_renderTargetWidth; 

	/// \brief Stores the height of the texture that m_renderTargetHandle
	/// has a handle to.  This has to be a power of 2.
	int m_renderTargetHeight; 

	int m_consoleHeight; ///< The height in pixels of the console
	int m_consoleWidth; ///< The width in pixels of the console

	/// \brief The ammount of padding around the borders of the console
	/// before the text appears.
	IRectangle m_textPadding;

	int m_textWidth; ///< Width of a single character
	int m_textHeight; ///< Height of a single character

	int m_fontHandle; ///< Handle to the font to will use
	int m_textRows; ///< Number of rows in the console	

	Vector3 m_finishCenterPoint; ///< Center point of the console when it is extend
	
	/// \brief Center point where the console comes from/ goes to deactivate
	Vector3 m_startCenterPoint; 

	/// \brief Current location of the center point of the console
	Vector3 m_consoleCurrentLocation; 
		
	/// \brief The TimeStep taken the last time the console was toggled	
	/// (used for animating the console dropping down)
	unsigned long m_timeLastToggled;
	float m_timeForTransition; ///< Time it takes the console to drop down
	
	//@}
	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	/// \name Text Editing Members
	/// \brief These members hold and manipulate text
	/// 
	//@{
		
	/// \brief Creates a text buffer to be drawn on the screen
	std::string createCurrentBuffer(); 
	
	void addChar(char input); ///< Safely adds a character to inputLine

	std::string m_inputLine; ///< Text on the input line
	int m_textBufferSize; ///< Length of the text buffer array

	std::string* m_textBuffer; ///< Circular array of recently type lines.
	
	/// \brief Index into textBuffer where the next input will be put;
	int m_nextInsertIndex; 

	//@}
	//-------------------------------------------------------------------------


	//-------------------------------------------------------------------------
	/// \name Custom Command Calling Members
	/// \brief Used to store and edit custom commands
	/// 
	//@{

	/// \brief Creates an entry, adds it to the table and returns 
	/// a pointer to it
	ConsoleFunctionEntry* createEntry(std::string commandName);

	/// \brief Returns a pointer to an entry if it exists
	ConsoleFunctionEntry* getMatchingEntry(std::string commandName); 
	
	/// \brief Indexes to function data is stored in this hashtable
	HashTableInt m_hashTable; 
	
	/// \brief All the functions and comments are stored in this vector
	std::vector<ConsoleFunctionEntry*> m_vectorCommands; 

	//@}
	//-------------------------------------------------------------------------

	void processInput(); ///< Process input since last call

	/// \brief Set if the console is down, coming down or going up
	bool m_consoleActive; 
	bool m_consoleActivating; ///< Set if the console coming down
	bool m_consoleDeactivating; ///< Set if the console goind up

	/// \brief Makes the conosle go up if it is down and down if it is up
	void toggleConsole(); 


};


extern Console gConsole;


#endif