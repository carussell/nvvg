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

/// \file Console.cpp
/// \brief Code for the Console class.

#include "Console.h"
#include "textparser.h"
#include "common/Renderer.h"
#include <string>
#include <iostream>
#include "input/input.h"
#include "windows.h"
#include "directorymanager/DirectoryManager.h"
#include <iostream>
#include "tinyxml/tinyxml.h"
#include "ConsoleCommentEntry.h"

using namespace std;

/// This is the singleton console.  Do not create one yourself!
/// This global is externed in Console.h.
Console gConsole;

/// prototype for function that adds all the engine commands to the console
void AddEngineConsoleCommands();


/// \brief hashtable pair typedef
typedef pair<string, int> hashIntPair;

/// \brief hashtable pair iterator typedef
typedef pair<HashTableInt::const_iterator, HashTableInt::const_iterator> hashIntIteratorPair;

// constructor initiate member variables
Console::Console()
{
	m_textureName = "Console.tga";
	m_textureHandle = -1;

	m_consoleActive = 0;
	m_consoleActivating = 0;
	m_consoleDeactivating = 0;

	m_consoleHeight = 0;
	m_consoleWidth = 0;
	
	m_finishCenterPoint = Vector3(0.0f,0.0f,0.0f);
	m_startCenterPoint = Vector3(0.0f,0.0f,0.0f);

	m_consoleCurrentLocation = Vector3(0.0f,0.0f,0.0f);
		
	m_nextInsertIndex = 0;

	m_textPadding.left = 10;
	m_textPadding.right = 10;
	m_textPadding.top = 10;
	m_textPadding.bottom = 5;

	m_textWidth = 16;
	m_textHeight = 24;

	m_timeForTransition = 0.35f;
	
	
	
    	
}

// Basic Destructor.  Invokes the shutdown() method.
Console::~Console()
{
	shutdown();
	
	



}


void Console::initiate()
{
	
	
	gDirectoryManager.setDirectory(eDirectoryEngine);
	
	// cache the texture to use for the background	
	m_textureHandle = gRenderer.cacheTexture(m_textureName.c_str(),false);
		
	m_consoleHeight = gRenderer.getScreenY()/2;
	m_consoleWidth = gRenderer.getScreenX();

	// set render target dimensions (must be power of 2
	m_renderTargetWidth = m_renderTargetHeight = 1;	
	while (m_renderTargetWidth < (int)m_consoleWidth)
		m_renderTargetWidth = m_renderTargetWidth << 1;

	while (m_renderTargetHeight < (int)m_consoleHeight)
		m_renderTargetHeight = m_renderTargetHeight << 1;

	// create a render target
	m_renderTargetHandle = gRenderer.allocTexture(
		"consoleTarget",
		m_renderTargetWidth,
		m_renderTargetHeight, 
		true);
	
	m_finishCenterPoint = Vector3(
		gRenderer.getScreenX()/ 2.0f,
		gRenderer.getScreenY()/ 4.0f, 
		1.0f);

	m_startCenterPoint = Vector3(
		gRenderer.getScreenX()/ 2.0f,
		- gRenderer.getScreenY()/ 4.0f, 
		1.0f);

		
	// set the console to the starting position
	m_consoleCurrentLocation = m_startCenterPoint;
    	
	// initiate the timer
	m_timeLastToggled = GetTickCount();

	m_fontHandle = gRenderer.addFont("Arial", m_textWidth, m_textHeight, true);

	// calculate number of rows in the conosle
	calculateRows();

	// gives the array of strings that represents all the lines
	m_textBufferSize = m_textRows;
	
	// create array for the text buffer
	m_textBuffer = new string[m_textBufferSize];


	
	gDirectoryManager.setDirectory(eDirectoryEngine);
	loadCommentsFromXml("consoleDoc.xml",false);
	
	
	AddEngineConsoleCommands();

	

	return;
}


