// provided by ZeitControl
#ifndef _CRC_H_
#define _CRC_H_

typedef unsigned short US;
typedef unsigned long UL;
typedef unsigned char UC;

US CRC16 (const void* data, US len) ;
void UpdateCRC16 (US& crc, const void* data, US len) ;
UL CRC32 (const void* data, US len) ;
void UpdateCRC32 (UL& crc, const void* data, US len) ;

#endif // _CRC_H_
