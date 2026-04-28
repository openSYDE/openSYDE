//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for openSYDE server specific settings (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCNODEOPENSYDESERVERSETTINGS_HPP
#define C_OSCNODEOPENSYDESERVERSETTINGS_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "stwtypes.hpp"
#include "C_OscNodeDataPool.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscNodeOpenSydeServerSettings
{
public:
   enum E_MaxServiceSizeModeType
   {
      eMSMT_AUTO,
      eMSMT_MANUAL
   };

   C_OscNodeOpenSydeServerSettings(void);

   //lint -sem(stw::opensyde_core::C_OscNodeOpenSYDEServerSettings::Initialize,initializer)
   void Initialize(void);

   void CalcHash(uint32_t & oru32_HashValue) const;

   uint32_t GetTransportBufferSizeInByte(const std::vector<C_OscNodeDataPool> & orc_DataPools,
                                         const uint16_t ou16_ProtocolDriverOsyMaximumServiceSize) const;

   static uint32_t h_GetTransportBufferSizeInByteWithAutoCalculation(
      const std::vector<C_OscNodeDataPool> & orc_DataPools,
      const uint16_t ou16_ProtocolDriverOsyMaximumServiceSize);

   uint8_t u8_MaxClients;               ///< Maximum number of concurrent clients
   uint8_t u8_MaxParallelTransmissions; ///< Maximum number of parallel cyclic/event driven transmissions
   int16_t s16_DpdDataBlockIndex;       ///< Which application is Diagnostic Protocol Driver assigned to?
   ///< -1: Application not assigned
   ///< Else: Assigned application
   uint16_t u16_MaxMessageBufferTx; ///< CAN Tx routing FIFO size
   ///< (number of messages that can be buffered by in Tx direction)
   uint16_t u16_MaxRoutingMessageBufferRx; ///< CAN Rx routing FIFO size
   ///< (number of messages that can be buffered by in Rx direction)
   E_MaxServiceSizeModeType e_MaxServiceSizeMode; ///< Mode of calculation of max service size
   uint16_t u16_MaxServiceSizeByte;               ///< Max service size in byte

   //Minimum buffer size required for DPD-services
   //greatest size for openSYDE server: WriteDataByIdentifier::SetKey
   //Calculation: (8 + 2 + 2) [ETH-Header] + 3 [WriteDataByIdentifierHeader] + (128 + 4 + 20) [SecurityKeyPayload]
   static const uint8_t hu8_MIN_SIZE_DPD_BUF_INSTANCE = 167U;

private:
   static uint32_t mh_GetSizeOfLargestDataPoolElementByte(const std::vector<C_OscNodeDataPool> & orc_DataPools);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
