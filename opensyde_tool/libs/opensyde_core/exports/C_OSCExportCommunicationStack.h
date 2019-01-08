//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Export communication stack settings of a openSYDE node.

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     16.10.2017  STW/U.Roesch
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OSCEXPORTCOMMUNICATIONSTACKH
#define C_OSCEXPORTCOMMUNICATIONSTACKH

/* -- Includes ------------------------------------------------------------- */
#include <vector>

#include "stwtypes.h"

#include "CSCLStringList.h"
#include "C_OSCNode.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OSCExportCommunicationStack
{
public:
   C_OSCExportCommunicationStack(void);
   virtual ~C_OSCExportCommunicationStack(void);

   static void h_GetFileName(const stw_types::uint8 ou8_InterfaceIndex, const C_OSCCanProtocol::E_Type & ore_Protocol,
                             stw_scl::C_SCLString & orc_FileName);
   static stw_types::sint32 h_CreateSourceCode(const stw_scl::C_SCLString & orc_Path, const C_OSCNode & orc_Node,
                                               const stw_types::uint8 ou8_InterfaceIndex,
                                               const C_OSCCanProtocol::E_Type & ore_Protocol);

private:
   static const bool mhq_IS_HEADER_FILE = false;
   static const bool mhq_IS_IMPLEMENTATION_FILE = true;

   static stw_types::sint32 mh_CreateHeaderFile(const stw_scl::C_SCLString & orc_Path,
                                                const C_OSCCanProtocol & orc_ComProtocol,
                                                const stw_types::uint8 ou8_InterfaceIndex,
                                                const C_OSCCanProtocol::E_Type & ore_Protocol,
                                                const stw_scl::C_SCLString & orc_ProjectId);
   static stw_types::sint32 mh_CreateImplementationFile(const stw_scl::C_SCLString & orc_Path,
                                                        const C_OSCCanProtocol & orc_ComProtocol,
                                                        const C_OSCNodeDataPool & orc_DataPool,
                                                        const stw_types::uint8 ou8_InterfaceIndex,
                                                        const C_OSCCanProtocol::E_Type & ore_Protocol,
                                                        const stw_scl::C_SCLString & orc_ProjectId);

   static void mh_AddHeader(stw_scl::C_SCLStringList & orc_Data, const stw_types::uint8 ou8_InterfaceIndex,
                            const C_OSCCanProtocol::E_Type & ore_Protocol, const bool oq_FileType);
   static void mh_AddCIncludes(stw_scl::C_SCLStringList & orc_Data, const stw_scl::C_SCLString & orc_DataPoolName,
                               const stw_types::uint8 ou8_InterfaceIndex, const C_OSCCanProtocol::E_Type & ore_Protocol,
                               const bool oq_NullRequired);
   static void mh_AddDefines(stw_scl::C_SCLStringList & orc_Data, const C_OSCCanMessageContainer & orc_ComMessage,
                             const stw_types::uint8 ou8_InterfaceIndex, const C_OSCCanProtocol::E_Type & ore_Protocol,
                             const stw_scl::C_SCLString & orc_ProjectId, const bool oq_FileType);
   static void mh_AddCModuleGlobal(stw_scl::C_SCLStringList & orc_Data, const bool oq_SafeData,
                                   const C_OSCCanMessageContainer & orc_ComMessage,
                                   const stw_types::uint8 ou8_InterfaceIndex,
                                   const C_OSCCanProtocol::E_Type & ore_Protocol,
                                   const stw_types::uint32 ou32_TxListIndex, const stw_types::uint32 ou32_RxListIndex);
   static void mh_AddCGlobalVariables(stw_scl::C_SCLStringList & orc_Data,
                                      const stw_scl::C_SCLString & orc_DataPoolName,
                                      const stw_types::uint8 ou8_InterfaceIndex,
                                      const C_OSCCanProtocol::E_Type & ore_Protocol, const bool oq_TxMessagesPresent,
                                      const bool oq_RxMessagesPresent);
   static stw_scl::C_SCLString mh_GetProtocolNameByType(const C_OSCCanProtocol::E_Type & ore_Protocol);
   static stw_scl::C_SCLString mh_GetByteOrderNameByType(const C_OSCCanSignal::E_ByteOrderType & ore_ByteOrder);
   static stw_scl::C_SCLString mh_GetTransmissionTriggerNameByType(const C_OSCCanMessage::E_TxMethodType & ore_Trigger);
   static stw_types::sint32 mh_SaveToFile(stw_scl::C_SCLStringList & orc_Data, const stw_scl::C_SCLString & orc_Path,
                                          const stw_types::uint8 ou8_InterfaceIndex,
                                          const C_OSCCanProtocol::E_Type & ore_Protocol, const bool oq_FileType);
   static void mh_AddSignalDefinitions(const stw_types::uint32 ou32_SignalListIndex,
                                       const std::vector<C_OSCCanMessage> & orc_Messages,
                                       stw_scl::C_SCLStringList & orc_Data);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
