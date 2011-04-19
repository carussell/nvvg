//Random.h
//Header for random number generator

//Copyright Ian Parberry, 2004
//Last updated February 18, 2004

#include <windows.h>
#include <wincrypt.h>

#pragma once

class CRandom {
  public:
    CRandom(); //constructor
    virtual ~CRandom(); //destructor
    //unsigned int number(unsigned int i,unsigned int j); //return random number in i..j
    int number(int i,int j); //return random number in i..j
  private:
    BOOL get(void *lpGoop,DWORD cbGoop); //get random bytes
    HCRYPTPROV m_hProv; 
};
