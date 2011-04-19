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

#include "HelpText.h"

HelpText::HelpText(Renderer* pRenderer) :
  m_pRenderer (pRenderer),
  m_bVisible  (true)
{}

bool HelpText::IsVisible(void)
{
  return m_bVisible;
}

void HelpText::Draw(void)
{
  if(m_bVisible)
  {
    m_pRenderer->drawText("ESCAPE - Exit Program"           , 700, 200);
    m_pRenderer->drawText("F2 - Load Model"                 , 700, 215);
    m_pRenderer->drawText("F3 - Change Background Color"    , 700, 230);
    m_pRenderer->drawText("Left Mouse Button - Rotate Model", 700, 245);
    m_pRenderer->drawText("Right Mouse Button - Zoom In/Out", 700, 260);
    m_pRenderer->drawText("TAB - Hide/Show Help Text"       , 700, 275);
  }
  else
  {
    m_pRenderer->drawText("TAB - Hide/Show Help Text"       , 700, 10);
  }
}

void HelpText::Show(void)
{
  m_bVisible = true;
}

void HelpText::Hide(void)
{
  m_bVisible = false;
}

void HelpText::ToggleVisibility(void)
{
  m_bVisible = !m_bVisible;
}

void HelpText::ReverseColor(void)
{

}