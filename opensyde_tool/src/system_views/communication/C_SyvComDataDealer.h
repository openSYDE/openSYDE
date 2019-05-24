//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Connects the openSYDE protocol driver to a node with GUI specific functionality

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVCOMDATADEALER_H
#define C_SYVCOMDATADEALER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QMap>
#include <QList>

#include "stwtypes.h"

#include "C_OSCDataDealerNvmSafe.h"

#include "C_OSCNode.h"
#include "C_OSCDiagProtocolBase.h"

#include "C_PuiSvDbDataElementHandler.h"
#include "C_OSCNodeDataPoolListElementId.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvComDataDealer :
   public stw_opensyde_core::C_OSCDataDealerNvmSafe
{
public:
   C_SyvComDataDealer(void);
   C_SyvComDataDealer(stw_opensyde_core::C_OSCNode * const opc_Node, const stw_types::uint32 ou32_NodeIndex,
                      stw_opensyde_core::C_OSCDiagProtocolBase * const opc_DiagProtocol);
   virtual ~C_SyvComDataDealer(void);

   void RegisterWidget(C_PuiSvDbDataElementHandler * const opc_Widget);

   virtual stw_types::sint32 DataPoolRead(const stw_types::uint8 ou8_DataPoolIndex,
                                          const stw_types::uint16 ou16_ListIndex,
                                          const stw_types::uint16 ou16_ElementIndex,
                                          stw_types::uint8 * const opu8_NrCode) override;
   virtual stw_types::sint32 NvmRead(const stw_types::uint8 ou8_DataPoolIndex, const stw_types::uint16 ou16_ListIndex,
                                     const stw_types::uint16 ou16_ElementIndex,
                                     stw_types::uint8 * const opu8_NrCode) override;
   virtual stw_types::sint32 NvmReadList(const stw_types::uint32 ou32_DataPoolIndex,
                                         const stw_types::uint32 ou32_ListIndex,
                                         stw_types::uint8 * const opu8_NrCode) override;

protected:
   virtual void m_OnReadDataPoolEventReceived(const stw_types::uint8 ou8_DataPoolIndex,
                                              const stw_types::uint16 ou16_ListIndex,
                                              const stw_types::uint16 ou16_ElementIndex);
   virtual void m_OnReadDataPoolEventErrorReceived(const stw_types::uint8 ou8_DataPoolIndex,
                                                   const stw_types::uint16 ou16_ListIndex,
                                                   const stw_types::uint16 ou16_ElementIndex,
                                                   const stw_types::uint8 ou8_ErrorCode);
   virtual void m_OnReadDataPoolNvmEventReceived(const stw_types::uint8 ou8_DataPoolIndex,
                                                 const stw_types::uint16 ou16_ListIndex,
                                                 const stw_types::uint16 ou16_ElementIndex);

private:
   QMap<stw_opensyde_core::C_OSCNodeDataPoolListElementId, QList<C_PuiSvDbDataElementHandler *> > mc_AllWidgets;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
