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


#ifndef __SILOOBJECT_H_INCLUDED__
#define __SILOOBJECT_H_INCLUDED__

#include "Objects/GameObject.h"

/// \brief Represents a silo object
class SiloObject : public GameObject
{
public:
  friend class Ned3DObjectManager;
  
  SiloObject(Model *m);
  
  virtual void process(float dt);
  virtual void move(float dt);

protected:
/*	/// \brief Array of all smoke particle engines the plane uses
  /// The plane smoke trail is set to the particle system that is located at
  /// index m_hp.
  /// For instance, if m_hp is 1, m_allParticles[1] will be set
  std::vector<std::string> m_allParticles; 
  int m_smokeID; ///< Handle to the smoke particle system (-1 if there isn't one)

   /// \brief Sets the texture of the model and the smoke particle system based
  /// on the damage the plane has taken
  void setTextureAndSmoke(); */
};

#endif