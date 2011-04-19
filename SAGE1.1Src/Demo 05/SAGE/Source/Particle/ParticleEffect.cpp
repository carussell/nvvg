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

#include "ParticleEffect.h"
#include "ParticleEngine.h"
#include "ParticleDefines.h"
#include "Particle.h"
#include "common/Quaternion.h"
#include "common/RotationMatrix.h"
#include "directorymanager/directorymanager.h"
#include "common/RotationMatrix.h"
#include "common/Renderer.h"
#include "common/commonstuff.h"

extern LPDIRECT3DDEVICE9 pD3DDevice; ///< Global DirectX device

//-----------------------------------------------------------------------------
/// \brief Maps strings to property functions
///
/// This class helps the ParticleEffect class by mapping strings (as keys) to
/// functions that set properties. This class is a singleton; only one
/// instance of this class is ever created. Call getInstance to get a pointer
/// to an instance of this class.
/// \warning Do not call delete on the return pointer value of getInstance
class ParticlePropertyMapper
{
private:
  ParticleEffect::PropertyMap m_propertyMap; ///< Hash map of string/function pointers

  /// \brief Basic constructor
  ///
  /// Initializes the hash map with all the supported properties
  ParticlePropertyMapper()
  {
    // add supported effect properties
    m_propertyMap.insert(ParticleEffect::PropPair("emit", &ParticleEffect::setEmit));
    m_propertyMap.insert(ParticleEffect::PropPair("sort", &ParticleEffect::setSort));
    m_propertyMap.insert(ParticleEffect::PropPair("gravity", &ParticleEffect::setGravity));
    m_propertyMap.insert(ParticleEffect::PropPair("cycle", &ParticleEffect::setCycle));

    // add supported initial particle property values
    m_propertyMap.insert(ParticleEffect::PropPair("particlelife", &ParticleEffect::setParticleLife));
    m_propertyMap.insert(ParticleEffect::PropPair("particlespeed", &ParticleEffect::setParticleSpeed));
    m_propertyMap.insert(ParticleEffect::PropPair("particlecolor", &ParticleEffect::setParticleColor));
    m_propertyMap.insert(ParticleEffect::PropPair("particlesize", &ParticleEffect::setParticleSize));
    m_propertyMap.insert(ParticleEffect::PropPair("particledrag", &ParticleEffect::setParticleDrag));
    m_propertyMap.insert(ParticleEffect::PropPair("particlefade", &ParticleEffect::setParticleFade));
    m_propertyMap.insert(ParticleEffect::PropPair("particlerotation", &ParticleEffect::setParticleRotation));
  }

public:
  /// \brief Gets a pointer to the only instance of this class
  /// \return Pointer to the only instance of this class
  static ParticlePropertyMapper* getInstance()
  {
    static ParticlePropertyMapper* mapper = new ParticlePropertyMapper();

    return mapper;
  }

  /// \brief Get the function pointer that maps to a string value
  /// \param property String representation to map
  /// \return Function pointer mapped to property
  ParticleEffect::PropertyFunc getFunction(std::string property)
  {
    ParticleEffect::PropertyMap::const_iterator iter;

    iter = m_propertyMap.find(property);

    if(iter == m_propertyMap.end()) // we don't have that key in our map
      return NULL;
    else
      return iter->second;
  }
};
//-----------------------------------------------------------------------------


