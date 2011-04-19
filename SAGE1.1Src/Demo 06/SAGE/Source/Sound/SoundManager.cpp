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

/// \file SoundManager.cpp
/// \brief Code for the sound manager class SoundManager.

/// Copyright Ian Parberry, 2004.
/// Last updated October 1, 2004.

#include <stdio.h>

#include "Common/Vector3.h"
#include "Common/EulerAngles.h"
#include "Common/RotationMatrix.h"
#include "SoundManager.h"
#include "DirectoryManager/DirectoryManager.h"
#include "TinyXML/tinyxml.h"

/// SoundManager constructor.
/// Sets member variables to sensible values and starts DirectSound. Member variable
/// m_bOperational is set to TRUE is it is able to start DirectSound and set the 
/// cooperative level correctly.

SoundManager::SoundManager()
    : m_lpDirectSound(NULL),m_bOperational(false)
{ //constructor

  m_nCount = 0; //no sounds yet
}

/// SoundManager destructor.
/// Reclaims all dynamic memory and releases DirectSound.

SoundManager::~SoundManager()
{ //destructor
  shutdown();
}

/// Initializes the sound manager for use with a given window.
/// \param hwnd Specifies the handle of the window to use the manager.
/// \param size Specifies the maximum number of sounds managed by the manager.
void SoundManager::init(HWND hwnd,int size)
{
  // If already initialized, we're done
  if(m_bOperational)
    return;
  
  //start DirectSound
  m_bOperational = SUCCEEDED(DirectSoundCreate8(NULL, &m_lpDirectSound, NULL));
  if(!m_bOperational)return; //bail if we failed

  //set the cooperative level
  if(!(m_bOperational = SUCCEEDED(m_lpDirectSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY))))
  {
    m_lpDirectSound->Release();
    m_lpDirectSound = NULL;
    return;
  }
  
  // Grab the primary buffer
  DSBUFFERDESC dsbdesc;
  ZeroMemory(&dsbdesc,sizeof(DSBUFFERDESC));
  dsbdesc.dwSize = sizeof(DSBUFFERDESC); 
  dsbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRL3D | DSBCAPS_CTRLVOLUME;
  dsbdesc.dwBufferBytes = 0;  // Buffer size is determined by sound hardware. 
  dsbdesc.lpwfxFormat = NULL; // Must be NULL for primary buffers. 
  if(FAILED(m_lpDirectSound->CreateSoundBuffer(&dsbdesc,&m_lpPrimaryBuffer,NULL)))
  {
    m_lpDirectSound->Release();
    m_lpDirectSound = NULL;
    return;
  }
  
  // Grab the listener interface
  if(FAILED(m_lpPrimaryBuffer->QueryInterface(IID_IDirectSound3DListener8,(LPVOID *)&m_lpListener)))
  {
    m_lpPrimaryBuffer->Release();
    m_lpPrimaryBuffer = NULL;
    m_lpDirectSound->Release();
    m_lpDirectSound = NULL;
    return;
  }
  
  // Allocate sound arrays
  m_lpBuffer.resize(size,NULL);
  m_lpBuffer3D.resize(size,NULL);
  m_lpGranted.resize(size,NULL);
  m_nInstanceCount.resize(size,0);
  m_soundNames.resize(size,"");
}

/// Frees dynamic memory and resources and shuts down the manager.
void SoundManager::shutdown()
{
  if(!m_bOperational)return;
  clear(); //clear all buffers
  if(m_lpListener)
  {
    m_lpListener->Release();
    m_lpListener = NULL;
  }
  if(m_lpPrimaryBuffer)
  {
    m_lpPrimaryBuffer->Release();
    m_lpPrimaryBuffer = NULL;
  }
  if(m_lpDirectSound) //safe release
  {
    m_lpDirectSound->Release(); //release direct sound
    m_lpDirectSound = NULL;
  }
  m_bOperational = false;
}

