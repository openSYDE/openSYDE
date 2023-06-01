#ifndef CKFXVARIABLELISTSHPP
#define CKFXVARIABLELISTSHPP

#include "stwtypes.hpp"
#include "CKFXVariableListBase.hpp"
#include "DiagLib_config.hpp"

//---------------------------------------------------------------------------

namespace stw
{
namespace diag_lib
{
//---------------------------------------------------------------------------

class C_KFXVariableLists :
   public stw::scl::C_SclDynamicArray<C_KFXVariableListBase>
{
private:
public:
   //over whole structure, not just the values:
   void CalcCRCOverAllLists(uint16_t & oru16_CRC, const bool oq_SkipValues = false) const;

   void ClearDefaults(void);
   void ClearValues(void);

   bool VariableExists(const uint16_t ou16_Index) const;
   bool VariableExists(const uint16_t ou16_List, const uint16_t ou16_Variable) const;
   bool ListExists(const uint16_t ou16_List) const;

   //Note: put the const functions first here as a workaround for problems in PC-Lint 9.00k in combination with -ctc
   const C_KFXVariableBase * GetVariable(const uint16_t ou16_Index) const;
   const C_KFXVariableBase * GetVariable(const uint16_t ou16_List, const uint16_t ou16_Variable) const;
   //next two not const as the result can be used to change our data ...
   //... and also because creating temporary copies is pretty slow.
   C_KFXVariableBase * GetVariable(const uint16_t ou16_Index);
   C_KFXVariableBase * GetVariable(const uint16_t ou16_List, const uint16_t ou16_Variable);

   static uint16_t PackIndex(const uint16_t ou16_List, const uint16_t ou16_Variable);
   static void UnpackIndex(const uint16_t ou16_Index, uint16_t * const opu16_List, uint16_t * const opu16_Variable);

   const C_KFXVariableListBase * GetListByName(const stw::scl::C_SclString & orc_ListName) const;
   C_KFXVariableListBase * GetListByName(const stw::scl::C_SclString & orc_ListName);

   uint16_t u16_CRCOverCRCsServer;                                     ///< last EEPROM value CRC over CRCs read from
                                                                       // ECU
   stw::scl::C_SclDynamicArray<stw::scl::C_SclString> ac_DefaultNames; //names of default sets
};

//---------------------------------------------------------------------------
}
}
#endif