/// \param effectDef XML tag containing the effect definition
ParticleEffect::ParticleEffect(TiXmlElement *effectDef)
{
  // assign defaults
  m_nTotalParticleCount = 0;
  m_bCycleParticles = true;
  m_bIsDead = false;
  m_IsDying = false;
  m_nLiveParticleCount = 0;
  m_fEmitPartial = 1.0f; // start with at least one particle
  m_vecGravity = Vector3::kZeroVector;
  m_vecPosition = Vector3::kZeroVector;
  m_txtParticleTexture = NULL;
  m_UpdateFunc.clear();
  m_InitFunc.clear();

  m_fPILife = 1.0f;
  m_fPISpeed = 100.0f;
  m_fPIDragValue = 0.0f;
  m_PIFadeIn = 0.0f;
  m_PIFadeOut = 1.0f;
  m_PIFadeMax = 1.0f;
  m_PIRotationSpeed = 0.0f;
  m_PIRotationStopTime = 0.0f;
  m_distFunc = NULL;
  
  // default emit rate is all at once (or at least all in the first .01 secs)
  m_nEmitRate = m_nTotalParticleCount * 100;
  m_sort = false;

  m_vertBuffer = NULL;
  m_indexBuffer = NULL;

  // get system data
  effectDef->Attribute("particleCount", &m_nTotalParticleCount);
  const char *filename = effectDef->Attribute("textureName");

  // initialize effect properties from the xml tag
  initProperties(effectDef);

  // allocate memory needed
  m_Particles = new Particle[m_nTotalParticleCount];
  m_drawOrder = new int[m_nTotalParticleCount];

  initParticles();

  // create vertex and index buffers, and initialize index buffer. We can
  // create the index buffer as static since it doesn't change values; this
  // will increase performance a little.
  m_vertBuffer = new VertexLBuffer(m_nTotalParticleCount * 4, true);
  m_indexBuffer = new IndexBuffer(m_nTotalParticleCount * 2);
  initIndexBuffer();
  
  // this should be replaced with the renderer version of loading a texture
  gDirectoryManager.setDirectory(eDirectoryTextures);
  D3DXIMAGE_INFO structImageInfo; //image information
  HRESULT hres=D3DXCreateTextureFromFileEx(pD3DDevice, filename,
    0,0,1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,D3DX_FILTER_NONE,
    D3DX_DEFAULT,0,&structImageInfo,NULL, &m_txtParticleTexture);
}


ParticleEffect::~ParticleEffect()
{
  if(m_Particles != NULL)
  {
    delete[] m_Particles;
    m_Particles = NULL;
  }

  if(m_drawOrder != NULL)
  {
    delete[] m_drawOrder;
    m_drawOrder = NULL;
  }

  if(m_vertBuffer != NULL)
  {
    delete m_vertBuffer;
    m_vertBuffer = NULL;
  }

  if(m_indexBuffer != NULL)
  {
    delete m_indexBuffer;
    m_indexBuffer = NULL;
  }

  m_txtParticleTexture->Release();
  m_txtParticleTexture = NULL;
}


/// The indices in the index buffer will never change, so we set it once and
/// we're done.
void ParticleEffect::initIndexBuffer()
{
  m_indexBuffer->lock();

  // make a pattern of
  //    0---1
  //    |  /|
  //    | / |
  //    |/  |
  //    2---3
  // for each particle

  for(int i=0; i<m_nTotalParticleCount; i++)
  {
    int vertOffset = i * 4;
    int triOffset = i * 2;

    (*m_indexBuffer)[triOffset].index[0]   = vertOffset;
    (*m_indexBuffer)[triOffset].index[1]   = vertOffset + 1;
    (*m_indexBuffer)[triOffset].index[2]   = vertOffset + 2;

    (*m_indexBuffer)[triOffset+1].index[0] = vertOffset + 2;
    (*m_indexBuffer)[triOffset+1].index[1] = vertOffset + 1;
    (*m_indexBuffer)[triOffset+1].index[2] = vertOffset + 3;
  }

  m_indexBuffer->unlock();
}


/// \param effectDef XML tag containing the effect definition
void ParticleEffect::initProperties(TiXmlElement *effectDef)
{
  // get first property tag
  TiXmlElement *prop = effectDef->FirstChildElement();

  PropertyFunc propFn = NULL; // will store the return value of property map
  ParticlePropertyMapper *mapper = ParticlePropertyMapper::getInstance();

  // loop through all the property tags
  while(prop)
  {
    std::string property = prop->Value(); // get the name of the property tag
    propFn = mapper->getFunction(property); // get the corresponding function

    if(propFn != NULL) // if function pointer is not null, it's a valid tag
      bool result = (*this.*propFn)(prop); // ugly code needed to call member function

    prop = prop->NextSiblingElement();
  }
}

void ParticleEffect::initParticles()
{
  // initialize draw order to straight through
  for(int i=0; i<m_nTotalParticleCount; i++)
  {
    m_drawOrder[i] = i;
    
    m_Particles[i].birthed = false;
    m_Particles[i].size = 1.0f;

    m_Particles[i].vTop = m_Particles[i].uLeft = 0.0f;
    m_Particles[i].vBottom = m_Particles[i].uRight = 1.0f;

    m_Particles[i].rotation = 0.0f;
  }
}

