#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <cstring>
#include <ctime>
#include <windows.h>
#include <stdint.h>

//cesar cryptage method
void cesar (char *str, int decalage);

void time_t2FileTime( time_t t, LPFILETIME pft );
void time_t2SystemTime( time_t t, LPSYSTEMTIME sft );

uint16_t _fletcher16( uint8_t* data, int count );

const int MOD_ADLER = 65521;
/* data is the location of the data in physical memory and
len =length of data in bytes */
uint64_t _adler32(unsigned char *data, int32_t len);


#endif
