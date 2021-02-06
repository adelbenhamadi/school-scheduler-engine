#include <windows.h>
#include "I_engine.h"
#include "engine13/engine.h"

#ifndef __MAIN_H__
#define __MAIN_H__




#ifdef EMPLOIDLL_EXPORTS
#define EMPLOILIB_API __declspec(dllexport)
#else
#define EMPLOILIB_API __declspec(dllimport)
#endif

HRESULT GetEngineIntf(I_Engine_Intf ** pInterface)
{
   if(!*pInterface)
   {
        *pInterface= new emEngine;

        return S_OK;
   }
   return E_FAIL;
}

HRESULT FreeEngineIntf(I_Engine_Intf ** pInterface)
{
   if(!*pInterface)
        return E_FAIL;
   delete *pInterface;
   *pInterface= 0;
   return S_OK;
}


#endif // __MAIN_H__