void ParticleEffect::start()
{
  initParticles();
  m_bIsDead = false;
  m_IsDying = false;

  m_nLiveParticleCount = 0;
  m_fEmitPartial = 1.0f;
}


/// \param elapsedTime Amount of time that has elapsed (in seconds) since last update 
void ParticleEffect::update(float elapsedTime)
{
  m_fElapsedTime = elapsedTime; // store time in class composition in case
                                // other update functions need it

  killParticles();  // cull old particles
  birthParticles(); // create new particles

  for(int i=0; i<m_nLiveParticleCount; i++)
  {
    Particle *part = &(m_Particles[m_drawOrder[i]]);

    // Here we use v = v + gt - vdt where v is velocity, g is gravity and d is
    // drag. This is a bad approximation but it works.
    // We also use p = p + vt, again this is a bad approximation, but fast
    part->velocity +=
      (m_vecGravity - part->velocity * part->drag) * m_fElapsedTime;
    part->position += part->velocity * m_fElapsedTime;
  }

  // call additional update functions
  for(UpdateFuncIter iter = m_UpdateFunc.begin(); iter != m_UpdateFunc.end(); iter++)
  {
    (*this.*(*iter))(); // really ugly looking code to call all relevant update functions
  }
}


/// Performs update operations specific to the alpha value of each particle.
/// Using this function, a live particle's alpha value will be zero at birth,
/// increase linearly to a value of m_PIFadeMax over a time of m_PIFadeIn secs,
/// remain at m_PIFadeMax until m_PIFadeOut secs, and then decrease linearly
/// to a value of 0, reaching 0 when lifeleft reaches 0.
void ParticleEffect::updateFade()
{
  Particle *part = NULL; // shorthand for the current particle
  int maxAlpha = (int)(255.0f * m_PIFadeMax); // precalculate max alpha

  for(int i=0; i<m_nLiveParticleCount; i++)
  {
    part = &m_Particles[m_drawOrder[i]];
    int alpha = 0;
    float percentLife = 0;

    // calculate percent of life lived from life left
    percentLife = 1.0f - (part->lifeleft / m_fPILife);

    if(percentLife < m_PIFadeIn) // fade in to max alpha value
    {
      alpha = (int)(255.0f * (percentLife / m_PIFadeIn) * m_PIFadeMax);
    }
    else if(percentLife > m_PIFadeOut) // fade out to alpha value of zero
    {
      alpha = (int)(255.0f * ((1.0f - percentLife)/(1.0f - m_PIFadeOut)) * m_PIFadeMax);
    }
    else // maintain max alpha value
    {
      alpha = maxAlpha;
    }

    if(alpha < 0)
      alpha = 0;
    else if(alpha > 255)
      alpha = 255;

    part->color = ((part->color & 0x00FFFFFF) | (alpha << 24));

    part++;
  }
}


/// Updates the particles' rotation values based on time elapsed. Current
/// rotation speed is calculated, then the angle is updated based on time
/// elapsed and new rotation speed.
void ParticleEffect::updateRotation()
{
  for(int i=0; i<m_nLiveParticleCount; i++)
  {
    Particle *part = &m_Particles[m_drawOrder[i]]; // shorthand

    float angularSpeed =
      part->rotationSpeed * (part->rotationStopTime / m_PIRotationStopTime);

    part->rotation += angularSpeed * m_fElapsedTime;

    part->rotationStopTime -= m_fElapsedTime;

    if(part->rotationStopTime < 0.0f)
      part->rotationStopTime = 0.0f;
  }
}


