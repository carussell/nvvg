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

/// \file SoundManager.h
/// \brief Interface for the SoundManager

/// Copyright Ian Parberry, 2004
/// Edited by the SAGE team, 2005
/// Last updated July 13, 2005

#pragma once

#include <dsound.h> //direct sound
#include <string>
#include <vector>

class Vector3;
class EulerAngles;

/// \brief Manages the sounds for the game.

/// The sound manager class. The sound manager allows you to play multiple 
/// overlapping copies of sounds simultaneously.  It reads settings from the
/// XML settings file, including a list of file names to be loaded. It can load
/// WAV format sounds. The code is currently written so that all sound files
/// must have the same bit depth and sample rate. Multiple copies of sounds are
/// made by sharing the sound data, to save memory.

/// \todo We have sound copies, but the identity of the copy played is
///     opaque to the user.  However, to control the position/velocity of
///     an instance of a sound, the game needs to know the exact copy of
///     the sound being played.  Consider requesting a handle to the copy
///     before playing, then using that handle to set 3D parameters, play
///     it, and update parameters.  To maintain the simple increment
///     method of generating handles, we can create a separate handle space
///     for each sound.  For example, if the game acquires copy #2 of
///     sound #38, then further calls involving that copy use the
///     handle (38,2). We would also need an explicit release request to
///     return the copy to the pool of available copies.

class SoundManager
{
public:
  SoundManager(); ///< Constructs a sound manager.
  ~SoundManager(); ///< Frees resources and destroys the manager..
  void init(HWND hwnd,int size = 256); ///< Initializes the sound manager for use with a given window.
  void shutdown(); ///< Shuts down the sound manager, freeing any dynamically-allocated resources.
  void clear(); ///< Clears all sounds from buffers.
  void parseXML(const char* filename); ///< Loads all sound files in an XML
  int load(std::string filename, int instances=1); ///< Loads a sound from a .WAV file and generates instances.
  void playNext(int index, bool looping = false); ///< Plays the next available instance of a sound.
  void play(int index, int instance, bool looping = false); ///< Plays a given instance of a sound.
  void stop(int index); ///< Stops playing all instances of a sound.
  void stop(int index, int instance); ///< Stops playing an instance of a sound.
  void stop(void); ///< Stops playing all sounds.
  int requestSoundHandle(const char* fileName); ///< Returns a handle to a previously loaded sound
  int requestInstance(int index); ///< Requests an available instance of a sound.
  void releaseInstance(int index, int instance); ///< Releases an instance, making it available for requests.
  
  
  void setRolloff(float rolloffFactor); ///< Sets the global rolloff factor for sound attenuation.
  void setDopplerUnit(float meters); ///< Sets the size of a vector unit (in meters) for Doppler calculations.
  void setListenerPosition(const Vector3 &position); ///< Sets the position of the listener.
  void setListenerVelocity(const Vector3 &velocity); ///<Sets the velocity of the listener.
  void setListenerOrientation(const EulerAngles &orientation); ///<Sets the orientation of the listener.
  void setPosition(int index, const Vector3 &position); ///< Sets the position of a sound.
  void setPosition(int index, int instance, const Vector3 &position); ///< Sets the position of a sound.
  void setVelocity(int index, const Vector3 &velocity); ///< Sets the velocity of a sound.
  void setVelocity(int index, int instance, const Vector3 &velocity); ///< Sets the velocity of a sound.
  void setToListener(int index, int instance); ///< Sets the position and velocity of a sound to the listener.
  void setDistance(int index, float minDistance, float maxDistance); ///< Sets the minimum and maximum distance for a sound
  void setDistance(int index, int instance, float minDistance, float maxDistance); ///< Sets the minimum and maximum distance for a sound
  void setVolume(int index, float volume); ///< Sets the volume for an individual sound

  static const int NOINSTANCE = -1; ///< Indicates an invalid or nonexistent sound instance

private:
  /// A temporary structure that holds sound data.
  
  struct SoundBuffer
  {
    BYTE *buffer;     ///< Raw buffer to hold the sound data.
    int size;         ///< Length of the buffer, in bytes.
    WORD channels;    ///< Number of channels in the sample.
    WORD bits;        ///< Number of bits in the sample.
    DWORD sampleRate; ///< Sample rate in Hertz.
  };

  typedef int SoundIndex;

  int m_nCount; ///< Number of sounds loaded.
  LPDIRECTSOUND8 m_lpDirectSound; ///< DirectSound object.
  LPDIRECTSOUNDBUFFER m_lpPrimaryBuffer; ///< Primary buffer.
  LPDIRECTSOUND3DLISTENER8 m_lpListener; ///< Listener interface.
  
  std::vector<LPDIRECTSOUNDBUFFER8 *> m_lpBuffer; ///< Sound buffers.
  std::vector<LPDIRECTSOUND3DBUFFER8 *> m_lpBuffer3D; ///< 3D sound buffers.
  std::vector<bool *> m_lpGranted; ///< Request flags.
  std::vector<int> m_nInstanceCount; ///< Number of copies of each sound.
  std::vector<std::string> m_soundNames; ///< Records the names of all the sounds loaded
  
  BOOL m_bOperational; ///< TRUE if DirectSound initialized correctly.
  bool isInit; ///< Holds true iff the sound manager has been initialized.

  BOOL createBuffers(int index, SoundManager::SoundBuffer &sound); ///< Create a sound buffer.
  BOOL loadBuffers(int index, SoundManager::SoundBuffer &sound);///< Load a sound buffer.
  int loadSound(std::string filename, SoundManager::SoundBuffer &sound); ///< Load a sound from file.
};

extern SoundManager gSoundManager;