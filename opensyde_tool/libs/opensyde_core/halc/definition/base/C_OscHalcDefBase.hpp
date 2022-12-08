//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       HALC definition data base
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCDEFBASE_H
#define C_OSCHALCDEFBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>

#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_OscHalcDefDomain.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscHalcDefBase
{
public:
   enum E_SafetyMode ///< Type of safety mode
   {
      eTWO_LEVELS_WITH_DROPPING,    ///< safe + non-safe Datapool + Config; channels of other safetyness get dropped
      eTWO_LEVELS_WITHOUT_DROPPING, ///< safe + non-safe Datapool + Config; channels of other safetyness do not get
                                    // dropped; each channel gets an implicit safetyness parameters
      eONE_LEVEL_ALL_SAFE,          ///< safe Datapool + config (no dropping)
      eONE_LEVEL_ALL_NON_SAFE       ///< non-safe Datapool + config (no dropping)
   };

   C_OscHalcDefBase(void);
   virtual ~C_OscHalcDefBase(void);

   bool CheckIdsUnique(std::vector<stw::scl::C_SclString> & orc_DuplicateIds) const;

   uint32_t u32_ContentVersion;
   stw::scl::C_SclString c_DeviceName;
   stw::scl::C_SclString c_FileString;              ///< Content of corresponding HALC definition as string
   stw::scl::C_SclString c_OriginalFileName;        ///< Original file name of corresponding HALC definition
   E_SafetyMode e_SafetyMode;                       ///< Safety mode
   uint8_t u8_NumConfigCopies;                      ///< Number of copies (starting at one)
   bool q_NvmBasedConfig;                           ///< Flag if NVM based
   std::vector<uint32_t> c_NvmSafeAddressOffset;    ///< NVM start addresses for safe datapools
   std::vector<uint32_t> c_NvmNonSafeAddressOffset; ///< NVM start address for non-safe datapools
   uint32_t u32_NvmReservedListSizeParameters;      ///< Reserved NVM parameters list size
   uint32_t u32_NvmReservedListSizeInputValues;     ///< Reserved NVM input values list size
   uint32_t u32_NvmReservedListSizeOutputValues;    ///< Reserved NVM output values list size
   uint32_t u32_NvmReservedListSizeStatusValues;    ///< Reserved NVM status values list size

   virtual void Clear(void);
   virtual bool IsClear(void) const;

   virtual uint32_t GetDomainSize(void) const = 0;
   virtual const C_OscHalcDefDomain * GetDomainDefDataConst(const uint32_t ou32_Index) const = 0;
   virtual void AddDomain(const C_OscHalcDefDomain & orc_Domain) = 0;
   virtual void HandleFileLoadPostProcessing(void) = 0;

   virtual void CalcHash(uint32_t & oru32_HashValue) const;

private:
   static void mh_AggregateIds(const std::vector<C_OscHalcDefStruct> & orc_Items,
                               std::vector<stw::scl::C_SclString> & orc_DuplicateIds);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