/// Rendering the particles involves looping through every live particle
/// and writing it's relevant data to a vertex buffer, and then rendering
/// that vertex buffer.
void ParticleEffect::render()
{
  if(m_nLiveParticleCount == 0) // make sure we have something to render
    return;

  if(m_sort)
    sort();

  // save render states before starting
  DWORD lighting;
  DWORD alphablend;
  DWORD zwrite;
  DWORD zenable;
  DWORD srcblend;
  DWORD destblend;

  pD3DDevice->GetRenderState(D3DRS_LIGHTING, &lighting);
  pD3DDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &alphablend);
  pD3DDevice->GetRenderState(D3DRS_ZWRITEENABLE, &zwrite);
  pD3DDevice->GetRenderState(D3DRS_ZENABLE, &zenable);
  pD3DDevice->GetRenderState(D3DRS_SRCBLEND, &srcblend);
  pD3DDevice->GetRenderState(D3DRS_DESTBLEND, &destblend);

  // set up particle engine states
  pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
  pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
  if(m_sort)
    pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
  else
    pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
  pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
  pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
  pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
  //pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
  //pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
  
  // set texture operations for alpha blending by setting the color
  // to texture color times diffuse color, and alpha taken entirely from
  // texture value
  pD3DDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
  pD3DDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
  pD3DDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);
  pD3DDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);  
  pD3DDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);    
  pD3DDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_DIFFUSE);
  
  // get camera right and up vectors to figure out how to orient the sprites
  D3DXMATRIXA16 view;
  pD3DDevice->GetTransform(D3DTS_VIEW, &view);

  Vector3 vecRight = Vector3(view._11, view._21, view._31);
  Vector3 vecUp = Vector3(view._12, view._22, view._32);
  Vector3 vecForward = Vector3(view._13, view._23, view._33);

  // precalculate corners
  Vector3 ul, ur, bl, br;
  //ul = -vecRight + vecUp; // upper left
  //ur = vecRight + vecUp;  // upper right
  //bl = -vecRight - vecUp; // bottom left
  //br = vecRight - vecUp;  // bottom right

  pD3DDevice->SetTexture(0, m_txtParticleTexture);

  if(!m_vertBuffer->lock())
  {
    return;
  }

  // shorthand to the current vertex
  RenderVertexL *vert = &((*m_vertBuffer)[0]);

  // although these values are the same for all particles (except color.alpha),
  // you could implement some randomness, at which point there would be a
  // reason to assign to them with every iteration of the loop
  unsigned int color;
  float size = m_fPISize/2.0f; // half of m_fPISize in each direction
  float tTop = 0;
  float tBottom = 1.0f;
  float tLeft = 0;
  float tRight = 1.0f;

  // loop through all live particles to assign proper values to the vertices
  for (int i=0; i<m_nLiveParticleCount; i++)
  {
    Vector3 pos = m_Particles[m_drawOrder[i]].position;
    color = m_Particles[m_drawOrder[i]].color;

    Vector3 myUp, myRight;
    Quaternion q;
    q.setToRotateAboutAxis(vecForward, m_Particles[m_drawOrder[i]].rotation);

    RotationMatrix r;
    r.fromObjectToInertialQuaternion(q);

    myUp = r.objectToInertial(vecUp);
    myRight = r.objectToInertial(vecRight);

    ul = -myRight + myUp; // upper left
    ur = myRight + myUp;  // upper right
    bl = -myRight - myUp; // bottom left
    br = myRight - myUp;  // bottom right

    vert->p = pos + ul*size;
    vert->argb = color;
    vert->u = tLeft;
    vert->v = tTop;
    vert++;

    vert->p = pos + ur*size;
    vert->argb = color;
    vert->u = tRight;
    vert->v = tTop;
    vert++;

    vert->p = pos + bl*size;
    vert->argb = color;
    vert->u = tLeft;
    vert->v = tBottom;
    vert++;

    vert->p = pos + br*size;
    vert->argb = color;
    vert->u = tRight;
    vert->v = tBottom;
    vert++;
  }

  m_vertBuffer->unlock();

  gRenderer.render(
    m_vertBuffer,
    m_nLiveParticleCount * 4,
    m_indexBuffer,
    m_nLiveParticleCount * 2);

  // restore render states
  pD3DDevice->SetRenderState(D3DRS_LIGHTING, lighting);
  pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, alphablend);
  pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, zwrite);
  pD3DDevice->SetRenderState(D3DRS_ZENABLE, zenable);
  pD3DDevice->SetRenderState(D3DRS_SRCBLEND, srcblend);
  pD3DDevice->SetRenderState(D3DRS_DESTBLEND, destblend);

}


void ParticleEffect::birthParticles()
{
  if(m_IsDying) // if we are "dying", then every particle has been created.
    return;     // we can skip the birthing process

  // Tracking partial particles is important if the emit rate is below
  // one per frame, since you would need emitrate*time to be greater than
  // one to create any.

  // To track partials, we use explicit conversions to get at the partial data.
  // Add to what we have already from last time
  m_fEmitPartial += (float)m_nEmitRate * m_fElapsedTime;

  // Set emit to be the number of complete particles to create
  int emit = (int)(m_fEmitPartial);

  // Recompute the partial
  m_fEmitPartial -= (float)emit;

  for(int i=0; i < emit && m_nLiveParticleCount < m_nTotalParticleCount; i++)
  {
    int index = m_drawOrder[m_nLiveParticleCount];
    if(initParticle(index))
      m_nLiveParticleCount++; // success, so add one to our number of live ones
    else
      return; // if initParticle returned false, then we are done creating new
              // particles since no more will be created (non-cycled systems)
  }
}


