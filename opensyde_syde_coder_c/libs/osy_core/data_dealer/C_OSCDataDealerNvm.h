//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Encapsulates non-trivial sequences for NVM access. (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     13.10.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OSCDATADEALERNVM_H
#define C_OSCDATADEALERNVM_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OSCDataDealer.h"
#include "C_OSCNodeDataPoolList.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

///openSYDE NVM data dealer
class C_OSCDataDealerNvm :
   public C_OSCDataDealer
{
public:
   C_OSCDataDealerNvm(void);
   C_OSCDataDealerNvm(C_OSCNode * const opc_Node, const stw_types::uint32 ou32_NodeIndex,
                      C_OSCDiagProtocolBase * const opc_DiagProtocol);
   virtual ~C_OSCDataDealerNvm(void);

   stw_types::uint16 NvmCalcCrc(const C_OSCNodeDataPoolList & orc_List) const;
   virtual stw_types::sint32 NvmReadList(const stw_types::uint32 ou32_DataPoolIndex,
                                         const stw_types::uint32 ou32_ListIndex, stw_types::uint8 * const opu8_NrCode);
   stw_types::sint32 NvmNotifyOfChanges(const stw_types::uint8 ou8_DataPoolIndex, const stw_types::uint8 ou8_ListIndex,
                                        bool & orq_ApplicationAcknowledge, stw_types::uint8 * const opu8_NrCode);

protected:
   stw_types::sint32 m_NvmReadListRaw(const C_OSCNodeDataPoolList & orc_List,
                                      std::vector<stw_types::uint8> & orc_Values, stw_types::uint8 * const opu8_NrCode);
   stw_types::sint32 m_SaveDumpToList(std::vector<stw_types::uint8> & orc_Values,
                                      C_OSCNodeDataPoolList & orc_List) const;
   stw_types::sint32 m_SaveDumpValuesToListValues(std::vector<stw_types::uint8> & orc_Values,
                                                  C_OSCNodeDataPoolList & orc_List) const;
   stw_types::sint32 m_AdaptProtocolReturnValue(const stw_types::sint32 os32_ProtReturnValue) const;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
