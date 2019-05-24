//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Export data pool settings of a openSYDE node.

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCEXPORTDATAPOOLH
#define C_OSCEXPORTDATAPOOLH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>

#include "stwtypes.h"

#include "CSCLStringList.h"
#include "C_OSCNodeDataPool.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCExportDataPool
{
public:
   C_OSCExportDataPool(void);
   virtual ~C_OSCExportDataPool(void);

   static void h_GetFileName(const C_OSCNodeDataPool & orc_DataPool, stw_scl::C_SCLString & orc_FileName);
   static stw_types::sint32 h_CreateSourceCode(const stw_scl::C_SCLString & orc_Path,
                                               const C_OSCNodeDataPool & orc_DataPool,
                                               const stw_types::uint8 ou8_DataPoolIndex, const bool oq_IsRemote,
                                               const stw_types::uint8 ou8_DataPoolIndexRemote,
                                               const stw_types::uint8 ou8_ProcessId);

private:
   static const bool mhq_IS_HEADER_FILE = false;
   static const bool mhq_IS_IMPLEMENTATION_FILE = true;

   static stw_types::sint32 mh_CreateImplementationFile(const stw_scl::C_SCLString & orc_Path,
                                                        const C_OSCNodeDataPool & orc_DataPool,
                                                        const stw_types::uint8 ou8_DataPoolIndex,
                                                        const stw_scl::C_SCLString & orc_ProjectId,
                                                        const stw_types::uint8 ou8_DataPoolIndexRemote,
                                                        const stw_types::uint8 ou8_ProcessId, const bool oq_IsRemote);
   static stw_types::sint32 mh_CreateHeaderFile(const stw_scl::C_SCLString & orc_Path,
                                                const C_OSCNodeDataPool & orc_DataPool,
                                                const stw_types::uint8 ou8_DataPoolIndex,
                                                const stw_scl::C_SCLString & orc_ProjectId, const bool oq_IsRemote);

   static void mh_AddHeader(stw_scl::C_SCLStringList & orc_Data, const C_OSCNodeDataPool & orc_DataPool,
                            const bool oq_FileType);
   static void mh_AddIncludes(stw_scl::C_SCLStringList & orc_Data, const C_OSCNodeDataPool & orc_DataPool,
                              const bool oq_FileType);
   static void mh_AddDefines(stw_scl::C_SCLStringList & orc_Data, const C_OSCNodeDataPool & orc_DataPool,
                             const stw_types::uint8 ou8_DataPoolIndex, const stw_scl::C_SCLString & orc_ProjectId,
                             const bool oq_FileType, const bool oq_IsRemote);
   static void mh_AddTypes(stw_scl::C_SCLStringList & orc_Data, const C_OSCNodeDataPool & orc_DataPool,
                           const bool oq_FileType, const bool oq_IsRemote);
   static void mh_AddGlobalVariables(stw_scl::C_SCLStringList & orc_Data, const C_OSCNodeDataPool & orc_DataPool,
                                     const bool oq_FileType, const bool oq_IsRemote);
   static void mh_AddFunctionPrototypes(stw_scl::C_SCLStringList & orc_Data, const C_OSCNodeDataPool & orc_DataPool,
                                        const stw_scl::C_SCLString & orc_ProjectId);
   static void mh_AddModuleGlobal(stw_scl::C_SCLStringList & orc_Data, const C_OSCNodeDataPool & orc_DataPool,
                                  const stw_types::uint8 ou8_ProcessId, const stw_types::uint8 ou8_DataPoolIndexRemote,
                                  const bool oq_IsRemote);
   static void mh_AddImplementation(stw_scl::C_SCLStringList & orc_Data, const bool oq_FileType);
   static stw_types::sint32 mh_SaveToFile(stw_scl::C_SCLStringList & orc_Data, const stw_scl::C_SCLString & orc_Path,
                                          const stw_scl::C_SCLString & orc_DataPoolName, const bool oq_FileType);
   static void mh_CreateFileName(const stw_scl::C_SCLString & orc_DataPoolName, stw_scl::C_SCLString & orc_FileName);
   static stw_scl::C_SCLString mh_GetType(const C_OSCNodeDataPoolContent::E_Type oe_Type);
   static stw_scl::C_SCLString mh_GetTypePrefix(const C_OSCNodeDataPoolContent::E_Type oe_Type, const bool oq_IsArray);
   static stw_scl::C_SCLString mh_GetElementValueString(const C_OSCNodeDataPoolContent & orc_Value,
                                                        const C_OSCNodeDataPoolContent::E_Type oe_Type,
                                                        const bool oq_IsArray);
   static stw_scl::C_SCLString mh_GetElementSize(const C_OSCNodeDataPoolContent::E_Type oe_Type,
                                                 const stw_types::uint32 ou32_ArraySize, const bool oq_IsArray);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
