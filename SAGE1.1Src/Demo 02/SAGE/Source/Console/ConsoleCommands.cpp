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

/// \file ConsoleCommands.cpp
#include "Console.h"
#include "common/Renderer.h"
#include "Game/gamebase.h"
#include "Input/Input.h"
#include "Terrain/Terrain.h"
#include "Water/Water.h"



bool consoleHelp (ParameterList* params,std::string* errorMessage)
{
	// tell console to run help command
	gConsole.helpCommand(params->Strings[0]);
		
	return 1;
}


bool consoleProblems (ParameterList* params,std::string* errorMessage)
{
	// tell the console to print our problems
	gConsole.problemsCommand(params->Bools[0]);		
	return 1;
}


bool consoleWireframe (ParameterList* params,std::string* errorMessage)
{
	// switches the wireframe mode to whatever is specified
	gRenderer.setWireframe(params->Bools[0]);
	
	return 1;
}

bool consoleFogEnable (ParameterList* params,std::string* errorMessage)
{
	// switches the wireframe mode to whatever is specified
	gRenderer.setFogEnable(params->Bools[0]);
	
	return 1;
}

bool consoleCameraFreeSpeed (ParameterList* params,std::string* errorMessage)
{
	
  if (!gGameBase)
    return 0;

  // sets the speed of the free camera
  gGameBase->setFreeSpeed(params->Floats[0]);

	return 1;
}

// turns information on and off
bool consoleInfoEnable (ParameterList* params,std::string* errorMessage)
{	
  if (!gGameBase)
    return 0;

	// sets camera to free camera
  gGameBase->enableRenderInfo(params->Bools[0]);

	return 1;
}

bool consoleAmbient (ParameterList* params, std::string* errorMessage)
{
  gRenderer.setAmbientLightColor(
    MAKE_RGB(params->Ints[0], params->Ints[1], params->Ints[2]));
  
  return 1;
}

bool consoleDirectionalColor (ParameterList* params, std::string* errorMessage)
{
  gRenderer.setDirectionalLightColor(
    MAKE_RGB(params->Ints[0], params->Ints[1], params->Ints[2]));
  
  return 1;
}

bool consoleLightDirection (ParameterList* params, std::string* errorMessage)
{
  Vector3 light = Vector3(params->Floats[0], params->Floats[1], params->Floats[2]);
  light.normalize();
  gRenderer.setDirectionalLightVector(light);
  
  return 1;
}

bool consoleTerrainDistort (ParameterList* params, std::string* errorMessage)
{
  Terrain::terrainTextureDistortion = params->Bools[0];
  
  return 1;
}

bool consoleTerrainLOD(ParameterList* params,std::string* errorMessage)
{  
  Terrain::LOD = params->Ints[0];
	return true;
}


bool consoleTerrainCracks(ParameterList* params,std::string* errorMessage)
{  
  // set the terrain texture
  Terrain::crackRepair = params->Bools[0];  
	return true;
}

/// Adds all the engine commands to the console.
/// this function is called once in Console::initiate()
void AddEngineConsoleCommands()
{
	gConsole.addFunction("help", "s",consoleHelp);
	gConsole.addFunction("problems", "b",consoleProblems);	
	gConsole.addFunction("wireframe", "b",consoleWireframe);
	gConsole.addFunction("fog", "b",consoleFogEnable);  
  gConsole.addFunction("cameraspeed", "f",consoleCameraFreeSpeed);  
  gConsole.addFunction("info", "b",consoleInfoEnable);
  gConsole.addFunction("ambient", "iii", consoleAmbient);
  gConsole.addFunction("dircolor", "iii", consoleDirectionalColor);  
  gConsole.addFunction("lightdirection", "fff", consoleLightDirection);  
  gConsole.addFunction("terraindistort", "b", consoleTerrainDistort);
  gConsole.addFunction("lod", "i", consoleTerrainLOD);
  gConsole.addFunction("crackrepair", "b", consoleTerrainCracks);


}




