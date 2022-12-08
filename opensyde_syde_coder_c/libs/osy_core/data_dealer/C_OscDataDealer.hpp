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

#include "stwtypes.hpp"

#include "C_OscNode.hpp"
#include "C_OscDiagProtocolBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

///openSYDE data dealer
class C_OscDataDealer
{
public:
   C_OscDataDealer(void);
   virtual ~C_OscDataDealer(void);
   C_OscDataDealer(C_OscNode * const opc_Node, const uint32_t ou32_NodeIndex,
                   C_OscDiagProtocolBase * const opc_DiagProtocol);

   void Initialize(C_OscNode * const opc_Node, const uint32_t ou32_NodeIndex,
                   C_OscDiagProtocolBase * const opc_DiagProtocol);

   uint32_t GetNodeIndex(void) const;
   virtual int32_t DataPoolRead(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                                const uint16_t ou16_ElementIndex, uint8_t * const opu8_NrCode);
   int32_t DataPoolWrite(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                         const uint16_t ou16_ElementIndex, uint8_t * const opu8_NrCode);
   virtual int32_t NvmRead(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                           const uint16_t ou16_ElementIndex, uint8_t * const opu8_NrCode);
   int32_t NvmWrite(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex, const uint16_t ou16_ElementIndex,
                    uint8_t * const opu8_NrCode);

protected:
   //Called by the DataDealer after it has placed the data in the data pool content structure associated with
   // the installed node.
   virtual void m_OnReadDataPoolEventReceived(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                                              const uint16_t ou16_ElementIndex);
   //called by the DataDealer after it has received an error response for a cyclic transmission.
   virtual void m_OnReadDataPoolEventErrorReceived(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                                                   const uint16_t ou16_ElementIndex, const uint8_t ou8_ErrorCode);

   //invoked when new data has been received and placed into the node's data pool
   virtual void m_OnReadDataPoolNvmEventReceived(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                                                 const uint16_t ou16_ElementIndex);

   C_OscNode * mpc_Node;
   uint32_t mu32_NodeIndex;
   C_OscDiagProtocolBase * mpc_DiagProtocol;

private:
   C_OscDataDealer(const C_OscDataDealer & orc_Source);               //not implemented -> prevent copying
   C_OscDataDealer & operator = (const C_OscDataDealer & orc_Source); //not implemented -> prevent assignment

   void m_Init(void);

   static void mh_ReadDataPoolDataEventReceived(void * const opv_Instance, const uint8_t ou8_DataPoolIndex,
                                                const uint16_t ou16_ListIndex, const uint16_t ou16_ElementIndex,
                                                const std::vector<uint8_t> & orc_Value);
   void m_ReadDataPoolDataEventReceived(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                                        const uint16_t ou16_ElementIndex, const std::vector<uint8_t> & orc_Value);

   static void mh_ReadDataPoolDataEventErrorReceived(void * const opv_Instance, const uint8_t ou8_DataPoolIndex,
                                                     const uint16_t ou16_ListIndex, const uint16_t ou16_ElementIndex,
                                                     const uint8_t ou8_NrCode);
   void m_ReadDataPoolDataEventErrorReceived(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                                             const uint16_t ou16_ElementIndex, const uint8_t ou8_NrCode);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
