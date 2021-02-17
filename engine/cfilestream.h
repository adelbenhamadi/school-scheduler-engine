#ifndef CFILESTREAM_H
#define CFILESTREAM_H

#include  <windows.h>


typedef LONG fsize_t ;

enum wrMode{wrWrite,wrRead,wrWriteRead} ;
enum cmMode{cmCreate,cmOpen} ;


class CFileStream
{

public:
    CFileStream(const char* fn,cmMode fc=cmCreate,LONG shareMode=0);
    virtual ~CFileStream();
    bool save(const char* fn);
    bool load(const char* fn);
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
    HANDLE _handle;
    const char* _fileName;
    fsize_t _position;


protected:

};

#endif // CFILESTREAM_H