/// Clear the sound manager.
/// Returns the sound manager to a pristine state by releasing and deleting all 
/// sound buffers, then deleting all dynamic arrays.
void SoundManager::clear()
{ //clear all sounds

  if(!m_bOperational)return; //bail if not initialized

  stop(); //stop all sounds (paranoia)

  for(int i=0; i<m_nCount; i++){ //for each sound
    for(int j=0; j<m_nInstanceCount[i]; j++){ //for each instance
      m_lpBuffer3D[i][j]->Release();
      m_lpBuffer3D[i][j] = NULL; //probably not needed
      m_lpBuffer[i][j]->Release(); //release the sound
      m_lpBuffer[i][j] = NULL; //probably not needed
    }

    //reclaim memory
    delete [] m_lpBuffer3D[i];
    m_lpBuffer3D[i] = NULL;
    delete [] m_lpBuffer[i];
    m_lpBuffer[i] = NULL;
  }
  m_soundNames.clear();


  m_nCount = 0; //no sounds left (hopefully)
}

/// Loads all sound files in an XML
/// \param fileName Name of XML file
/// \note The XML file must be located in the default XML directory.
/// Sounds will be loaded from the default sound directory.
void SoundManager::parseXML(const char* fileName)
{
  
  // Variables
	TiXmlDocument file; // the XML document object
	TiXmlElement *sounds = NULL; // used to parse XML
	TiXmlElement *sound = NULL; // used to parse XML	
  const char* soundFileName; // temporarily holds sound filename
  int soundInstances; // temporarily holds number of instances
  int soundHandle; // temporarily holds sound handle
  double minDistance;  
  double maxDistance;
	
	// set the directory to the XML directory	
	gDirectoryManager.setDirectory(eDirectoryXML);

	// load the xml specified and leave on failure and print error
	if (!file.LoadFile(fileName))
  {
    assert(false);
		return;
  }
	
	// get the list of commands
	sounds = file.FirstChildElement("sounds");
  if (sounds == NULL)
  {
    assert(false);
		return;
  }
	
  // set directory to the sound directory
  gDirectoryManager.setDirectory(eDirectorySounds);

	// loop through every sound in the sounds list
	sound = sounds->FirstChildElement();	
	while(sound != NULL)
	{			
    soundFileName = sound->Attribute("name");
    sound->Attribute("count", &soundInstances);

    soundHandle = gSoundManager.load(soundFileName, soundInstances);

    
    sound->Attribute("mindistance",&minDistance);
    sound->Attribute("mindistance",&maxDistance);

    // set distance if specified in the XML
    if (!(minDistance == 0 && maxDistance == 0))
      gSoundManager.setDistance(soundHandle,(float)minDistance, (float)maxDistance);

		// go to the next entry in the XML
		sound = sound->NextSiblingElement();
	}


}

/// Load a sound from a file into a sound buffer.
/// Loads a sound from a file into a temporary buffer. Creates a DirectSound
/// sound buffer of the right size and copies it over. Creates a number of instances
/// of the sound so that we can play overlapping instances of the sound.
/// \param filename name of file to be loaded
/// \param instances number of instances of sound that can be played simultaneously
/// \return The index of the loaded sound, or less than 0 on failure.
/// \note If the sound is already loaded, the sound will not be loaded again.
/// Instead, a handle to the previously loaded sound will be returned.
int SoundManager::load(std::string filename, int instances)
{
  int length; //length of sound
  SoundBuffer sound = {NULL,0,0,0};

  //bail out if necessary
  if(!m_bOperational)return -1; //bail if not initialized

  //search to see if the sound was already loaded  
  for (int i =0; i < m_nCount; i++)
    if (filename == m_soundNames[i])
      return i;

  // Resize vectors if necessary
  size_t size = m_lpBuffer.size();
  if(m_nCount == size - 1)
  {
    m_lpBuffer.resize(size * 2);
    m_lpBuffer3D.resize(size * 2);
    m_lpGranted.resize(size * 2);
    m_nInstanceCount.resize(size * 2);
    m_soundNames.resize(size * 2);
  }

  //load sound data from file
  length = loadSound(filename, sound); //load sound from file
  m_nInstanceCount[m_nCount]=instances; //record number of instances
  createBuffers(m_nCount, sound); //create buffers
  loadBuffers(m_nCount, sound); //load into buffer
  m_soundNames[m_nCount] = filename; //save filename

  //clean up and exit
  return m_nCount++; //increment counter
}