// renders the console to the screen
void Console::render()
{
	// if the console shouldn't be rendered then leave
	if (!(m_consoleActive || m_consoleActivating || m_consoleDeactivating))
		return;

	bool zBufferState; // use to save the state of zbuffering
	bool wfState; // saves the wireframe state
	
	// recalculate the texture coordinates because the render target might now
	// be the same size as the screen coord
	float texU = m_consoleWidth / (float) m_renderTargetWidth;
	float texV = m_consoleHeight / (float) m_renderTargetHeight;
	float sx = (m_consoleWidth / 2.0f);
	float sy = (m_consoleHeight / 2.0f);
	
	// 4 vertices used to draw the console to the screen
	RenderVertexTL VertexList[4];
	
	// save states
	zBufferState = gRenderer.getZBufferEnable();
	wfState = gRenderer.getWireframe();
	
	// turn off z buffer
	gRenderer.setZBufferEnable(false);
	gRenderer.setWireframe(false);

	// renders the text and background image to a texture
	renderToTexture();

	// get 4 vertices ready to be drawn to the screen
	
    // upper left corner			
	VertexList[0].p = Vector3(-sx,-sy,1.0f) + m_consoleCurrentLocation;
	VertexList[0].u = 0.0f;
	VertexList[0].v = 0.0f;
	VertexList[0].argb = 0XFFFFFFFF;

	//  upper right corner
	VertexList[1].p = Vector3(sx,-sy,1.0f) + m_consoleCurrentLocation;
	VertexList[1].u = texU;
	VertexList[1].v = 0.0f;
	VertexList[1].argb = 0XFFFFFFFF;

	// bottom right corner
	VertexList[2].p = Vector3(sx,sy,1.0f) + m_consoleCurrentLocation;
	VertexList[2].u = texU;
	VertexList[2].v = texV;
	VertexList[2].argb = 0XFFFFFFFF;

	// bottom left corner
	VertexList[3].p = Vector3(-sx,sy,1.0f) + m_consoleCurrentLocation;
	VertexList[3].u = 0.0f;
	VertexList[3].v = texV;
	VertexList[3].argb = 0XFFFFFFFF;	
	
	// set the texture
	gRenderer.selectTexture(m_renderTargetHandle);

		
	// render the console specifying the width and height
	gRenderer.renderSprite(VertexList);		
	

	// set the zbuffer state back to what it was
	gRenderer.setZBufferEnable(zBufferState);

		// set the zbuffer state back to what it was
	gRenderer.setWireframe(wfState);

	

	return;
}

// renders the text to the texture 
void Console::renderText()
{
	// rectangle to draw the text in
	IRectangle rect;
	rect.left = (int)m_textPadding.left;
	rect.top = (int)m_textPadding.top;
	rect.right = (int)(m_consoleWidth - m_textPadding.right);
	rect.bottom = (int)(m_consoleHeight - m_textPadding.bottom);
	
	int width = rect.right - rect.left;
	int height = 0;
	int maxHeight = rect.bottom - rect.top;
	bool done = false;

	// get the text that needs to be drawn
	string s = createCurrentBuffer();

	// trim the string down so it will fit on the console.  This must
	// be done because some strings are more than one line on the screen
	while (!done)
	{		
		// calculate height needed to draw text
		height = gRenderer.calculateTextHeight(s.c_str(), width,m_fontHandle);

		// there is nothing to draw
		if (height == 0)
			return;

		// doesn't fit
		if (height > maxHeight)
		{
			// shave off a line
			s = s.substr(s.find_first_of('\n') + 1, s.length() -1);
		}
		else // it does fit
		{
			done = true;
		}
	}
	
	// draw the text to the current render target
	gRenderer.drawText(s.c_str(),&rect, eTextAlignModeBottom,true ,m_fontHandle);

	return;
}

// renders the background image to the texture
void Console::renderBackgroundTexture()
{

	// calculate the center point for th sprite
	Vector3 center = Vector3(m_consoleWidth/2.0f, m_consoleHeight/2.0f,0.0f);	
	
	// push translation
	gRenderer.instance(center,EulerAngles(0.0f,0.0f,0.0f));
		
	// set the texture to the background texture
	gRenderer.selectTexture(m_textureHandle);
	
	gRenderer.setARGB(0XFFFFFFFF);
	
	// render the background texture to our temporary texture
	gRenderer.renderSprite(
		(float)m_consoleWidth,
		(float)m_consoleHeight);

	// pop translation
	gRenderer.instancePop();

}


