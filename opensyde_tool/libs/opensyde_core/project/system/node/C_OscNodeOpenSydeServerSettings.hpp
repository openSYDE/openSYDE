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
   C_OscNodeOpenSydeServerSettings(void);

   //lint -sem(stw::opensyde_core::C_OscNodeOpenSYDEServerSettings::Initialize,initializer)
   void Initialize(void);

   void CalcHash(uint32_t & oru32_HashValue) const;

   uint8_t u8_MaxClients;               ///< Maximum number of concurrent clients
   uint8_t u8_MaxParallelTransmissions; ///< Maximum number of parallel cyclic/event driven transmissions
   int16_t s16_DpdDataBlockIndex;       ///< Which application is Diagnostic Protocol Driver assigned to?
   ///< -1: Application not assigned
   ///< Else: Assigned application
   uint16_t u16_MaxMessageBufferTx; ///< CAN Tx routing FIFO size
   ///< (number of messages that can be buffered by in Tx direction)
   uint16_t u16_MaxRoutingMessageBufferRx; ///< CAN Rx routing FIFO size
   ///< (number of messages that can be buffered by in Rx direction)
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