/// Play a sound.
/// If the sound has multiple instances, locates a instance that is not currently playing,
/// and plays it. If there is no instance available, nothing happens.
/// \param index index of sound to be played
/// \param looping TRUE if sound is to be looped
void SoundManager::playNext(int index, bool looping)
{ //play sound

  if(!m_bOperational)return; //bail if not initialized
  if(index<0||index>=m_nCount)return; //bail if bad index

  int instance=0; //current instance
  DWORD status; //status of that instance

  //get status of first instance
  if(FAILED(m_lpBuffer[index][instance]->GetStatus(&status)))
    status = DSBSTATUS_PLAYING; //assume playing if failed

  //find next unplayed instance, if any
  while(instance < m_nInstanceCount[index]&&
  (status&DSBSTATUS_PLAYING)){ //while current instance in use
    if(++instance < m_nInstanceCount[index]) //go to next instance
      if(FAILED(m_lpBuffer[index][instance]->GetStatus(&status)))
        status = DSBSTATUS_PLAYING; //assume playing if failed
  }

  //play instance
  if(instance < m_nInstanceCount[index]){ //if unused instance found
    m_lpBuffer[index][instance]->
      Play(0, 0, looping?DSBPLAY_LOOPING:0); //play it
  }
}

/// Plays a particular instance of a sound.
/// \param index Specifies the index of the sound to be played.
/// \param instance Specifies the instance of the sound to be played.
/// \param looping Specifies whether the sound will loop.
void SoundManager::play(int index, int instance, bool looping)
{
  if(!m_bOperational)return; //bail if not initialized
  if(index<0||index>=m_nCount)return; //bail if bad index
  if(instance < 0 || instance >= m_nInstanceCount[index]) return;
  m_lpBuffer[index][instance]->Play(0, 0, looping?DSBPLAY_LOOPING:0); //play it
}

/// Stop sound.
/// Stops all instances of a sound from playing, and resets them back to the
/// start of the sound so that they start properly next time they are played.
///\param index index of sound to stop
void SoundManager::stop(int index)
{ //stop playing sound

  if(!m_bOperational)return; //bail if not initialized 
  if(index<0||index>=m_nCount)return; //bail if bad index

  for(int i=0; i<m_nInstanceCount[index]; i++){ //for each instance
    m_lpBuffer[index][i]->Stop(); //stop playing
    m_lpBuffer[index][i]->SetCurrentPosition(0); //rewind
  }
}

/// \param index Specifies the index of the sound to be played.
/// \param instance Specifies the instance of the sound to be played.
/// Stops a particular instance of a sound.
void SoundManager::stop(int index, int instance)
{ //stop playing sound

  if(!m_bOperational)return; //bail if not initialized 
  if(index<0||index>=m_nCount)return; //bail if bad index
  if(instance < 0 || instance >= m_nInstanceCount[index]) return;

  m_lpBuffer[index][instance]->Stop(); //stop playing
  m_lpBuffer[index][instance]->SetCurrentPosition(0); //rewind
}

/// Stop all sounds.
/// Stops all sounds from playing. It simply loops through all sounds and calls the
/// other \p stop() function to do all the work.
void SoundManager::stop(void)
{ //stop playing sound
  if(!m_bOperational)return; //bail if not initialized 
  for(int index=0; index<m_nCount; index++) //for each sound
    stop(index); //stop that sound
}

/// Returns a handle to a previously loaded sound
/// \param fileName Name of sound file.
/// \return Handle to requested sound.  This value will be 0 on failure.
int SoundManager::requestSoundHandle(const char* fileName)
{
  //bail out if necessary
  if(!m_bOperational)return -1; //bail if not initialized

  //search to see if the sound was already loaded  
  for (int i =0; i < m_nCount; i++)
    if (fileName == m_soundNames[i])
      return i;

  return 0;
}