// renders the background image to the texture and then the text
void Console::renderToTexture()
{
	// save the old render target
	int oldTarget = gRenderer.getRenderTarget();
	
	// set the render target to the texture created in initate
	gRenderer.setRenderTarget(m_renderTargetHandle);

	// render the background image
	renderBackgroundTexture();

	// render the text
	renderText();

  // set the render target back to what it was
  gRenderer.setRenderTarget(oldTarget);
	
  

	return;
}


// called every time through the game loop
void Console::process()
{
	// allow the console to process movement
	moveConsole();

	// process input from the input wrapper
	processInput();

	return;
}

// deallocates memory
void Console::shutdown()
{
	// deallocate the memory for all the text
	if (m_textBuffer)
	{
		delete [] m_textBuffer;
		m_textBuffer = NULL;
	}

	// delete all the commands in the vector
	int size = (int)m_vectorCommands.size();	
	for (int a= 0; a < size; a++)		
	{
		ConsoleFunctionEntry* p = m_vectorCommands[a];
		if (p != NULL) delete p;			
		m_vectorCommands[a] = NULL;
	}
	
	return;
}

/// The command is processed as if it were typed on the console
/// \param line processes a line as if the user typed it in the console.
/// \param printToConsole True indicates that the line parameter should
/// be printed to the console.
void Console::processLine(std::string line, bool printToConsole)
{


	// Variables
	ParameterList paramList; // used to store parameters passed to functions
	TextParser tp; // used to parse the texted passed in
	bool result = 0; // stores the result of text parsing
	string command; // holds the command aka the first word in line
	ConsoleFunctionEntry* entry = NULL; // pointer used to access items in m_vectorCommands
	
	string errorMessage; // used to hold an error message from a user defined function

	// check if the command should be printed to the console
	if (printToConsole) 
		printLine(line);

	

	// peel off the command
	int i = 0;
	string s = line;
	i = (int)s.find_first_of(' ');
	if (i == -1)
	{
		command = s;
		s = "";

	}
	else
	{
		command = s.substr(0, i);
		s = s.substr(i+1, s.length());
	}
	

	result = tp.parse(s);


	// if the text parsing failed print the error to the console and return
	if (result == false)
	{
		// print what the error was to the console and then leave
		printLine(tp.errorMessage);
		return;
	}
		 	

	// get a pointer to the entry if it exists
	entry = getMatchingEntry(command);
		
	// if it exists
	if (entry != NULL) 
	{
		if (entry->getParameters() == tp.getTypes())
		{
		// feed in parameters into structure
		tp.feedParameterList(&paramList);

		// call the function
		result = entry->callFunction(&paramList, &errorMessage);

		// if custom function returns false then print the errorMessage
		if (result == false)
			printLine(errorMessage);
		}
		else
		{
			// parameters mismatch, print out the prototype
			printLine("parameter mismatch");
			printLine("Prototype: " + command + " " + 
				getParameterListFromString(entry->getParameters()));
		}
	}
	else	// command was not found print an error
	{		
		printLine("Command not found");
	}	


	return;
}


// prints a line to the console with wrapping
/// \param line Text to print to the console
void Console::printLine(std::string line)
{
	// make sure next inserting point works
	m_nextInsertIndex = m_nextInsertIndex % m_textBufferSize;
	
	// something is wrong reset insertion index
	if (m_nextInsertIndex < 0) 
		m_nextInsertIndex = 0;	
	
	// set the line	
	m_textBuffer[m_nextInsertIndex] = line;	
	
	// increment the line
	m_nextInsertIndex++;
	
	// mod the indicies so they wrap around
	m_nextInsertIndex = m_nextInsertIndex % m_textBufferSize;

}

// returns true if the console is down
bool Console::isActive()
{
 return m_consoleActive;
}

