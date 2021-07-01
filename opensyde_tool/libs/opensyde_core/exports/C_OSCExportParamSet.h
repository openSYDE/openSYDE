//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Export parameter set image for an openSYDE node.

   See cpp file for detailed description

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCEXPORTPARAMSET_H
#define C_OSCEXPORTPARAMSET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "CSCLString.h"
#include "C_OSCNode.h"
#include "C_OSCParamSetHandler.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCExportParamSet
{
public:
   static stw_scl::C_SCLString h_GetFileName(const C_OSCNodeApplication & orc_DataBlock, const bool oq_IsSafe);
   static stw_types::sint32 h_CreateParameterSetImage(const stw_scl::C_SCLString & orc_Path, const C_OSCNode & orc_Node,
                                                      const stw_types::uint16 ou16_ApplicationIndex,
                                                      std::vector<stw_scl::C_SCLString> & orc_Files,
                                                      const stw_scl::C_SCLString & orc_ExportToolName = "",
                                                      const stw_scl::C_SCLString & orc_ExportToolVersion = "");

private:
   static stw_types::sint32 mh_FillPsiStructure(const C_OSCNode & orc_Node, const bool oq_IsSafe,
                                                const stw_types::uint16 ou16_ApplicationIndex,
                                                C_OSCParamSetRawNode & orc_RawNode,
                                                C_OSCParamSetInterpretedNode & orc_IntNode);
   static void mh_FillInterpretedDatapool(const stw_opensyde_core::C_OSCNodeDataPool & orc_SdDataPool,
                                          stw_opensyde_core::C_OSCParamSetInterpretedDataPool & orc_IntDataPool);
   static stw_types::sint32 mh_FillRawEntries(const stw_opensyde_core::C_OSCNodeDataPool & orc_SdDataPool,
                                              const std::vector<stw_types::uint8> & orc_ConfigRawBytes,
                                              std::vector<C_OSCParamSetRawEntry> & orc_Entries);
   static C_OSCParamSetInterpretedFileInfoData mh_GetFileInfo(const stw_scl::C_SCLString & orc_ExportToolName,
                                                              const stw_scl::C_SCLString & orc_ExportToolVersion);
   static stw_types::sint32 mh_WriteParameterSetImage(const C_OSCParamSetRawNode & orc_RawNode,
                                                      const C_OSCParamSetInterpretedNode & orc_IntNode,
                                                      const bool oq_IsSafe, const C_OSCNodeApplication & orc_DataBlock,
                                                      const stw_scl::C_SCLString & orc_Path,
                                                      std::vector<stw_scl::C_SCLString> & orc_Files,
                                                      const stw_scl::C_SCLString & orc_ExportToolName,
                                                      const stw_scl::C_SCLString & orc_ExportToolVersion);

   static stw_types::sint32 mh_InsertCRC16(std::vector<stw_types::uint8> & orc_Bytes);
   static stw_types::sint32 mh_GetConfigurationRawBytes(const stw_opensyde_core::C_OSCNodeDataPool & orc_SdDataPool,
                                                        std::vector<stw_types::uint8> & orc_Bytes);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