/// Requests an available instance of a sound.  When an application is finished
/// using that instance, it should free the instance with \p releaseInstance().
/// \param index Specifies the index of the sound to be requested.
/// \return If an instance is available, returns the available instance; if not,
///     returns NOINSTANCE.
/// \note This function defines an available instance as an instance that
///     <ul>
///       <li>has not been requested without a corresponding release, and</li>
///       <li>is not currently playing.</li>
///     </ul>
int SoundManager::requestInstance(int index)
{
  if(!m_bOperational)return NOINSTANCE; //bail if not initialized
  if(index<0||index>=m_nCount)return NOINSTANCE; //bail if bad index

  int instance=0; //current instance
  DWORD status; //status of that instance

  //get status of first instance
  if(m_lpGranted[index][instance] || FAILED(m_lpBuffer[index][instance]->GetStatus(&status)))
    status = DSBSTATUS_PLAYING; //assume playing if failed

  //find next unplayed instance, if any
  while(instance < m_nInstanceCount[index]&&
  (status&DSBSTATUS_PLAYING)){ //while current instance in use
    if(++instance < m_nInstanceCount[index]) //go to next instance
      if(m_lpGranted[index][instance] || FAILED(m_lpBuffer[index][instance]->GetStatus(&status)))
        status = DSBSTATUS_PLAYING; //assume playing if failed
  }

  //play instance
  if(instance < m_nInstanceCount[index]){ //if unused instance found
    m_lpGranted[index][instance] = true;
    return instance;
  }
  else
    return NOINSTANCE;
}

/// Releases an instance of a sound, making it available to other requests.
/// \param index Specifies the index of the sound to be released.
/// \param instance Specifies the instance of the sound to be released.
/// \note The sound will not actually become available for request until it
///     ceases playing, either through finishing the sound or a call to \p stop().
void SoundManager::releaseInstance(int index, int instance)
{
  if(!m_bOperational)
    return;
  if(index < 0 || index >= m_nCount)
    return;
  if(instance < 0 || instance >= m_nInstanceCount[index])
    return;
  m_lpGranted[index][instance] = false;
}

/// Sets the global distance rolloff factor for sound attenuation.  The rolloff
/// factor affects the degree to which distance from a sound source affects
/// the volume of the sound.  A factor of 1.0f indicates "real-world" attenuation,
/// while other values represent a multiple of the real-world rolloff factor.
/// \param rolloffFactor 0.0f to deactivate attenuation, or a value in the
///     interval (0.0f, 10.0f] for the rolloff factor.
/// \note Behavior is undefined for values outside of the interval [0.0, 10.0].
void SoundManager::setRolloff(float rolloffFactor)
{
  if(!m_bOperational)return; //bail if not initialized
  m_lpListener->SetRolloffFactor(rolloffFactor, DS3D_IMMEDIATE);
}

/// Sets the "size" of a unit in space for Doppler calculation.  The Doppler unit
/// determines how many meters are represented by a unit-length vector.  The
/// default value is one meter.  Larger values will exaggerate the Doppler effect.
/// \param meters Specifies the number of meters in a vector unit.
void SoundManager::setDopplerUnit(float meters)
{
  if(!m_bOperational) return;
  m_lpListener->SetDistanceFactor(meters, DS3D_IMMEDIATE);
}

/// Sets the position of the listener.
/// \param position Specifies the position of the listener.
void SoundManager::setListenerPosition(const Vector3 &position)
{
  if(!m_bOperational)return; //bail if not initialized
  m_lpListener->SetPosition(position.x, position.y, position.z,
      DS3D_IMMEDIATE);
}

/// Sets the velocity of the listener.
/// \param velocity Specifies the velocity of the listener.
void SoundManager::setListenerVelocity(const Vector3 &velocity)
{
  if(!m_bOperational)return; //bail if not initialized
  m_lpListener->SetVelocity(velocity.x,velocity.y,velocity.z,
      DS3D_IMMEDIATE);
}

/// Sets the orientation of the listener.
/// \param orientation Specifies the orientation of the listener.
void SoundManager::setListenerOrientation(const EulerAngles &orientation)
{
  if(!m_bOperational) return;
  RotationMatrix m;
  m.setup(orientation);
  m_lpListener->SetOrientation(m.m13,m.m23,m.m33,m.m12,m.m22,m.m32,
      DS3D_IMMEDIATE);
}

