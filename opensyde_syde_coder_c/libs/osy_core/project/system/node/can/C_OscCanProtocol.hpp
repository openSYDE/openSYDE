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
#include "stwtypes.hpp"
#include "C_OscCanMessageContainer.hpp"
#include "C_OscNodeDataPool.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

///Data class for a node CAN communication via a specific CAN communication protocol
class C_OscCanProtocol
{
public:
   enum E_Type ///< type of process data communication protocol
   {
      eLAYER2,          ///< Data pool communication protocol OSI layer 2
      eCAN_OPEN_SAFETY, ///< Data pool communication protocol CAN open safety (safety protocol)
      eECES,            ///< Data pool communication protocol ECeS (safety protocol)
      eCAN_OPEN         ///< Data pool communication protocol CAN open manager
   };

   C_OscCanProtocol(void);

   void CalcHash(uint32_t & oru32_HashValue) const;
   int32_t GetAllSignalsForMessage(const C_OscNodeDataPool & orc_DataPool, const uint32_t ou32_InterfaceIndex,
                                   const uint32_t ou32_MessageIndex, const bool oq_IsTx,
                                   std::vector<const C_OscNodeDataPoolListElement *> & orc_Signals) const;
   static const C_OscNodeDataPoolList * h_GetComListConst(const C_OscNodeDataPool & orc_DataPool,
                                                          const uint32_t ou32_InterfaceIndex, const bool oq_IsTx);
   static int32_t h_GetListIndex(const C_OscNodeDataPool & orc_DataPool, const uint32_t ou32_InterfaceIndex,
                                 const bool oq_IsTx);
   static C_OscNodeDataPoolList * h_GetComList(C_OscNodeDataPool & orc_DataPool, const uint32_t ou32_InterfaceIndex,
                                               const bool oq_IsTx);
   static int32_t h_GetComListIndex(const C_OscNodeDataPool & orc_DataPool, const uint32_t ou32_InterfaceIndex,
                                    const bool oq_IsTx, uint32_t & oru32_ListIndex);
   const C_OscNodeDataPoolListElement * GetComListElementConst(const C_OscNodeDataPool & orc_DataPool,
                                                               const uint32_t ou32_InterfaceIndex, const bool oq_IsTx,
                                                               const uint32_t ou32_MessageIndex,
                                                               const uint32_t ou32_SignalIndex) const;
   C_OscNodeDataPoolListElement * GetComListElement(C_OscNodeDataPool & orc_DataPool,
                                                    const uint32_t ou32_InterfaceIndex, const bool oq_IsTx,
                                                    const uint32_t ou32_MessageIndex,
                                                    const uint32_t ou32_SignalIndex) const;
   static bool h_ListIsComTx(const C_OscNodeDataPoolList & orc_List);
   static uint32_t h_GetCanMessageValidSignalsDlcOffset(const E_Type oe_Type);
   static bool h_GetCanMessageSignalGapsValid(const E_Type oe_Type);
   static bool h_GetCanMessageSignalByteAlignmentRequired(const E_Type oe_Type);
   static bool h_GetCanMessageSignalsRequired(const E_Type oe_Type);

   E_Type e_Type;                                       ///< Communication protocol associated to this data pool
   uint32_t u32_DataPoolIndex;                          ///< Related data pool index
   std::vector<C_OscCanMessageContainer> c_ComMessages; ///< Communication messages.
   ///< Vector size needs to be number of CAN interfaces.
   static const std::vector<C_OscCanProtocol::E_Type> hc_ALL_PROTOCOLS; ///< Vector with all available CAN protocols, in
   /// the same order as the enum

private:
   static std::vector<C_OscCanProtocol::E_Type> mh_GetAllProtocols(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
