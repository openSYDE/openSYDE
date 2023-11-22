#ifndef CKFXVARIABLELISTBASEHPP
#define CKFXVARIABLELISTBASEHPP

#include "stwtypes.hpp"
#include "CKFXVariableBase.hpp"
#include "DiagLib_config.hpp"

//---------------------------------------------------------------------------

namespace stw
{
namespace diag_lib
{
//---------------------------------------------------------------------------
//possible values of "u8_ListType"
const uint8_t KFX_VARIABLE_TYPE_RAM    = 0x01U;
const uint8_t KFX_VARIABLE_TYPE_EEPROM = 0x02U;

//---------------------------------------------------------------------------

///contains all information about one KEFEX variable list
class C_KFXVariableListBase
{
private:
   uint16_t mu16_NumDefaults;

public:
   C_KFXVariableListBase(void);
   C_KFXVariableListBase(const C_KFXVariableListBase & orc_Source);
   virtual ~C_KFXVariableListBase(void);

   void SetNumDefaults(const uint16_t ou16_NumDefaults);
   uint16_t GetNumDefaults(void) const;

   void CalcCRCOverList(uint16_t & oru16_CRC, const bool oq_SkipValues = false) const; ///< uses "classic" STW CRC16

   uint16_t CountChangedValues(void) const;

   C_KFXVariableListBase & operator =(const C_KFXVariableListBase & orc_Source);

   //list definition
   uint8_t u8_ListType; ///< RAM,EEPROM
   bool q_CheckSummed;
   uint32_t u32_CheckSumAddress;
   stw::scl::C_SclString c_ListName;

   uint64_t u64_CRCServer; ///< last EEPROM value CRC read from ECU

   const C_KFXVariableBase * GetVariableByName(const stw::scl::C_SclString & orc_VariableName) const;
   C_KFXVariableBase * GetVariableByName(const stw::scl::C_SclString & orc_VariableName);

   stw::scl::C_SclDynamicArray<C_KFXVariableBase> VariableList;
};

//---------------------------------------------------------------------------
}
}

#endif
