#include "utils.h"
#include <ctype.h>
//cesar cryptage method
void cesar (char *str, int decalage)
{
  if (str)
  {
    const char alphabet[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
                             'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
                             'u', 'v', 'w', 'x', 'y', 'z'};
    int maj = 0;

    while (*str)
    {
      int i;

      maj = isupper (*str);
      *str = tolower (*str);
      for (i = 0; i < 26; i++)
      {
        if (alphabet[i] == *str)
        {
          *str = alphabet[(i+decalage)%26];
          if (maj)
          {
            *str = toupper (*str);
          }
          break;
        }
      }
      str++;
    }
  }
}

void time_t2FileTime( time_t t, LPFILETIME pft )
{
    LONGLONG ll = Int32x32To64(t, 10000000) + 116444736000000000;
    pft->dwLowDateTime = (DWORD) ll;
    pft->dwHighDateTime = ll >>32;
}
void time_t2SystemTime( time_t t, LPSYSTEMTIME sft )
{
    FILETIME ft;
    time_t2FileTime(t, &ft );
    FileTimeToSystemTime(&ft,sft);

}

uint16_t _fletcher16( uint8_t* data, int count )
{

    uint16_t sum1 = 0;
    uint16_t sum2 = 0;
    int index;
    for( index = 0; index < count; ++index )
    {
        sum1 = (sum1 + data[index]) % 255;
        sum2 = (sum2 + sum1) % 255;
    }

    return (sum2 << 8) | sum1;
}


uint64_t _adler32(unsigned char *data, int32_t len)
{
    uint64_t a = 1, b = 0;
    int32_t i;

    /* Process each byte of the data in order */
    for (i = 0; i < len; ++i)
    {
        a = (a + data[i]) % MOD_ADLER;
        b = (b + a) % MOD_ADLER;
    }

    return (b << 16) | a;
}