/// \param i Index of the particle to initialize
/// \return True if the particle was initialized, false otherwise
bool ParticleEffect::initParticle(int i)
{
  // check bounds on i and current number of particles
  if(
    i < 0 || // no negative i
    i >= m_nTotalParticleCount || // no i greater than total number of particles
    m_nLiveParticleCount >= m_nTotalParticleCount) // current number can't exceed max
  {
    return false;
  }

  Particle *p = &(m_Particles[i]);

  if(!m_bCycleParticles && p->birthed) // if we're not recycling particles
  {
    m_IsDying = true;
    return false; // don't reinitialize
  }

  // one time initializations, don't assign these values after the
  // particle is "birthed".
  if(!p->birthed)
  {
    p->size = m_fPISize;
    p->drag = m_fPIDragValue;
    p->color = m_cPIColor;
    p->birthed = true;
  }

  p->velocity = (*m_distFunc)() * m_fPISpeed;
  p->position = m_vecPosition;
  p->lifeleft = m_fPILife;

  // call all other relevant init functions
  for(InitFuncIter iter = m_InitFunc.begin(); iter != m_InitFunc.end(); iter++)
  {
    (*this.*(*iter))(p); // gotta love syntax
  }

  return true;
}

/// \param p Pointer to particle to initialize
void ParticleEffect::initParticleRotation(Particle *p)
{
  p->rotationSpeed = (ParticleUtil::randf() - 0.5f) * m_PIRotationSpeed * 2.0f;
  p->rotationStopTime = m_PIRotationStopTime;
}


/// Updates particles ages and removes any particles that have expired
void ParticleEffect::killParticles()
{
  // By starting from the back, we do less work. See ParticleEffect::killParticle
  // for details.
  for(int i=m_nLiveParticleCount-1; i>=0; i--)
  {
    m_Particles[m_drawOrder[i]].lifeleft -= m_fElapsedTime;

    if(m_Particles[m_drawOrder[i]].lifeleft < 0.0f)
    {
      killParticle(i);
    }
  }

  // if we're not cycling and there are none left
  if(m_IsDying && m_nLiveParticleCount == 0)
    m_bIsDead = true; // we're dead
}


/// Kills a particle by shifting all the particles from index to the last
/// particle drawn forward one, then putting the killed particle index at the
/// end of the list
/// \param index Draw order index of the particle to kill
/// \return True if the particle was killed, false otherwise
bool ParticleEffect::killParticle(int index)
{
  if(0 <= index && index < m_nTotalParticleCount)
  {
    --m_nLiveParticleCount;

    // if index is the last particle being drawn, we're done
    if(index == m_nLiveParticleCount)
      return true;

    int tmp = m_drawOrder[index]; 

    for(int i=index; i<m_nLiveParticleCount; i++)
    {
      m_drawOrder[i] = m_drawOrder[i+1];
    }

    m_drawOrder[m_nLiveParticleCount] = tmp;

    return true;
  }
  else
    return false;
}


void ParticleEffect::sort()
{
  if(!m_sort)
    return;

  // get distance to the camera for each particle
  Vector3 camPos = gRenderer.getCameraPos();
  for(int i=0; i<m_nTotalParticleCount; i++)
  {
    // magnitude squared saves some time since square root is expensive
    m_Particles[i].distance =
      (m_Particles[i].position - camPos).magnitudeSquared();
  }

  /****************************************************************************
  *                                                                           *
  *  Simple bubble sort here. You could implement a faster sort here but it   *
  *  may not be worth it. Bubble sort is n*p operations, where p is the       *
  *  number of elements out of place. Since very few particles will become    *
  *  out of order from frame to frame, p remains very small. As long as p is  *
  *  comparable to log(n), bubble sort will perform similarly to a            *
  *  O(n log(n)) algorithm (eg quicksort or mergesort), but without the       *
  *  added cpu overhead                                                       *
  *                                                                           *
  ****************************************************************************/

  bool swapped;
  do
  {
    swapped = false;

    for(int i=0; i<m_nLiveParticleCount-1; i++)
    {
      int next = i+1;

      if(m_Particles[m_drawOrder[i]].distance < m_Particles[m_drawOrder[next]].distance)
      {
        swap<int>(m_drawOrder[i], m_drawOrder[next]);

        swapped = true;
      }
    }
  }
  while(swapped);
}