/// Sets the position of all instances of a sound.
/// \param index Specifies the index of the sound.
/// \param position Specifies the position of the sound.
void SoundManager::setPosition(int index, const Vector3 &position)
{
  if(!m_bOperational)return; //bail if not initialized
  if(index<0||index>=m_nCount)return; //bail if bad index
  
  for(int instance = 0; instance < m_nInstanceCount[index]; ++instance)
  {
    m_lpBuffer3D[index][instance]->SetPosition(position.x, position.y, position.z,
        DS3D_IMMEDIATE);
  }
}

/// Sets the position of a sound instance.
/// \param index Specifies the index of the sound.
/// \param instance Specifies the instance of the sound.
/// \param position Specifies the position of the sound instance.
void SoundManager::setPosition(int index, int instance, const Vector3 &position)
{
  if(!m_bOperational)return; //bail if not initialized
  if(index<0||index>=m_nCount)return; //bail if bad index
  if(instance < 0 || instance >= m_nInstanceCount[index]) return;
  m_lpBuffer3D[index][instance]->SetPosition(position.x, position.y, position.z,
      DS3D_IMMEDIATE);
}

/// Sets the velocity of all instances of a sound.
/// \param index Specifies the index of the sound.
/// \param velocity Specifies the velocity of the sound.
void SoundManager::setVelocity(int index, const Vector3 &velocity)
{
  if(!m_bOperational)return; //bail if not initialized
  if(index<0||index>=m_nCount)return; //bail if bad index
  
  for(int instance = 0; instance < m_nInstanceCount[index]; ++instance)
  {
    m_lpBuffer3D[index][instance]->SetVelocity(velocity.x, velocity.y, velocity.z,
        DS3D_IMMEDIATE);
  }
}

/// Sets the velocity of a sound instance.
/// \param index Specifies the index of the sound.
/// \param instance Specifies the instance of the sound.
/// \param velocity Specifies the velocity of the sound instance.
void SoundManager::setVelocity(int index, int instance, const Vector3 &velocity)
{
  if(!m_bOperational)return; //bail if not initialized
  if(index<0||index>=m_nCount)return; //bail if bad index
  if(instance < 0 || instance >= m_nInstanceCount[index]) return;
  m_lpBuffer3D[index][instance]->SetVelocity(velocity.x, velocity.y, velocity.z,
      DS3D_IMMEDIATE);
}

/// Sets the position and velocity to that of the listener's.
/// \param index Specifies the index of the sound.
/// \param instance Specifies the instance of the sound.
void SoundManager::setToListener(int index, int instance)
{
  if(!m_bOperational)return; //bail if not initialized
  if(index<0||index>=m_nCount)return; //bail if bad index
  if(instance < 0 || instance >= m_nInstanceCount[index]) return;
  D3DVECTOR pos, vel;
  m_lpListener->GetPosition(&pos);
  m_lpListener->GetVelocity(&vel);
  m_lpBuffer3D[index][instance]->SetPosition(pos.x, pos.y, pos.z,
      DS3D_IMMEDIATE);
  m_lpBuffer3D[index][instance]->SetVelocity(vel.x, vel.y, vel.z,
      DS3D_IMMEDIATE);
}

/// Specifies the minimum and maximum distances of all instances of a sound for attenuation.
/// A sound closer than its minimum distance will play at full volume.
/// As the sound's actual distance increases, the sound will decrease in volume
/// until it reaches its maximum distance.  The default distance values of a sound
/// are 1 unit and 1 billion units, respectively; with these values, sounds will
/// become inaudible long before reaching a distance of 1 billion units.
/// \param index Specifies the index of the sound.
/// \param minDistance Specifies the minimum distance of the sound in vector units.
/// \param maxDistance Specifies the maximum distance of the sound in vector units.
void SoundManager::setDistance(int index, float minDistance, float maxDistance)
{
  if(!m_bOperational)return; //bail if not initialized
  if(index<0||index>=m_nCount)return; //bail if bad index
  
  for(int instance = 0; instance < m_nInstanceCount[index]; ++instance)
  {
    m_lpBuffer3D[index][instance]->SetMinDistance(minDistance, DS3D_IMMEDIATE);
    m_lpBuffer3D[index][instance]->SetMaxDistance(maxDistance, DS3D_IMMEDIATE);
  }
}

