//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Encapsulates safety relevant sequences for NVM access. (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCDATADEALERNVMSAFE_H
#define C_OSCDATADEALERNVMSAFE_H

/* -- Includes ------------------------------------------------------------- */
#include <set>

#include "C_OSCDataDealerNvm.h"
#include "C_OSCParamSetRawNode.h"
#include "C_OSCParamSetRawEntry.h"
#include "C_OSCParamSetInterpretedList.h"
#include "C_OSCParamSetInterpretedFileInfoData.h"
#include "C_OSCNodeDataPoolListId.h"
#include "C_OSCNodeDataPoolListElementId.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

///openSYDE safe NVM data dealer
class C_OSCDataDealerNvmSafe :
   public C_OSCDataDealerNvm
{
public:
   C_OSCDataDealerNvmSafe(void);
   C_OSCDataDealerNvmSafe(C_OSCNode * const opc_Node, const stw_types::uint32 ou32_NodeIndex,
                          C_OSCDiagProtocolBase * const opc_DiagProtocol);
   virtual ~C_OSCDataDealerNvmSafe(void);

   stw_types::sint32 NvmSafeCheckCrcs(const C_OSCNode & orc_Node) const;
   stw_types::sint32 NvmSafeWriteChangedValues(std::vector<C_OSCNodeDataPoolListElementId> & orc_ChangedElements, const std::vector<C_OSCNodeDataPoolListId> * const opc_AdditionalListsToUpdate =
                                                  NULL,
                                               stw_types::uint8 * const opu8_NrCode = NULL);
   stw_types::sint32 NvmSafeReadValues(const C_OSCNode * & orpc_NodeCopy, stw_types::uint8 * const opu8_NrCode);
   stw_types::sint32 NvmSafeWriteCrcs(stw_types::uint8 * const opu8_NrCode);

   //Create file process
   static void h_NvmSafeClearInternalContent(void);
   stw_types::sint32 NvmSafeReadParameterValues(const std::vector<C_OSCNodeDataPoolListId> & orc_ListIds,
                                                stw_types::uint8 * const opu8_NrCode);
   static stw_types::sint32 h_NvmSafeCreateCleanFileWithoutCRC(const stw_scl::C_SCLString & orc_Path, const stw_opensyde_core::C_OSCParamSetInterpretedFileInfoData & orc_FileInfo =
                                                                  stw_opensyde_core::C_OSCParamSetInterpretedFileInfoData());
   static stw_types::sint32 h_NvmSafeReadFileWithoutCRC(const stw_scl::C_SCLString & orc_Path);
   stw_types::sint32 NvmSafeCheckParameterFileContents(const stw_scl::C_SCLString & orc_Path,
                                                       std::vector<C_OSCNodeDataPoolListId> & orc_DataPoolLists);
   static stw_types::sint32 h_NvmSafeUpdateCRCForFile(const stw_scl::C_SCLString & orc_Path);

   //Write file process
   static stw_types::sint32 h_NvmSafeReadFileWithCRC(const stw_scl::C_SCLString & orc_Path);
   stw_types::sint32 NvmSafeWriteParameterSetFile(const stw_scl::C_SCLString & orc_Path,
                                                  stw_types::sint32 & ors32_ResultDetail);

private:
   static void mh_CreateInterpretedList(const C_OSCNodeDataPoolList & orc_List,
                                        C_OSCParamSetInterpretedList & orc_InterpretedList);
   static void mh_AppendBytes(const std::vector<stw_types::uint8> & orc_Source,
                              std::vector<stw_types::uint8> & orc_Target);
   stw_types::sint32 m_CheckParameterFileContent(const C_OSCParamSetRawNode & orc_Node);
   stw_types::sint32 m_CreateRawEntryAndPrepareInterpretedData(C_OSCNodeDataPoolList & orc_List,
                                                               C_OSCParamSetRawEntry & orc_Entry,
                                                               stw_types::uint8 * const opu8_NrCode);

   enum E_CreateParameterSetFileState
   {
      eCPSFS_IDLE,
      eCPSFS_FILE_CREATED,
      eCPSFS_FILE_CHECKED
   };

   enum E_ParameterSetFileState
   {
      ePSFS_IDLE,
      ePSFS_DATA_RESET,
      ePSFS_FILE_CREATED,
      ePSFS_FILE_READ_WITHOUT_CRC,
      ePSFS_FILE_READ_WITH_CRC
   };

   E_CreateParameterSetFileState me_CreateParameterSetWorkflowState;
   static E_ParameterSetFileState mhe_ParameterSetFileState;
   static stw_scl::C_SCLString mhc_ParameterSetFilePath;
   C_OSCNode mc_NodeCopy;
   std::set<C_OSCNodeDataPoolListId> mc_ChangedLists;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
