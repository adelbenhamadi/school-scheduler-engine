#ifndef CFILESTREAM_H
#define CFILESTREAM_H

#include  <windows.h>


typedef LONG fsize_t ;

typedef enum {wrWrite,wrRead,wrWriteRead} wrMode;
typedef enum {cmCreate,cmOpen} cmMode;


class CFileStream
{

public:
    CFileStream(const char* fn,cmMode fc=cmCreate,LONG shareMode=0);
    virtual ~CFileStream();
    bool SaveToFile(const char* fn);
    bool LoadFromFile(const char* fn);
    bool writeString(const char* s);
    char* readString(void);
    bool Save(void);
    bool openFile(const char * fn,wrMode fm=wrRead);
    bool closeFile(void);
    bool setSize(const fsize_t newsize);
    fsize_t getSize(void);
//    fsize_t copyFrom(CStream source,fsize_t co);
    DWORD write(LPCVOID buf,DWORD co);
    DWORD read(LPVOID buf,DWORD co);
    DWORD seek(LONG offset,DWORD origin);
private:
    HANDLE handle;
    char* fileName;
    fsize_t position;


protected:

};

#endif // CFILESTREAM_H