/// Specifies the minimum and maximum distances of a particular instance of a sound for attenuation.
/// A sound closer than its minimum distance will play at full volume.
/// As the sound's actual distance increases, the sound will decrease in volume
/// until it reaches its maximum distance.  The default distance values of a sound
/// are 1 unit and 1 billion units, respectively; with these values, sounds will
/// become inaudible long before reaching a distance of 1 billion units.
/// \param index Specifies the index of the sound.
/// \param instance Specifies the instance of the sound.
/// \param minDistance Specifies the minimum distance of the sound in vector units.
/// \param maxDistance Specifies the maximum distance of the sound in vector units.
void SoundManager::setDistance(int index, int instance, float minDistance, float maxDistance)
{
  if(!m_bOperational)return; //bail if not initialized
  if(index<0||index>=m_nCount)return; //bail if bad index
  if(instance < 0 || instance >= m_nInstanceCount[index]) return;

  m_lpBuffer3D[index][instance]->SetMinDistance(minDistance, DS3D_IMMEDIATE);
  m_lpBuffer3D[index][instance]->SetMaxDistance(maxDistance, DS3D_IMMEDIATE);
}
/// Sets the volume for an individual sound.
/// \param index Specifies the index of the sound
/// \param volume Specifies the volume for the sound.  This value can range from 0.0 - 1.0
void SoundManager::setVolume( int index, float volume)
{
  if(index<0||index>=m_nCount) return; //bail if bad index

  int instances = m_nInstanceCount[index];
  
  for(int instance = 0; instance < instances; ++instance)
  {
    LPDIRECTSOUNDBUFFER buffer=m_lpBuffer[index][instance];
    int logarithmicVolume = DSBVOLUME_MIN;
    if ( volume != 0 )
    {
      logarithmicVolume = (int)(2000.0f * log10f((float) volume));
    }
    buffer->SetVolume(logarithmicVolume);
  }
  
  
}


