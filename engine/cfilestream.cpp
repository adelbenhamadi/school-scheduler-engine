#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "cfilestream.h"

HANDLE openFile_em(const char * fn,wrMode fm,LONG shareMode=0){


    DWORD dAccess = GENERIC_ALL;
    switch (fm){
        case wrWrite:       dAccess=GENERIC_WRITE;break;
        case wrRead:        dAccess=GENERIC_READ;break;
        case wrWriteRead:   dAccess=GENERIC_READ | GENERIC_WRITE;break;

    }

     HANDLE res=CreateFileA(fn,
                       dAccess,
                       shareMode,//0:no sharing!
                       NULL,
                       OPEN_ALWAYS,
                       FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
                       0);
    if(res==INVALID_HANDLE_VALUE){
    printf("INVALID_HANDLE_VALUE openFile error!\n\r");

    }
    return res;
}

HANDLE createFile_em(const char * fn,wrMode fm,LONG shareMode=0){


    DWORD dAccess = GENERIC_ALL;
    switch (fm){
        case wrWrite:       dAccess=GENERIC_WRITE;break;
        case wrRead:        dAccess=GENERIC_READ;break;
        case wrWriteRead:   dAccess=GENERIC_READ | GENERIC_WRITE;break;

    }

     HANDLE res=CreateFileA(fn,
                       dAccess,
                       shareMode,//0:no sharing!
                       NULL,
                       CREATE_ALWAYS,
                       FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
                       0);
    if(res==INVALID_HANDLE_VALUE){
    printf("INVALID_HANDLE_VALUE createFile error!\n\r");

    }
    return res;
}

CFileStream::CFileStream(const char* fn,cmMode fc,LONG shareMode)
    : _fileName(fn),_handle(0),_position(0)
{
    if (fn!=NULL){
    wrMode fm=wrWriteRead;
    switch (fc){
        case cmCreate:
            _handle=createFile_em(fn,fm,shareMode);
            break;
        case cmOpen  :
            _handle=openFile_em(fn,fm,shareMode);
            break;

    }
    if(_handle==INVALID_HANDLE_VALUE)
        printf("invalid handle error!");
    }

}

CFileStream::~CFileStream()
{
   if(_handle>=0)
    closeFile();
}
DWORD CFileStream::write(LPCVOID buf,DWORD co){
    DWORD writtenBytes;
    if((co>0)&&(_handle!=INVALID_HANDLE_VALUE)){
     if(WriteFile(_handle, buf, co, &writtenBytes, NULL))
        return writtenBytes;
       else{
           return (DWORD)GetLastError();
        }

    }
    return -1;

}

DWORD CFileStream::read(LPVOID buf,DWORD co){
    DWORD readBytes;
    if((co>0)&&(_handle!=INVALID_HANDLE_VALUE)){
    if(ReadFile(_handle, buf, co, &readBytes, NULL)){
        // Check for end of file.
        if (readBytes == 0)
        {
            // this is the end of the file
        }
        return readBytes;
    }
    else{
        return (DWORD)GetLastError();
    }


    }
  return 0;
}

DWORD CFileStream::seek(LONG offset,DWORD origin){
    DWORD ptr,err;
   ptr=SetFilePointer(_handle, offset, NULL, origin);
   if(( ptr==INVALID_SET_FILE_POINTER) &&
       (err=GetLastError() != NO_ERROR)){
        printf("seek error!");
       }
   
    return ptr;

}
bool CFileStream::setSize(const LONG newsize){
/*   if(seek(newsize,FILE_BEGIN)!=INVALID_SET_FILE_POINTER)
        return(SetEndOfFile(handle) && (GetLastError() == NO_ERROR));
 */
   return false;

}

bool CFileStream::writeString(const char* s){
    DWORD sz=strlen(s) ;
    return ((write(&sz,sizeof(sz))==sizeof(sz))&&(write(s,sz)==sz));
}


char* CFileStream::readString(void){
    DWORD sz;
    char* s=NULL;
    if(read(&sz,sizeof(sz))==sizeof(sz)){
        s = (char*)(malloc (sizeof (*s) * (sz + 1)));
        if (read(s,sz)==sz){
       // s[sz+1]="\0";
        return s;
        }
       }
    return NULL;
}

bool CFileStream::load(const char* fn){
     _handle=openFile_em(fn,wrRead);
    _fileName = fn;
    return (_handle!=INVALID_HANDLE_VALUE);
}

bool CFileStream::save(const char* fn){
     _handle=openFile_em(fn,wrWriteRead);
    _fileName = fn;
    return (_handle!=INVALID_HANDLE_VALUE);

}
bool CFileStream::Save(void){
    if((_fileName!=NULL)&&(strlen(_fileName)>0))
       return (bool)save(_fileName);
       return false;
}
bool    CFileStream::closeFile(void){
    if(_handle!=INVALID_HANDLE_VALUE)
        return (CloseHandle(_handle)!=0);
    return true;

}


