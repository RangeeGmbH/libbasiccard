// provided by ZeitControl
#include "crc.h"

US CRC16 (const void* data, US len)
  {
  US crc = 0 ;
  UpdateCRC16 (crc, data, len) ;
  return crc ;
  }

void UpdateCRC16 (US& crc, const void* data, US len)
  {
  const UC* p = (const UC*)data ;
  while (len--)
    {
    UC NextByte = *p++ ;
    for (int i = 0 ; i < 8 ; i++, NextByte >>= 1)
      {
      if ((crc ^ NextByte) & 1)
        {
        crc >>= 1 ;
        crc ^= 0xCA00 ;
        }
      else crc >>= 1 ;
      }
    }
  }

UL CRC32 (const void* data, US len)
  {
  UL crc = 0 ;
  UpdateCRC32 (crc, data, len) ;
  return crc ;
  }

void UpdateCRC32 (UL& crc, const void* data, US len)
  {
  const UC* p = (const UC*)data ;
  while (len--)
    {
    UC NextByte = *p++ ;
    for (int i = 0 ; i < 8 ; i++, NextByte >>= 1)
      {
      if ((crc ^ NextByte) & 1)
        {
        crc >>= 1 ;
        crc ^= 0xA3000000 ;
        }
      else crc >>= 1 ;
      }
    }
  }