/// Create buffer.
/// Creates a set of buffers for a sound.  The number of instances is expected
/// to be set by the calling function.
/// \param index index in the sound buffer array
/// \param sound Indicates a sound structure whose buffer will be allocated.
/// \return TRUE if it succeeds
BOOL SoundManager::createBuffers(int index, SoundManager::SoundBuffer &sound)
{

  if(!m_bOperational)return FALSE; //bail if not initialized
  if(sound.size<=0)return FALSE; //bail if length info wrong
  
  int instances = m_nInstanceCount[index];

  DSBUFFERDESC dsbdesc;
  PCMWAVEFORMAT pcmwf;

  //init pcmwf, wave format descriptor
  memset(&pcmwf,0,sizeof(PCMWAVEFORMAT));
  pcmwf.wf.wFormatTag = WAVE_FORMAT_PCM;
  pcmwf.wf.nChannels = sound.channels;
  pcmwf.wf.nSamplesPerSec = sound.sampleRate;
  pcmwf.wBitsPerSample = sound.bits;
  pcmwf.wf.nBlockAlign = pcmwf.wf.nChannels * pcmwf.wBitsPerSample / 8;
  pcmwf.wf.nAvgBytesPerSec = pcmwf.wf.nSamplesPerSec * pcmwf.wf.nBlockAlign;

  //init dsbdesc, direct sound buffer descriptor
  memset(&dsbdesc, 0, sizeof(DSBUFFERDESC));
  dsbdesc.dwSize = sizeof(DSBUFFERDESC);
  dsbdesc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_CTRLVOLUME;
  dsbdesc.dwBufferBytes = sound.size;
  dsbdesc.lpwfxFormat = (LPWAVEFORMATEX)&pcmwf;

  //create sound buffers and return success code
  m_lpBuffer[index] = new LPDIRECTSOUNDBUFFER8[instances];
  m_lpBuffer3D[index] = new LPDIRECTSOUND3DBUFFER8[instances];
  m_lpGranted[index] = new bool[instances];
  for(int instance=0; instance < instances; instance++)
  {
    LPDIRECTSOUNDBUFFER buffer = NULL; // Vanilla buffer
    if(FAILED(m_lpDirectSound->CreateSoundBuffer(&dsbdesc, &buffer, NULL)))
    {
      // Failed; clean up any successfully-made buffers
      for(int j = 0; j < instance; ++j)
      {
        m_lpBuffer3D[index][j]->Release();
        m_lpBuffer3D[index][j] = NULL;
        m_lpBuffer[index][j]->Release();
        m_lpBuffer[index][j] = NULL;
      }
      delete [] m_lpGranted[index];
      m_lpGranted[index] = NULL;
      delete [] m_lpBuffer3D[index];
      m_lpBuffer3D[index] = NULL;
      delete [] m_lpBuffer[index];
      m_lpBuffer[index] = NULL;
      return false;
    }
    if(FAILED(buffer->QueryInterface(IID_IDirectSoundBuffer8,(LPVOID *)&(m_lpBuffer[index][instance]))))
    {
      // Failed; clean up any successfully-made buffers
      buffer->Release();
      for(int j = 0; j < instance; ++j)
      {
        m_lpBuffer3D[index][j]->Release();
        m_lpBuffer3D[index][j] = NULL;
        m_lpBuffer[index][j]->Release();
        m_lpBuffer[index][j] = NULL;
      }
      delete [] m_lpGranted[index];
      m_lpGranted[index] = NULL;
      delete [] m_lpBuffer3D[index];
      m_lpBuffer3D[index] = NULL;
      delete [] m_lpBuffer[index];
      m_lpBuffer[index] = NULL;
      return false;
    }
    if(FAILED(m_lpBuffer[index][instance]->QueryInterface(IID_IDirectSound3DBuffer8,(LPVOID *)&(m_lpBuffer3D[index][instance]))))
    {
      // Failed; clean up any successfully-made buffers
      m_lpBuffer[index][instance]->Release();
      m_lpBuffer[index][instance] = NULL;
      buffer->Release();
      for(int j = 0; j < instance; ++j)
      {
        m_lpBuffer3D[index][j]->Release();
        m_lpBuffer3D[index][j] = NULL;
        m_lpBuffer[index][j]->Release();
        m_lpBuffer[index][j] = NULL;
      }
      delete [] m_lpGranted[index];
      m_lpGranted[index] = NULL;
      delete [] m_lpBuffer3D[index];
      m_lpBuffer3D[index] = NULL;
      delete [] m_lpBuffer[index];
      m_lpBuffer[index] = NULL;
      return false;
    }
    buffer->Release();
    buffer = NULL;
    m_lpGranted[index][instance] = false;
  }
  return true;
}

/// Load sound buffer.
/// Copies a sound from a byte buffer into a DirectSound buffer. The hairy part
/// is that when you lock down a DirectSound buffer, you get the buffer in two
/// halves. This function copies the correct two parts of the byte buffer into
/// the DirectSound buffer.
/// \param index index in the sound buffer array
/// \param sound Specifies the sound structure containing the raw sound data and its properties.
/// \return TRUE if it succeeds
BOOL SoundManager::loadBuffers(int index, SoundManager::SoundBuffer &sound)
{

  if(!m_bOperational)return FALSE; //bail if not initialized
  if(sound.size<=0)return FALSE; //bail if length info wrong

  LPVOID w1,w2; //write pointer (use 2 for buffer wraparound)
  DWORD l1,l2; //length of sound to be written to write pointers
  
  int instances = m_nInstanceCount[index];
  
  for(int instance = 0; instance < instances; ++instance)
  {
    LPDIRECTSOUNDBUFFER buffer=m_lpBuffer[index][instance];

    //lock down buffer
    if(buffer->Lock(0, sound.size, &w1, &l1, &w2, &l2, 0) == DSERR_BUFFERLOST){ //if buffer lost
      buffer->Restore(); //restore, then try again
      if(FAILED(buffer->Lock(0, sound.size, &w1, &l1, &w2, &l2, 0)))
        return FALSE; //abort if failed the second time
    }

    //load both halves of sound to buffer
    CopyMemory(w1,sound.buffer,l1); //load first half 
    if(w2!=NULL)CopyMemory(w2, sound.buffer+l1, l2); //load second half

    //unlock the buffer and return
    if(FAILED(buffer->Unlock(w1, l1, w2, l2)))return FALSE;
  }
  return TRUE;

}