// adds a function to the console to be called by the user
/// \param functionName Command name
/// \param parameters Parameters the custom command takes 
/// (i.e. "sib" means String, Integer, Boolean)
/// \param functionPtr pointer to a function to be called to 
/// process the command.\n
/// The function passed in must be of prototype: \n
/// bool function( ParameterList *,string *)\n
/// \return true on success and false on failure
/// \remark See Console class explanation for an example
bool Console::addFunction(std::string functionName, std::string parameters, consoleFunctionPointer functionPtr)
{
	ConsoleFunctionEntry* cfe = NULL; // pointer to a console function entry

	// find an entry that matches
	cfe = getMatchingEntry(functionName);


	// if a matching entry wasn't found then make a new entry
	if (cfe == NULL)
		cfe = createEntry(functionName);

	// if the entry already has a function defined print an error
	if (cfe->functionDefined())
	{		
		string error;
		error = "Command: '";
		error += functionName;
		error += "' has already been defined for the specified parameters";

		printLine(error);
		return false;
	}				
	
	// add the function to the entry
	cfe->DefineFunction(functionName, parameters, functionPtr);
		
	// return success
	return true;
}

// this must be called when a character is pressed
/// \param input Character pressed	
void Console::pressedChar(char input)
{
	if (input == '`' || input == '~')
	{
		toggleConsole();
		return;
	}

	// if the console is not active then don't take in input
	if (!m_consoleActive) return;

	if (input == 8 ) // backspace pressed
	{
		// if there isn't anything in the user input return
		if (m_inputLine.length() == 0) 
			return;

		// delete the last character
		m_inputLine.erase(m_inputLine.length()-1,1);		
		return;
	}
	if (input == 13) // enter pressed
	{
		// process line
		processLine(m_inputLine.c_str(), true);
		
		// clear user input
		m_inputLine = "";
			return;
	}

	// add the character
	addChar(input);
	

}

// provides animation
void Console::moveConsole()
{
	
	// calculate time passed since last toggle
	float timePassed = ((float)(GetTickCount() - m_timeLastToggled)) / 1000.0f;
	
	// holds the ratio of time completed.  (0 means starting 1 means done
	float ratio;

	// if the console is being activated
	if (m_consoleActivating)
	{
		// if the console should be fully down
		if (timePassed >= m_timeForTransition)
		{
			// the console is no longer activating
			m_consoleActivating = false;			

			// set the postion for that the console will stay at		
			m_consoleCurrentLocation = m_finishCenterPoint;		
		}
		else // the console is still animating
		{
			ratio = timePassed / m_timeForTransition;
		
			// interpolate between start and end
			m_consoleCurrentLocation = 
				(1.0f - ratio) * m_startCenterPoint +
				ratio * m_finishCenterPoint;						
		}
	}


		// if the console is being deactivated
	if (m_consoleDeactivating)
	{
		// if the console should be fully up
		if (timePassed >= m_timeForTransition)
		{
			// the console is done deactivating
			m_consoleDeactivating = false;

			// set the location of the console to the starting point
			m_consoleCurrentLocation = m_startCenterPoint;
		}
		else // the console is still animating
		{
			ratio = timePassed / m_timeForTransition;
			
			// interpolate between start and end
			m_consoleCurrentLocation = 
				(1.0f - ratio) * m_finishCenterPoint +
				ratio * m_startCenterPoint;
		}
	}
	
	return;
}

// process input from the keyboard
void Console::processInput()
{
	// once input is working we will use direct input

		

	return;
}



/// Console calculateRows
/// calculates the number of rows base on console dimensions
/// stores this number in the m_textRows member variable
void Console::calculateRows()
{
	// calculate total height for text
	int height = m_consoleHeight;

	// subtract off the text padding
	height -= m_textPadding.top;
	height -= m_textPadding.bottom;

	// calculate height for each row
	int rowheight = m_textHeight;// + rowSpacing;

	m_textRows = height / rowheight;

	// take one row out for the input line
	m_textRows--;

	return;
}



