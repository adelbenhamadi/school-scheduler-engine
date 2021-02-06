#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "cfilestream.h"

HANDLE openFile_em(const char * fn,wrMode fm,LONG shareMode=0)
{


    DWORD dAccess;
    switch (fm)
    {
    case wrWrite:
        dAccess=GENERIC_WRITE;
        break;
    case wrRead:
        dAccess=GENERIC_READ;
        break;
    case wrWriteRead:
        dAccess=GENERIC_READ | GENERIC_WRITE;
        break;

    }

    HANDLE res=CreateFile(fn,
                          dAccess,
                          shareMode,//0:no sharing!
                          NULL,
                          OPEN_ALWAYS,
                          FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
                          0);
    if(res==INVALID_HANDLE_VALUE)
    {
        printf("INVALID_HANDLE_VALUE openFile error!\n\r");

    }
    return res;
}

HANDLE createFile_em(const char * fn,wrMode fm,LONG shareMode=0)
{


    DWORD dAccess;
    switch (fm)
    {
    case wrWrite:
        dAccess=GENERIC_WRITE;
        break;
    case wrRead:
        dAccess=GENERIC_READ;
        break;
    case wrWriteRead:
        dAccess=GENERIC_READ | GENERIC_WRITE;
        break;

    }

    HANDLE res=CreateFile(fn,
                          dAccess,
                          shareMode,//0:no sharing!
                          NULL,
                          CREATE_ALWAYS,
                          FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
                          0);
    if(res==INVALID_HANDLE_VALUE)
    {
        printf("INVALID_HANDLE_VALUE createFile error!\n\r");

    }
    return res;
}

CFileStream::CFileStream(const char* fn,cmMode fc,LONG shareMode)
{
    if (fn!=NULL)
    {
        wrMode fm=wrWriteRead;
        switch (fc)
        {
        case cmCreate:
            handle=createFile_em(fn,fm,shareMode);
            break;
        case cmOpen  :
            handle=openFile_em(fn,fm,shareMode);
            break;

        }
        if(handle==INVALID_HANDLE_VALUE)
            printf("invalid handle error!");
    }

}

CFileStream::~CFileStream()
{
    if(handle>=0)
        closeFile();
}
DWORD CFileStream::write(LPCVOID buf,DWORD co)
{
    DWORD nbytesWrite;
    if((co>0)&&(handle!=INVALID_HANDLE_VALUE))
    {
        if(WriteFile(handle, buf, co, &nbytesWrite, NULL))
            return nbytesWrite;
        else
        {
            return (DWORD)GetLastError();
        }

    }
    return -1;

}

DWORD CFileStream::read(LPVOID buf,DWORD co)
{
    DWORD nbytesRead;
    if((co>0)&&(handle!=INVALID_HANDLE_VALUE))
    {
        if(ReadFile(handle, buf, co, &nbytesRead, NULL))
        {
            // Check for end of file.
            if (nbytesRead == 0)
            {
                // this is the end of the file
            }
            return nbytesRead;
        }
        else
        {
            return (DWORD)GetLastError();
        }


    }
    return 0;
}

DWORD CFileStream::seek(LONG offset,DWORD origin)
{
    DWORD ptr,err;
    /*  ptr=SetFilePointer(handle, offset, NULL, origin);
     if(( ptr==INVALID_SET_FILE_POINTER) &&
         (err=GetLastError() != NO_ERROR)){
          printf("seek error!");
         }
      */
    return ptr;

}
bool CFileStream::setSize(const LONG newsize)
{
    /*   if(seek(newsize,FILE_BEGIN)!=INVALID_SET_FILE_POINTER)
            return(SetEndOfFile(handle) && (GetLastError() == NO_ERROR));
     */
    return false;

}

bool CFileStream::writeString(const char* s)
{
    DWORD sz=strlen(s) ;
    return ((write(&sz,sizeof(sz))==sizeof(sz))&&(write(s,sz)==sz));
}


char* CFileStream::readString(void)
{
    DWORD sz;
    char* s=NULL;
    if(read(&sz,sizeof(sz))==sizeof(sz))
    {
        s = (char*)(malloc (sizeof (*s) * (sz + 1)));
        if (read(s,sz)==sz)
        {
            // s[sz+1]="\0";
            return s;
        }
    }
    return NULL;
}

bool CFileStream::LoadFromFile(const char* fn)
{
    handle=openFile_em(fn,wrRead);
    *fileName=*fn;
    return (handle!=INVALID_HANDLE_VALUE);
}

bool CFileStream::SaveToFile(const char* fn)
{
    handle=openFile_em(fn,wrWriteRead);
    *fileName=*fn;
    return (handle!=INVALID_HANDLE_VALUE);

}
bool CFileStream::Save(void)
{
    if((fileName!=NULL)&&(strlen(fileName)>0))
        return (bool)SaveToFile(fileName);
    return false;
}
bool    CFileStream::closeFile(void)
{
    if(handle!=INVALID_HANDLE_VALUE)
        return (CloseHandle(handle)!=0);
    return true;

}