/// Load a sound.
/// Creates a byte buffer and loads sound data from a file into that buffer.
/// Cribbed from the SDK sample files.
/// \param filename name of sound file
/// \param sound Specifies the structure containing the sound buffer.
/// \return the length of the byte buffer created

int SoundManager::loadSound(std::string filename, SoundManager::SoundBuffer &sound)
{

  if(!m_bOperational)return 0; //bail if not initialized

  int size=0;
  HMMIO hmfr;
  MMCKINFO parent,child;
  WAVEFORMATEX wfmtx;

  //reclaim memory from buffer, if already used
  delete [] sound.buffer;

  //init parent and child MMCKINFOs
  parent.ckid = (FOURCC)0;
  parent.cksize = 0;
  parent.fccType = (FOURCC)0;
  parent.dwDataOffset = 0;
  parent.dwFlags = 0;
  child = parent;

  //open sound file; note that mmioOpen() erroneously requires a char* even
  //    though it never changes filename 

  MMIOINFO mmioinfo;
  ZeroMemory(&mmioinfo,sizeof(MMIOINFO));
  hmfr=mmioOpen(const_cast<char *>(filename.c_str()), &mmioinfo, MMIO_READ|MMIO_ALLOCBUF);
  if(hmfr==NULL)return NULL;

  //descend into the RIFF
  parent.fccType=mmioFOURCC('W','A','V','E');
  if(mmioDescend(hmfr, &parent, NULL, MMIO_FINDRIFF)){
    mmioClose(hmfr, 0); return NULL; //not a wave file
  }

  //descend to the WAVEfmt
  child.ckid=mmioFOURCC('f', 'm', 't', ' ');
  if(mmioDescend(hmfr, &child, &parent, 0)){
    mmioClose(hmfr, 0); return NULL; //file has no fmt chunk
  }

  //read the WAVEFMT from the wave file
  if(mmioRead(hmfr, (char*)&wfmtx, sizeof(wfmtx)) != sizeof(wfmtx)){
    mmioClose(hmfr, 0); return NULL; //unable to read fmt chunk
  }

  //check wave format
  if(wfmtx.wFormatTag != WAVE_FORMAT_PCM){
    mmioClose(hmfr, 0); return NULL; //WAVE file is not PCM format
  }

  //ascend back to RIFF level
  if(mmioAscend(hmfr, &child, 0)){
    mmioClose(hmfr, 0); return NULL; //unable to ascend
  }

  //descend to the data chunk
  child.ckid=mmioFOURCC('d', 'a', 't', 'a');
  if(mmioDescend(hmfr, &child, &parent, MMIO_FINDCHUNK)){
    mmioClose(hmfr, 0); return NULL; //WAVE file has no data chunk
  }

  //grab memory to store sound
  size = child.cksize;
  sound.buffer = new BYTE[size];
  if(sound.buffer == NULL){
    mmioClose(hmfr, 0); return NULL; //out of memory
  }

  //read the wave data
  if((int)mmioRead(hmfr, (char *)sound.buffer, size) != size){
    //data read failed
    mmioClose(hmfr, 0); delete [] sound.buffer; return NULL; 
  }

  // Fill in wave properties
  sound.size = size;
  sound.channels = wfmtx.nChannels;
  sound.bits = wfmtx.wBitsPerSample;
  sound.sampleRate = wfmtx.nSamplesPerSec;

  //close file and return
  mmioClose(hmfr, 0);
  return size;
}

SoundManager gSoundManager;