// returns a string that holds the buffer that will be printed to the screen
string Console::createCurrentBuffer()
{
	string buff; // buffer that holds all text on the screen
	int index; // hold the index of a string from the m_textBuffer to add to buff

	index = (m_nextInsertIndex - m_textRows) % m_textBufferSize;
	if (index < 0) index += m_textBufferSize;

	for (int a= 0; a < m_textRows; a++)
	{
		// add a line to the console from the text buffer
		buff += m_textBuffer[index] + '\n';

		// increase the index of the textbuffer
		index++;

		// loop around the circular buffer
		index = index % m_textBufferSize;
	}

	// add the user's typing to the buffer
	buff += m_inputLine;

	// add a blinking cursor
	if ((GetTickCount() / 100) %2 == 1)
		buff += '_';
	else buff += ' ';
	


	// return all the text that needs to be drawn to the screen
	return buff;
}

// if the console is down it goes up if it is up it goes down
void Console::toggleConsole()
{

	// if the console is not active activate it
	if (!m_consoleActive)
	{
		m_consoleActive = true;
		m_consoleActivating = true;
		m_consoleDeactivating = false;
    // disable keyboard input for the game
    gInput.keyBoardEnable(false);
	}			
	else // if the console is active deactivate it
	{
		m_consoleActive = false;
		m_consoleDeactivating = true;
		m_consoleActivating = false;
    // enable keyboard input for the game
    gInput.keyBoardEnable(true);
	}

	// record the time that the key was pressed so we can animate the
	// console dropping down
	m_timeLastToggled = GetTickCount();

	return;
}


// safely adds a character to the input string
void Console::addChar(char input)
{
	// make sure it is a character
	if (input < 32 || input > 126) 
		return;

	// add the character to the m_inputLine
	m_inputLine += input;

	return;
}




// loads console comments from an XML file
/// This method can be called multiple times with seperate files.
/// It important to comment your commands so that you won't forget
/// what they do.  To check what commands are uncommented type
/// 'problems' into the console.
/// \param fileName XML filename
/// \param defaultDirectory Whether or not to use the default XML directory
void Console::loadCommentsFromXml(std::string fileName, bool defaultDirectory)
{
	// Variables
	TiXmlDocument file; // the XML document object
	TiXmlElement *commands = NULL; // used to parse XML
	TiXmlElement *command = NULL; // used to parse XML	
	TiXmlElement *parameter = NULL; // used to parse XML
	ConsoleFunctionEntry* entry; // makes it easier to edit the command information
	ConsoleCommentEntry ccce; // temporary holds XML info before it is transfered to entry

	// set the directory to the XML directory
	if (defaultDirectory)
		gDirectoryManager.setDirectory(eDirectoryXML);

	
	

	// load the xml specified and leave on failure and print error
	if (!file.LoadFile(fileName.c_str()))
	{
		string output = "Failed to command load comments from XML: '";
		output += fileName + "'";		
        printLine(output);
		return;
	}
	

	// get the list of commands
	commands = file.FirstChildElement("commands");
	
	// loop through every command in the commands list
	command = commands->FirstChildElement();	
	while(command != 0)
	{			

		// load the XML info into the temporary object ccce
		ccce.loadComments(command);

		// get a pointer to the function if it is there
		entry = getMatchingEntry(ccce.functionName);			
		
		// if the entry isn't there then add it
		if (!entry) 
			entry =	createEntry(ccce.functionName);

		// if it still isn't there then skip it
		if (!entry) 
			continue;

		// if comments haven't already been defined
		if (!entry->commentDefined())
		{
			// feed information loaded from the XML into the entry object we just made
			entry->DefineComments(&ccce);
		}
		else // if they have print an error
		{
			printLine("Redefinition of comments for command: " + ccce.functionName);

		}

		// go to the next entry in the XML
		command = command->NextSiblingElement();
	}



	return;
}


// brings your attention to comments without functions attached
// and functinos without comments attached
/// ConsoleFunctionEntry::See printProblems
/// \param criticalOnly true specifies to only print critical errors

