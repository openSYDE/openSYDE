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

#include "stwtypes.h"
#include "CSCLString.h"
#include "C_OSCHalcDefDomain.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCHalcDefBase
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

   C_OSCHalcDefBase(void);
   virtual ~C_OSCHalcDefBase(void);

   bool CheckIdsUnique(std::vector<stw_scl::C_SCLString> & orc_DuplicateIds) const;

   stw_types::uint32 u32_ContentVersion;
   stw_scl::C_SCLString c_DeviceName;
   stw_scl::C_SCLString c_FileString;                        ///< Content of corresponding HALC definition as string
   stw_scl::C_SCLString c_OriginalFileName;                  ///< Original file name of corresponding HALC definition
   E_SafetyMode e_SafetyMode;                                ///< Safety mode
   stw_types::uint8 u8_NumConfigCopies;                      ///< Number of copies (starting at one)
   bool q_NvMBasedConfig;                                    ///< Flag if NVM based
   std::vector<stw_types::uint32> c_NvMSafeAddressOffset;    ///< NVM start addresses for safe datapools
   std::vector<stw_types::uint32> c_NvMNonSafeAddressOffset; ///< NVM start address for non-safe datapools
   stw_types::uint32 u32_NvMReservedListSizeParameters;      ///< Reserved NVM parameters list size
   stw_types::uint32 u32_NvMReservedListSizeInputValues;     ///< Reserved NVM input values list size
   stw_types::uint32 u32_NvMReservedListSizeOutputValues;    ///< Reserved NVM output values list size
   stw_types::uint32 u32_NvMReservedListSizeStatusValues;    ///< Reserved NVM status values list size

   virtual void Clear(void);
   virtual bool IsClear(void) const;

   virtual stw_types::uint32 GetDomainSize(void) const = 0;
   virtual const C_OSCHalcDefDomain * GetDomainDefDataConst(const stw_types::uint32 ou32_Index) const = 0;
   virtual void AddDomain(const C_OSCHalcDefDomain & orc_Domain) = 0;
   virtual void HandleFileLoadPostProcessing(void) = 0;

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const;

private:
   static void mh_AggregateIds(const std::vector<C_OSCHalcDefStruct> & orc_Items,
                               std::vector<stw_scl::C_SCLString> & orc_DuplicateIds);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
