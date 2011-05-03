/*
 * BuzzedSiloObject.  Adapted from SiloObject.  Original license below.
 *
 *--o0o=================================================================o0o----
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
 *--o0o=================================================================o0o--*/

#include "BuzzedSiloObject.h"

BuzzedSiloObject::BuzzedSiloObject(Model *m)
  : SiloObject(m)
{
  this->m_className = "BuzzedSilo";
  this->m_type = ObjectTypes::BUZZEDSILO;
  this->m_isBuzzedSiloDead = false;

  this->m_allTextures.resize(2);
  this->m_allTextures[0] = "buzzedcylo.tga"; // when dead
  this->m_allTextures[1] = "bcylo2.tga";
  for (int a =0; a < (int)m_allTextures.size(); a++)
    gRenderer.cacheTextureDX(m_allTextures[a].c_str());
  //setting texture back to original buzzed silo
  setTexture(1);

}

void BuzzedSiloObject::kill(void)
{
	this->m_isBuzzedSiloDead = true;
	//changing the texture to show user has buzzed it
	tag();
}

void BuzzedSiloObject::tag(void)
{
	//sets texture to "buzzed"
	setTexture(0);
}

void BuzzedSiloObject::setTexture(int textNdx)
{
  int textureIndex = textNdx;// index into m_allTextures array
      // set texture 
  // make sure the indicies are in range
  if (textureIndex >= (int)m_allTextures.size())
    textureIndex = (int)m_allTextures.size() - 1;
  if (textureIndex < 0) textureIndex = 0;
  //actually sets the textures to the new texture
  int numParts = m_pModel->getPartCount();
  for (int a = 0; a < numParts; a++)
    m_pModel->setPartTextureName(a,m_allTextures[textureIndex].c_str());
   m_pModel->cache();

   
}