/// \param prop XML tag containing the property values
/// \return True if the property was set successfully, false otherwise
bool ParticleEffect::setEmit(TiXmlElement *prop)
{
  if(prop->Attribute("rate") != NULL)
    prop->Attribute("rate", &m_nEmitRate);
  
  if(prop->Attribute("shape") != NULL)
    m_distFunc = ParticleUtil::getEDTFunc(prop->Attribute("shape"));

  return true;
}

/// \param prop XML tag containing the property values
/// \return True if the property was set successfully, false otherwise
bool ParticleEffect::setSort(TiXmlElement *prop)
{
  m_sort = (atoi(prop->Attribute("value")) != 0);
  return true;
}

/// \param prop XML tag containing the property values
/// \return True if the property was set successfully, false otherwise
bool ParticleEffect::setGravity(TiXmlElement *prop)
{
  m_vecGravity = atovec3(prop->Attribute("value"));
  return true;
}

/// \param prop XML tag containing the property values
/// \return True if the property was set successfully, false otherwise
bool ParticleEffect::setCycle(TiXmlElement *prop)
{
  m_bCycleParticles = (atoi(prop->Attribute("value")) != 0);
  return true;
}

/// \param prop XML tag containing the property values
/// \return True if the property was set successfully, false otherwise
bool ParticleEffect::setParticleLife(TiXmlElement *prop)
{
  m_fPILife = (float)atof(prop->Attribute("value"));
  return true;
}

/// \param prop XML tag containing the property values
/// \return True if the property was set successfully, false otherwise
bool ParticleEffect::setParticleSpeed(TiXmlElement *prop)
{
  m_fPISpeed = (float)atof(prop->Attribute("value"));
  return true;
}

/// \param prop XML tag containing the property values
/// \return True if the property was set successfully, false otherwise
bool ParticleEffect::setParticleColor(TiXmlElement *prop)
{
  m_cPIColor = atocolor(prop->Attribute("value"));
  return true;
}

/// \param prop XML tag containing the property values
/// \return True if the property was set successfully, false otherwise
bool ParticleEffect::setParticleSize(TiXmlElement *prop)
{
  m_fPISize = (float)atof(prop->Attribute("value"));
  return true;
}

/// \param prop XML tag containing the property values
/// \return True if the property was set successfully, false otherwise
bool ParticleEffect::setParticleDrag(TiXmlElement *prop)
{
  m_fPIDragValue = (float)atof(prop->Attribute("value"));
  return true;
}

/// \param prop XML tag containing the property values
/// \return True if the property was set successfully, false otherwise
bool ParticleEffect::setParticleFade(TiXmlElement *prop)
{
  if(prop->Attribute("fadein") != NULL)
  {
    double tmp;
    prop->Attribute("fadein", &tmp);
    m_PIFadeIn = (float)tmp;
  }

  if(prop->Attribute("fadeout") != NULL)
  {
    double tmp;
    prop->Attribute("fadeout", &tmp);
    m_PIFadeOut = (float)tmp;
  }

  if(prop->Attribute("fademax") != NULL)
  {
    double tmp;
    prop->Attribute("fademax", &tmp);
    m_PIFadeMax = (float)tmp;
  }

  m_UpdateFunc.push_back(&ParticleEffect::updateFade);

  return true;
}

/// \param prop XML tag containing the property values
/// \return True if the property was set successfully, false otherwise
bool ParticleEffect::setParticleRotation(TiXmlElement *prop)
{
  if(prop->Attribute("initial") != NULL)
  {
    double tmp;
    prop->Attribute("initial", &tmp);
    m_PIRotationSpeed = (float)tmp;
  }

  if(prop->Attribute("stoptime") != NULL)
  {
    double tmp;
    prop->Attribute("stoptime", &tmp);
    m_PIRotationStopTime = (float)tmp;
  }

  m_UpdateFunc.push_back(&ParticleEffect::updateRotation);
  m_InitFunc.push_back(&ParticleEffect::initParticleRotation);

  return true;
}


