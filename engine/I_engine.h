#ifndef I_ENGINE_INCLUDED
#define I_ENGINE_INCLUDED

#include <windef.h>
#include "classes.h"
#include "solution.h"


struct I_Engine_Intf
{

  //virtual CemSolution solution()=0;
  virtual bool  Load(CEngineConfig *ecfg)=0;
  virtual bool  Release()=0;
  virtual CEnginePlugin* getPluginInfo()=0;
  virtual void execute(bool _first ,bool _new)=0;
  virtual bool  verifySolution(bool b)=0;
  virtual bool  getOptimizeValue(int *hp,int*hc,int*cp,int*cc)=0;
  virtual bool  Save(const char* fn)=0;
  virtual int SeancesCount()=0;
};

/*
Declarations for the Dlls exported functions and typedef'ed function
pointers to make it easier to load them. Note the extern "C" which
tell the compiler to use C-style linkage for these functions
*/

extern "C"
{
HRESULT  GetEngineIntf(I_Engine_Intf ** pInterface);
typedef HRESULT (*GETINTERFACE)(I_Engine_Intf  ** pInterface);

HRESULT  FreeEngineIntf(I_Engine_Intf ** pInterface);
typedef HRESULT (*FREEINTERFACE)(I_Engine_Intf ** pInterface);


}



#endif // I_ENGINE_H_INCLUDED
