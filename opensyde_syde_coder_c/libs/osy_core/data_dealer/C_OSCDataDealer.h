//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Connects the openSYDE protocol driver to a node.

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCDATADEALERH
#define C_OSCDATADEALERH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>

#include "stwtypes.h"

#include "C_OSCNode.h"
#include "C_OSCDiagProtocolBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

///openSYDE data dealer
class C_OSCDataDealer
{
public:
   C_OSCDataDealer(void);
   virtual ~C_OSCDataDealer(void);
   C_OSCDataDealer(C_OSCNode * const opc_Node, const stw_types::uint32 ou32_NodeIndex,
                   C_OSCDiagProtocolBase * const opc_DiagProtocol);

   void Initialize(C_OSCNode * const opc_Node, const stw_types::uint32 ou32_NodeIndex,
                   C_OSCDiagProtocolBase * const opc_DiagProtocol);

   stw_types::uint32 GetNodeIndex(void) const;
   virtual stw_types::sint32 DataPoolRead(const stw_types::uint8 ou8_DataPoolIndex,
                                          const stw_types::uint16 ou16_ListIndex,
                                          const stw_types::uint16 ou16_ElementIndex,
                                          stw_types::uint8 * const opu8_NrCode);
   stw_types::sint32 DataPoolWrite(const stw_types::uint8 ou8_DataPoolIndex, const stw_types::uint16 ou16_ListIndex,
                                   const stw_types::uint16 ou16_ElementIndex, stw_types::uint8 * const opu8_NrCode);
   virtual stw_types::sint32 NvmRead(const stw_types::uint8 ou8_DataPoolIndex, const stw_types::uint16 ou16_ListIndex,
                                     const stw_types::uint16 ou16_ElementIndex, stw_types::uint8 * const opu8_NrCode);
   stw_types::sint32 NvmWrite(const stw_types::uint8 ou8_DataPoolIndex, const stw_types::uint16 ou16_ListIndex,
                              const stw_types::uint16 ou16_ElementIndex, stw_types::uint8 * const opu8_NrCode);

protected:
   //invoked when new data has been received and placed into the node's data pool
   virtual void m_OnReadDataPoolEventReceived(const stw_types::uint8 ou8_DataPoolIndex,
                                              const stw_types::uint16 ou16_ListIndex,
                                              const stw_types::uint16 ou16_ElementIndex);
   //invoked when new data has been received and placed into the node's data pool
   virtual void m_OnReadDataPoolEventErrorReceived(const stw_types::uint8 ou8_DataPoolIndex,
                                                   const stw_types::uint16 ou16_ListIndex,
                                                   const stw_types::uint16 ou16_ElementIndex,
                                                   const stw_types::uint8 ou8_ErrorCode);

   //invoked when new data has been received and placed into the node's data pool
   virtual void m_OnReadDataPoolNvmEventReceived(const stw_types::uint8 ou8_DataPoolIndex,
                                                 const stw_types::uint16 ou16_ListIndex,
                                                 const stw_types::uint16 ou16_ElementIndex);

   C_OSCNode * mpc_Node;
   stw_types::uint32 mu32_NodeIndex;
   C_OSCDiagProtocolBase * mpc_DiagProtocol;

private:
   C_OSCDataDealer(const C_OSCDataDealer & orc_Source);               //not implemented -> prevent copying
   C_OSCDataDealer & operator = (const C_OSCDataDealer & orc_Source); //not implemented -> prevent assignment

   void m_Init(void);

   static void mh_ReadDataPoolDataEventReceived(void * const opv_Instance, const stw_types::uint8 ou8_DataPoolIndex,
                                                const stw_types::uint16 ou16_ListIndex,
                                                const stw_types::uint16 ou16_ElementIndex,
                                                const std::vector<stw_types::uint8> & orc_Value);
   void m_ReadDataPoolDataEventReceived(const stw_types::uint8 ou8_DataPoolIndex,
                                        const stw_types::uint16 ou16_ListIndex,
                                        const stw_types::uint16 ou16_ElementIndex,
                                        const std::vector<stw_types::uint8> & orc_Value);

   static void mh_ReadDataPoolDataEventErrorReceived(void * const opv_Instance,
                                                     const stw_types::uint8 ou8_DataPoolIndex,
                                                     const stw_types::uint16 ou16_ListIndex,
                                                     const stw_types::uint16 ou16_ElementIndex,
                                                     const stw_types::uint8 ou8_NrCode);
   void m_ReadDataPoolDataEventErrorReceived(const stw_types::uint8 ou8_DataPoolIndex,
                                             const stw_types::uint16 ou16_ListIndex,
                                             const stw_types::uint16 ou16_ElementIndex,
                                             const stw_types::uint8 ou8_NrCode);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