void Console::problemsCommand(bool criticalOnly)
{
	// loop through every ConsoleFunctionEntry
	for (int a =0 ;a < (int)m_vectorCommands.size(); a++)
	{
		// if the entry is not null
		if (m_vectorCommands[a])
			// print its problems to the console
			m_vectorCommands[a]->printProblems(criticalOnly);
	}
	return;
}

// this is called when a user types help on the command line
/*!
		if command is "-a" all commands will be printed to the console
		with spaces inbetween each one
		
		if command is not "-a", information on the command
		will be printed to the console.
*/
/// \param command to view comments of
void Console::helpCommand(std::string command)
{		
	bool commandFound = false;
	ConsoleFunctionEntry * entry = NULL;
	
	
	if (command == "-a")
	{
		string output = "Commands: ";			
		// get size of vector
		int size = (int)m_vectorCommands.size();

		// loop through vector
		for (int a = 0; a < size; a++)
		{
								
			// if the entry isn't NULL
			if (m_vectorCommands[a])
			{				
					output += m_vectorCommands[a]->getName();
					
					// put a comma inbetween execpt on the last one
					if (a != size - 1)
						output += ", ";				
			} // end if
		} // end for

		// print all the commands
		printLine(output);		

	}
	else // user didn't type "-a"
	{
    // print a space 
			printLine("");

		// search hashtable for the command
		hashIntIteratorPair p = m_hashTable.equal_range(command);
		// go through every entry with that name
		for (HashTableInt::const_iterator i = p.first; i != p.second; ++i)
		{	  
			// mark that the command was found
			commandFound = true;

			// get a pointer to the command entry
			entry = m_vectorCommands[i->second];	

			// tell the entry to print its detailed information
			entry->printDetailedInformation();						
				
		}
		
		// if the command wasn't found 
		if (!commandFound)printLine("Command '" + command + "' Not Found!");
	}
	
	


 return;
}

ConsoleFunctionEntry* Console::createEntry(std::string commandName)
{
	
	// create a new ConsoleFunctionEntry
	ConsoleFunctionEntry *cfe =  new ConsoleFunctionEntry();
	
	// get the index where cfe is about to be inserted
	int index = (int)m_vectorCommands.size();

	// push cfe onto the end
	m_vectorCommands.push_back(cfe);

	// create a pair object with the key (FunctionName) and the index into
	// the vector (index)
	hashIntPair htp(commandName,index);

	// put it in the hashtable
	m_hashTable.insert(htp);

	// return a pointer to the entry
	return cfe;
}

 ///< returns a pointer to an entry if it exists
ConsoleFunctionEntry* Console::getMatchingEntry(std::string commandName)
{

			
	// find the key
	HashTableInt::const_iterator p = m_hashTable.find(commandName);	
	
	// if the key is not there leave
    if ( p == m_hashTable.end( ) ) 
		return NULL;
	  
	// return a pointer to the ConsoleFunctionEntry
	return m_vectorCommands[p->second];
		

	}
// global functsion
char getCharFromType(std::string type)
{
	// convert a string to a letter
	if (_stricmp(type.c_str(), "string") == 0) return 's';
	if (_stricmp(type.c_str(), "float") == 0) return 'f';
	if (_stricmp(type.c_str(), "bool") == 0) return 'b';
	if (_stricmp(type.c_str(), "int") == 0) return 'i';
	if (_stricmp(type.c_str(), "vector3") == 0) return 'v';
	return ' ';
}

// global functsion
string getTypeFromChar(char c)
{
	// convert a letter to a string
	switch (c)
	{
		case 's': return "string";
		case 'f': return "float";
		case 'b': return "bool";
		case 'i': return "int";
		case 'v': return "vector3";
	}

	return "Error, parameter corrupt check XML";
}

// global functions
string getParameterListFromString(std::string s)
{
	
	string output;
	int numParameters = (int)s.length();
	
	// go through every character in the string sent in
	for (int i = 0 ; i < numParameters; i++)
	{		
		// convert each type represented as a character 
		// to a string and add it to output
		output += getTypeFromChar(s[i]);
		// put a comma after each type except the last one
		if ( numParameters - 1 != i) output += ", ";		
	}	
			
	// return the types as a string
	return output;
}



