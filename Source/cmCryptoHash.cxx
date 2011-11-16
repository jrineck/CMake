/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2000-2009 Kitware, Inc., Insight Software Consortium

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/
#include "cmCryptoHash.h"

#include <cmsys/MD5.h>

//----------------------------------------------------------------------------
std::string cmCryptoHash::HashString(const char* input)
{
  this->Initialize();
  this->Append(reinterpret_cast<unsigned char const*>(input),
               static_cast<int>(strlen(input)));
  return this->Finalize();
}

//----------------------------------------------------------------------------
std::string cmCryptoHash::HashFile(const char* file)
{
  std::ifstream fin(file, std::ios::in | cmsys_ios_binary);
  if(!fin)
    {
    return "";
    }

  this->Initialize();

  // Should be efficient enough on most system:
  const int bufferSize = 4096;
  char buffer[bufferSize];
  unsigned char const* buffer_uc =
    reinterpret_cast<unsigned char const*>(buffer);
  // This copy loop is very sensitive on certain platforms with
  // slightly broken stream libraries (like HPUX).  Normally, it is
  // incorrect to not check the error condition on the fin.read()
  // before using the data, but the fin.gcount() will be zero if an
  // error occurred.  Therefore, the loop should be safe everywhere.
  while(fin)
    {
    fin.read(buffer, bufferSize);
    if(int gcount = static_cast<int>(fin.gcount()))
      {
      this->Append(buffer_uc, gcount);
      }
    }
  if(fin.eof())
    {
    return this->Finalize();
    }
  return "";
}

//----------------------------------------------------------------------------
cmCryptoHashMD5::cmCryptoHashMD5(): MD5(cmsysMD5_New())
{
}

//----------------------------------------------------------------------------
cmCryptoHashMD5::~cmCryptoHashMD5()
{
  cmsysMD5_Delete(this->MD5);
}

//----------------------------------------------------------------------------
void cmCryptoHashMD5::Initialize()
{
  cmsysMD5_Initialize(this->MD5);
}

//----------------------------------------------------------------------------
void cmCryptoHashMD5::Append(unsigned char const* buf, int sz)
{
  cmsysMD5_Append(this->MD5, buf, sz);
}

//----------------------------------------------------------------------------
std::string cmCryptoHashMD5::Finalize()
{
  char md5out[32];
  cmsysMD5_FinalizeHex(this->MD5, md5out);
  return std::string(md5out, 32);
}
