//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for a node CAN communication via a specific CAN communication protocol (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCCANPROTOCOL_H
#define C_OSCCANPROTOCOL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include "stwtypes.h"
#include "C_OSCCanMessageContainer.h"
#include "C_OSCNodeDataPool.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

///Data class for a node CAN communication via a specific CAN communication protocol
class C_OSCCanProtocol
{
public:
   enum E_Type ///< type of process data communication protocol
   {
      eLAYER2,          ///< Data pool communication protocol OSI layer 2
      eCAN_OPEN_SAFETY, ///< Data pool communication protocol CAN open safety (safety protocol)
      eECES,            ///< Data pool communication protocol ECeS (safety protocol)
      eCAN_OPEN         ///< Data pool communication protocol CAN open manager
   };

   C_OSCCanProtocol(void);

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;
   stw_types::sint32 GetAllSignalsForMessage(const C_OSCNodeDataPool & orc_DataPool,
                                             const stw_types::uint32 ou32_InterfaceIndex,
                                             const stw_types::uint32 ou32_MessageIndex, const bool oq_IsTx,
                                             std::vector<const C_OSCNodeDataPoolListElement *> & orc_Signals) const;
   static const C_OSCNodeDataPoolList * h_GetComListConst(const C_OSCNodeDataPool & orc_DataPool,
                                                          const stw_types::uint32 ou32_InterfaceIndex,
                                                          const bool oq_IsTx);
   static stw_types::sint32 h_GetListIndex(const C_OSCNodeDataPool & orc_DataPool,
                                           const stw_types::uint32 ou32_InterfaceIndex, const bool oq_IsTx);
   static C_OSCNodeDataPoolList * h_GetComList(C_OSCNodeDataPool & orc_DataPool,
                                               const stw_types::uint32 ou32_InterfaceIndex, const bool oq_IsTx);
   static stw_types::sint32 h_GetComListIndex(const C_OSCNodeDataPool & orc_DataPool,
                                              const stw_types::uint32 ou32_InterfaceIndex, const bool oq_IsTx,
                                              stw_types::uint32 & oru32_ListIndex);
   const C_OSCNodeDataPoolListElement * GetComListElementConst(const C_OSCNodeDataPool & orc_DataPool,
                                                               const stw_types::uint32 ou32_InterfaceIndex,
                                                               const bool oq_IsTx,
                                                               const stw_types::uint32 ou32_MessageIndex,
                                                               const stw_types::uint32 ou32_SignalIndex) const;
   C_OSCNodeDataPoolListElement * GetComListElement(C_OSCNodeDataPool & orc_DataPool,
                                                    const stw_types::uint32 ou32_InterfaceIndex, const bool oq_IsTx,
                                                    const stw_types::uint32 ou32_MessageIndex,
                                                    const stw_types::uint32 ou32_SignalIndex) const;
   static bool h_ListIsComTx(const C_OSCNodeDataPoolList & orc_List);
   static stw_types::uint32 h_GetCANMessageValidSignalsDLCOffset(const E_Type oe_Type);
   static bool h_GetCANMessageSignalGapsValid(const E_Type oe_Type);

   E_Type e_Type;                                       ///< Communication protocol associated to this data pool
   stw_types::uint32 u32_DataPoolIndex;                 ///< Related data pool index
   std::vector<C_OSCCanMessageContainer> c_ComMessages; ///< Communication messages.
   ///< Vector size needs to be number of CAN interfaces.
   static const std::vector<C_OSCCanProtocol::E_Type> hc_ALL_PROTOCOLS; ///< Vector with all available CAN protocols, in
   /// the same order as the enum

private:
   static std::vector<C_OSCCanProtocol::E_Type> mh_GetAllProtocols(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
