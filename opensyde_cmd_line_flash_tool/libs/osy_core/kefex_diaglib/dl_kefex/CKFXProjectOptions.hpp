//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Container classes for holding KEFEX project options

   \copyright   Copyright 2014 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef CKFXPROJECTOPTIONSHPP
#define CKFXPROJECTOPTIONSHPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_SclStringList.hpp"
#include "C_SclIniFile.hpp"
#include "DiagLib_config.hpp"
#include "CKFXVariableBase.hpp" //for KFX_DATA_MAX_NUM_LANGUAGES

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace diag_lib
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_KFXProjectMetaInfo
{
public:
   C_KFXProjectMetaInfo(void);
   virtual ~C_KFXProjectMetaInfo(void);

   void LoadFromINI(stw::scl::C_SclIniFile & orc_IniFile, const stw::scl::C_SclString & orc_Section);

   //We only use one stringlist, so the user can type anything he/she wants
   //This might greatly differ between each company / type of project so
   // we'll never get finished providing more specialized fields.
   stw::scl::C_SclStringList c_Text;

   C_KFXProjectMetaInfo(const C_KFXProjectMetaInfo & orc_Source);
   C_KFXProjectMetaInfo & operator = (const C_KFXProjectMetaInfo & orc_Source);
   bool operator !=(C_KFXProjectMetaInfo & orc_Source);
};

//----------------------------------------------------------------------------------------------------------------------

class C_KFXProjectOptions
{
private:
public:
   uint16_t u16_MaxDesignatorLength; ///< for editing only -> not put into KSP
   uint16_t u16_ProjectIndex;
   uint16_t u16_ListOffset;
   uint16_t u16_DataVersion;
   stw::scl::C_SclString c_DeviceName;
   stw::scl::C_SclString ac_CommentDescriptions[KFX_DATA_MAX_NUM_LANGUAGES];
   C_KFXProjectMetaInfo c_MetaInfo;
   bool aq_UserMayWriteKDX[KFX_NUM_ACCESS_GROUPS];

   C_KFXProjectOptions(void);

   bool operator !=(C_KFXProjectOptions & orc_Source);

   void LoadConfigFromIni(stw::scl::C_SclIniFile & orc_IniFile);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
