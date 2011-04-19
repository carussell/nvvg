//Random.cpp
//random number generator

//Cryptographically secure random number generator
//modified from the book "Writing Secure Code" by
//Michael Howard and David LeBlanc, Microsoft Press, 2003.

//This is a FIPS 140-1 compliant algorithm.

//This way cool random number generator automatically reseeds itself
//from a variety of sources, including time of day.

//Copyright Ian Parberry, 2004
//Last updated February 18, 2004

#include "random2.h"

CRandom::CRandom(){ //constructor
  m_hProv=NULL; //default
  CryptAcquireContext(&m_hProv,NULL,NULL,
    PROV_RSA_FULL,CRYPT_VERIFYCONTEXT);
}

CRandom::~CRandom(){ //destructor
  if(m_hProv)CryptReleaseContext(m_hProv, 0); //safe release
}

BOOL CRandom::get(void* lpGoop,DWORD cbGoop){
  if(!m_hProv)return FALSE;
  return CryptGenRandom(m_hProv,cbGoop,
    reinterpret_cast<LPBYTE>(lpGoop));
}
  
int CRandom::number(int i,int j){  
  //return random number in  range i..j
  DWORD r; //random bytes
  get(&r,sizeof r); //get some random bytes into r
  return j>i?r%(j-i+1)+i:r%(i-j+1)+j; //compute result
}

