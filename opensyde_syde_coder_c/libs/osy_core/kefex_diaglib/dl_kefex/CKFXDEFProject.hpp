//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       KEFEX DEF Project handling

   Implements reading DEF-style KEFEX projects.

   \copyright   Copyright 2009 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef CKFXDEFPROJECTHPP
#define CKFXDEFPROJECTHPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "CKFXVariableLists.hpp"
#include "C_SclString.hpp"
#include "C_OscChecksummedIniFile.hpp"
#include "DiagLib_config.hpp"

namespace stw
{
namespace diag_lib
{
/* -- Defines ------------------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_KFXRAMNameIndex
{
public:
   stw::scl::C_SclString c_FileName;
   uint16_t u16_ListIndex;
   C_KFXRAMNameIndex & operator =(const C_KFXRAMNameIndex & orc_Source);
   C_KFXRAMNameIndex();
   C_KFXRAMNameIndex(const C_KFXRAMNameIndex & orc_Source);
};

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Function Prototypes ------------------------------------------------------------------------------------------- */

///KEFEX DEF Project handling
class C_KFXDEFProject
{
private:
   static int32_t m_LoadRAMListFromFile(const stw::scl::C_SclString & orc_FilePath, C_KFXVariableListBase & orc_List,
                                        stw::scl::C_SclString & orc_ErrorText);
   static int32_t m_SortRAMLists(stw::scl::C_SclDynamicArray<C_KFXRAMNameIndex> & orc_Files,
                                 stw::scl::C_SclString & orc_ErrorText);
   static int32_t m_FindRelatedFiles(const stw::scl::C_SclString & orc_Directory,
                                     const stw::scl::C_SclString & orc_DeviceName,
                                     stw::scl::C_SclDynamicArray<C_KFXRAMNameIndex> & orc_Files,
                                     stw::scl::C_SclDynamicArray<stw::scl::C_SclString> & orc_Warnings);

   static void m_VarStringsToMinMax(const stw::scl::C_SclString & orc_Min, const stw::scl::C_SclString & orc_Max,
                                    C_KFXVariableBase & orc_Variable);

public:
   static E_TransmissionType TransTypeStringToEnum(const stw::scl::C_SclString & orc_Text);
   static int32_t LoadRAMFiles(const stw::scl::C_SclString & orc_Directory,
                               const stw::scl::C_SclString & orc_DeviceName, C_KFXVariableLists & orc_Lists,
                               stw::scl::C_SclString & orc_ErrorText,
                               stw::scl::C_SclDynamicArray<stw::scl::C_SclString> & orc_Warnings);

   static int32_t LoadComments(const stw::scl::C_SclString & orc_FileName, const stw::scl::C_SclString & orc_DeviceName,
                               C_KFXVariableLists & orc_VariableLists,
                               stw::scl::C_SclString * const opc_CommentDescriptions);
   static void LoadDefaultNames(stw::opensyde_core::C_OscChecksummedIniFile * const opc_IniFile,
                                C_KFXVariableLists & orc_VariableLists);
};
}
}

/* -- Implementation ------------------------------------------------------------------------------------------------ */

#endif
